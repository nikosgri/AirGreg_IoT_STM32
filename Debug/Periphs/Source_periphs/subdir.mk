################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Periphs/Source_periphs/lcd204a.c 

OBJS += \
./Periphs/Source_periphs/lcd204a.o 

C_DEPS += \
./Periphs/Source_periphs/lcd204a.d 


# Each subdirectory must supply rules for building sources it contributes
Periphs/Source_periphs/%.o Periphs/Source_periphs/%.su Periphs/Source_periphs/%.cyclo: ../Periphs/Source_periphs/%.c Periphs/Source_periphs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DNUCLEO_L053R8 -DSTM32 -DSTM32L0 -DSTM32L053R8Tx -DSTM32L053xx -c -I../Inc -I"/home/grego/STM32CubeIDE/DataStructures/STM32L0_Wi-Fi/CMSIS/Device/ST/STM32L0xx/Include" -I"/home/grego/STM32CubeIDE/DataStructures/STM32L0_Wi-Fi/CMSIS/Include" -I"/home/grego/STM32CubeIDE/DataStructures/STM32L0_Wi-Fi/Periphs/Include_periphs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Periphs-2f-Source_periphs

clean-Periphs-2f-Source_periphs:
	-$(RM) ./Periphs/Source_periphs/lcd204a.cyclo ./Periphs/Source_periphs/lcd204a.d ./Periphs/Source_periphs/lcd204a.o ./Periphs/Source_periphs/lcd204a.su

.PHONY: clean-Periphs-2f-Source_periphs

