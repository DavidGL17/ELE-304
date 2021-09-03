################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core_App_CM4/ADC_app.c \
../Core_App_CM4/App.c \
../Core_App_CM4/print_server.c 

OBJS += \
./Core_App_CM4/ADC_app.o \
./Core_App_CM4/App.o \
./Core_App_CM4/print_server.o 

C_DEPS += \
./Core_App_CM4/ADC_app.d \
./Core_App_CM4/App.d \
./Core_App_CM4/print_server.d 


# Each subdirectory must supply rules for building sources it contributes
Core_App_CM4/%.o: ../Core_App_CM4/%.c Core_App_CM4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H745xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/xxpow/OneDrive/HEIG/5emeSemestre/ELE-304/Workspace/LAB_1/CM4/Core_App_CM4" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

