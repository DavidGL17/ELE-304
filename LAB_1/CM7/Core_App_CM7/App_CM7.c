/**
 * @file App_CM7.c
 *
 * @brief This file defines the main functions of the CM7 App
 *
 * @date 5 sept. 2021
 * @author David González León, Jade Gröli
 */

#include "App_CM7.h"
#include "DSP_handler.h"
#include "mdma_handler.h"
#include "message_sending.h"
#include "print_server.h"

void app_CM7();

void preOsStartupInit() {
   cleanInitMessageSending();
   cleanInitPrintServer();
}

bool AppCM7Init() {
   bool result = messageSendingInit();
   result &= PrintServerInit();

   // Launch main app thread
   osThreadAttr_t app_CM7_attributes = {.name = "app_CM7", .priority = osPriorityNormal};
   osThreadNew(app_CM7, NULL, &app_CM7_attributes);

   if (result) {
      PRINTF("CM7 message sending and print server correctly initialized");
   }

   if (MDMA_init()) {
      PRINTF("MDMA initialized correctly");
   }

   if (DSP_init()) {
      PRINTF("DSP initialized correctly");
   }

   return true;
}

void app_CM7() {
   while (1) {
   }
}
