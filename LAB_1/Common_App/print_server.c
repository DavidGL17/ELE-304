/**
 * @file  print_server.c
 *
 * @date  2021-08-12
 * @author David González León, Jade Gröli
 *
 */
#include "cmsis_os2.h" /* CMSIS RTOS v2 header file */
#include "message_sending.h"
#include <print_server.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Define UART transfer completed flag */
#define UART_TX_COMPLETED 0x04

/* For convenience, define a pointer to UART handle */
UART_HandleTypeDef *print_server_uart_handle_p = NULL;

/* Declare PrintServer thread function */
void PrintServer(void *arg);
/* Define a memory block format */
typedef struct {
   uint8_t buff[PS_PRINT_BUFFER_SIZE];
} print_server_block_format_t;

/* Defined PrintServer memory pool id */
osMemoryPoolId_t print_server_pool_id = NULL;

/* Defined PrintServer message queue id */
osMessageQueueId_t print_server_msg_qid = NULL;

uint8_t sevFlag;

#pragma GCC optimize("O0")

volatile uint32_t ps_ready __attribute__((aligned(4))) __attribute__((section(".RAM_D3_SHM")));

// Define two buffer spaces for each core
#define SIZE_OF_POOL 1000
uint8_t pool_buf_cm4[SIZE_OF_POOL] __attribute__((aligned(32))) __attribute__((section(".RAM_D3_SHM")));
uint8_t pool_buf_cm7[SIZE_OF_POOL] __attribute__((aligned(32))) __attribute__((section(".RAM_D3_SHM")));

/* Define PrintServer thread id */
osThreadId_t print_server_thread_id = NULL;

uint8_t *cm7_sev_buff_pointer;

bool PrintServerInitCommon() {
   /* Define PrintServer thread attributes */
   osThreadAttr_t print_server_thread_attributes = {.name = "PrintServerThread", .priority = osPriorityNormal};
#ifdef CORE_CM4 // If on core CM4
   /* Define PrintServer memory pool attributes */
   osMemoryPoolAttr_t print_server_pool_attributes = {.name = "PrintServerPoolCM4"};
#else
   /* Define PrintServer memory pool attributes */
   osMemoryPoolAttr_t print_server_pool_attributes = {.name = "PrintServerPoolCM7", .mp_mem = pool_buf_cm7, .mp_size = SIZE_OF_POOL};
#endif
   /* Define PrintServer message queue attributes */
   osMessageQueueAttr_t print_server_message_queue_attributes = {.name = "PrintServerMessageQueue"};

   /* Create PrintServer memory pool */
   print_server_pool_id = osMemoryPoolNew(PS_POOL_AND_MESSAGE_QUEUE_SIZE, sizeof(print_server_block_format_t), &print_server_pool_attributes);
   if (print_server_pool_id == NULL)
      return false;

   /* Create PrintServer message queue */
   print_server_msg_qid = osMessageQueueNew(PS_POOL_AND_MESSAGE_QUEUE_SIZE, sizeof(uint32_t), &print_server_message_queue_attributes);
   if (print_server_msg_qid == NULL)
      return false;

   /* Create PrintServer thread */
   print_server_thread_id = osThreadNew(PrintServer, NULL, &print_server_thread_attributes);
   if (print_server_thread_id == NULL)
      return false;

   return true;
}

#ifdef CORE_CM7
bool PrintServerInit() { return PrintServerInitCommon(); }

void cleanInitPrintServer(void) { ps_ready = 0; }

#else
bool PrintServerInit(void *huart) {
   if (huart == NULL) {
      return false;
   }

   /* Store UART handle to Use by PrintServer */
   print_server_uart_handle_p = (UART_HandleTypeDef *)huart;
   ps_ready = 1;
   sevFlag = 0;
   return PrintServerInitCommon();
}
#endif

static void PrintServerCRLF(char *const buf) {
   uint32_t len = strlen(buf);

   if ((len > 1) && (((buf[len - 2] == '\r') && (buf[len - 1] == '\n')) || ((buf[len - 2] == '\n') && (buf[len - 1] == '\r')))) {
      return;
   }

   if ((buf[len - 1] == '\r') || (buf[len - 1] == '\n')) {
      len--;
   }

   if (len == PS_PRINT_BUFFER_SIZE - 1U) {
      len -= 2U;
   } else if (len == PS_PRINT_BUFFER_SIZE - 2U) {
      len -= 1U;
   }
   buf[len] = '\r';
   len++;
   buf[len] = '\n';
   len++;
   buf[len] = '\0';
}

void PrintServerPrintf(const char *fmt, ...) {
   /* va_list is a type to hold information about variable arguments */
   va_list args;

   osStatus_t status = osError;

   /* Allocate a memory pool block */
   print_server_block_format_t *block_p = (print_server_block_format_t *)osMemoryPoolAlloc(print_server_pool_id, 0);

   if (block_p == NULL) {
      /* Pool is empty, bail out */
      return;
   }

   /* va_start must be called before accessing variable argument list */
   va_start(args, fmt);

   /* The vsnprintf() function formats and stores a series of characters and
    * values in the buffer target-string. The vsnprintf() function works just
    * like the snprintf() function, except that arg_ptr points to a list of
    * arguments whose number can vary from call to call in the program. These
    * arguments should be initialized by the va_start function for each call.
    * In contrast, the snprintf() function can have a list of arguments, but
    * the number of arguments in that list is fixed when you compile the program.
    *
    * The vsnprintf() function converts each entry in the argument list according
    * to the corresponding format specifier in format. The format has the same form
    * and function as the format string for the printf() function. */
   vsnprintf((char *)block_p->buff, PS_PRINT_BUFFER_SIZE, fmt, args);

   /* va_end should be executed before the function returns whenever
    * va_start has been previously used in that function */
   va_end(args);

   /* Make sure formatted printout ends with \r\n */
   PrintServerCRLF((char *)block_p->buff);

   /* Put the formatted printout on to the message queue */
   status = osMessageQueuePut(print_server_msg_qid, &block_p, 0, 0);

   if (status != osOK) {
      /* Free pool buffer and bail out */
      osMemoryPoolFree(print_server_pool_id, block_p);
   }
}

void sendToUART(uint8_t *buff) {
   if (HAL_UART_Transmit_DMA(print_server_uart_handle_p, buff, strlen((char *)buff)) == HAL_OK) {
      uint32_t flag = osThreadFlagsWait(UART_TX_COMPLETED, osFlagsWaitAny, osWaitForever);
      if ((flag & osFlagsError) || !(flag & UART_TX_COMPLETED)) {
         PrintServerPrintf("Error: Received unexpected flag 0x%x", flag);
      }
   }
}

void PrintServer(void *arg) {
   while (1) {
      uint32_t msg;
      print_server_block_format_t *block_p;
      osStatus_t status = osMessageQueueGet(print_server_msg_qid, (void *)&msg, NULL, osWaitForever);
      if (status == osOK) {
#ifdef CORE_CM4 // If on core CM4
         if (msg == 1 && sevFlag) {
            sendToUART(cm7_sev_buff_pointer);
            sevFlag = 0;
            cm7_sev_buff_pointer = 0;
            ps_ready = 1;
         } else {
            block_p = (print_server_block_format_t *)msg;
            sendToUART(block_p->buff);
            osMemoryPoolFree(print_server_pool_id, block_p);
         }
#else // if on core CM7
         ps_ready = 0;
         block_p = (print_server_block_format_t *)msg;
         sendMessage(CORE_MESSAGE_CM7_CM4_NEW_PRINT, (void *)block_p->buff);
         while (!ps_ready)
            ; // Wait for message to be sent before freeing the pool
         osMemoryPoolFree(print_server_pool_id, block_p);
#endif
      }
   }
}

void SEVMessageHandling(uint8_t *buff) {
   cm7_sev_buff_pointer = buff;
   sevFlag = 1;
   uint32_t msg = 1;
   osMessageQueuePut(print_server_msg_qid, (void *)&msg, 0, 0);
}

/* Take over the weak defined HAL_UART_TxCpltCallback() */
#define PrintServerTxCompletedCB HAL_UART_TxCpltCallback

void PrintServerTxCompletedCB(UART_HandleTypeDef *huart) {
   if (huart == print_server_uart_handle_p) {
      /* Set PrintServer thread flag */
      osThreadFlagsSet(print_server_thread_id, UART_TX_COMPLETED);
   }
}

/* End of print_server.c */
