################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
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

OBJS += \
./Src/adc.o \
./Src/ble.o \
./Src/device.o \
./Src/gpio.o \
./Src/https.o \
./Src/i2c.o \
./Src/low_level_com.o \
./Src/main.o \
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

C_DEPS += \
./Src/adc.d \
./Src/ble.d \
./Src/device.d \
./Src/gpio.d \
./Src/https.d \
./Src/i2c.d \
./Src/low_level_com.d \
./Src/main.d \
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


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DNUCLEO_L053R8 -DSTM32 -DSTM32L0 -DSTM32L053R8Tx -DSTM32L053xx -c -I../Inc -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/CMSIS/Device/ST/STM32L0xx/Include" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/CMSIS/Include" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Periphs/Include_periphs" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Peripherals" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Peripherals/Src" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Peripherals/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/adc.cyclo ./Src/adc.d ./Src/adc.o ./Src/adc.su ./Src/ble.cyclo ./Src/ble.d ./Src/ble.o ./Src/ble.su ./Src/device.cyclo ./Src/device.d ./Src/device.o ./Src/device.su ./Src/gpio.cyclo ./Src/gpio.d ./Src/gpio.o ./Src/gpio.su ./Src/https.cyclo ./Src/https.d ./Src/https.o ./Src/https.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/low_level_com.cyclo ./Src/low_level_com.d ./Src/low_level_com.o ./Src/low_level_com.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/mqtt.cyclo ./Src/mqtt.d ./Src/mqtt.o ./Src/mqtt.su ./Src/nvic.cyclo ./Src/nvic.d ./Src/nvic.o ./Src/nvic.su ./Src/pwr.cyclo ./Src/pwr.d ./Src/pwr.o ./Src/pwr.su ./Src/rtc.cyclo ./Src/rtc.d ./Src/rtc.o ./Src/rtc.su ./Src/swo.cyclo ./Src/swo.d ./Src/swo.o ./Src/swo.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/system_init.cyclo ./Src/system_init.d ./Src/system_init.o ./Src/system_init.su ./Src/system_stm32l0xx.cyclo ./Src/system_stm32l0xx.d ./Src/system_stm32l0xx.o ./Src/system_stm32l0xx.su ./Src/tim.cyclo ./Src/tim.d ./Src/tim.o ./Src/tim.su ./Src/timebase.cyclo ./Src/timebase.d ./Src/timebase.o ./Src/timebase.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su ./Src/wifi.cyclo ./Src/wifi.d ./Src/wifi.o ./Src/wifi.su

.PHONY: clean-Src

