################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Peripherals/Src/bme280.c \
../Peripherals/Src/sgp30.c 

OBJS += \
./Peripherals/Src/bme280.o \
./Peripherals/Src/sgp30.o 

C_DEPS += \
./Peripherals/Src/bme280.d \
./Peripherals/Src/sgp30.d 


# Each subdirectory must supply rules for building sources it contributes
Peripherals/Src/%.o Peripherals/Src/%.su Peripherals/Src/%.cyclo: ../Peripherals/Src/%.c Peripherals/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DNUCLEO_L053R8 -DSTM32 -DSTM32L0 -DSTM32L053R8Tx -DSTM32L053xx -c -I../Inc -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/CMSIS/Device/ST/STM32L0xx/Include" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/CMSIS/Include" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Periphs/Include_periphs" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Peripherals" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Peripherals/Src" -I"/home/grego/STM32CubeIDE/workspace/STM32L0_Wi-Fi/Peripherals/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Peripherals-2f-Src

clean-Peripherals-2f-Src:
	-$(RM) ./Peripherals/Src/bme280.cyclo ./Peripherals/Src/bme280.d ./Peripherals/Src/bme280.o ./Peripherals/Src/bme280.su ./Peripherals/Src/sgp30.cyclo ./Peripherals/Src/sgp30.d ./Peripherals/Src/sgp30.o ./Peripherals/Src/sgp30.su

.PHONY: clean-Peripherals-2f-Src

