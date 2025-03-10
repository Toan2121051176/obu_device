################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/nmea_p/nmea_parse.c \
../src/nmea_p/uart_mqtt.c 

C_DEPS += \
./src/nmea_p/nmea_parse.d \
./src/nmea_p/uart_mqtt.d 

OBJS += \
./src/nmea_p/nmea_parse.o \
./src/nmea_p/uart_mqtt.o 

SREC += \
project_with_rtos.srec 

MAP += \
project_with_rtos.map 


# Each subdirectory must supply rules for building sources it contributes
src/nmea_p/%.o: ../src/nmea_p/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"D:/Download/project_with_rtos_12125/src" -I"." -I"D:/Download/project_with_rtos_12125/ra/fsp/inc" -I"D:/Download/project_with_rtos_12125/ra/fsp/inc/api" -I"D:/Download/project_with_rtos_12125/ra/fsp/inc/instances" -I"D:/Download/project_with_rtos_12125/ra/fsp/src/rm_freertos_port" -I"D:/Download/project_with_rtos_12125/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"D:/Download/project_with_rtos_12125/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"D:/Download/project_with_rtos_12125/ra_gen" -I"D:/Download/project_with_rtos_12125/ra_cfg/fsp_cfg/bsp" -I"D:/Download/project_with_rtos_12125/ra_cfg/fsp_cfg" -I"D:/Download/project_with_rtos_12125/ra_cfg/aws" -I"D:/Download/project_with_rtos_12125/src/icons" -I"D:/Download/project_with_rtos_12125/src/nmea_p" -I"D:/Download/project_with_rtos_12125/src/LCD" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

