################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/ble.c \
../Src/device.c \
../Src/gpio.c \
../Src/https.c \
../Src/i2c.c \
../Src/low_level_com.c \
../Src/main.c \
../Src/modem.c \
../Src/mqtt.c \
../Src/nvic.c \
../Src/pwr.c \
../Src/rtc.c \
../Src/swo.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_init.c \
../Src/system_stm32l0xx.c \
../Src/tim.c \
../Src/timebase.c \
../Src/uart.c \
../Src/wifi.c 

C_DEPS += \
./Src/adc.d \
./Src/ble.d \
./Src/device.d \
./Src/gpio.d \
./Src/https.d \
./Src/i2c.d \
./Src/low_level_com.d \
./Src/main.d \
./Src/modem.d \
./Src/mqtt.d \
./Src/nvic.d \
./Src/pwr.d \
./Src/rtc.d \
./Src/swo.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_init.d \
./Src/system_stm32l0xx.d \
./Src/tim.d \
./Src/timebase.d \
./Src/uart.d \
./Src/wifi.d 

OBJS += \
./Src/adc.o \
./Src/ble.o \
./Src/device.o \
./Src/gpio.o \
./Src/https.o \
./Src/i2c.o \
./Src/low_level_com.o \
./Src/main.o \
./Src/modem.o \
./Src/mqtt.o \
./Src/nvic.o \
./Src/pwr.o \
./Src/rtc.o \
./Src/swo.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_init.o \
./Src/system_stm32l0xx.o \
./Src/tim.o \
./Src/timebase.o \
./Src/uart.o \
./Src/wifi.o 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/grego/STM32CubeIDE/vn/embedded_device/Common" -I"/home/grego/STM32CubeIDE/vn/embedded_device/Testing" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Src

clean-Src:
	-$(RM) ./Src/adc.d ./Src/adc.o ./Src/ble.d ./Src/ble.o ./Src/device.d ./Src/device.o ./Src/gpio.d ./Src/gpio.o ./Src/https.d ./Src/https.o ./Src/i2c.d ./Src/i2c.o ./Src/low_level_com.d ./Src/low_level_com.o ./Src/main.d ./Src/main.o ./Src/modem.d ./Src/modem.o ./Src/mqtt.d ./Src/mqtt.o ./Src/nvic.d ./Src/nvic.o ./Src/pwr.d ./Src/pwr.o ./Src/rtc.d ./Src/rtc.o ./Src/swo.d ./Src/swo.o ./Src/syscalls.d ./Src/syscalls.o ./Src/sysmem.d ./Src/sysmem.o ./Src/system_init.d ./Src/system_init.o ./Src/system_stm32l0xx.d ./Src/system_stm32l0xx.o ./Src/tim.d ./Src/tim.o ./Src/timebase.d ./Src/timebase.o ./Src/uart.d ./Src/uart.o ./Src/wifi.d ./Src/wifi.o

.PHONY: clean-Src

