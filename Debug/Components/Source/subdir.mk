################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/Source/lcd204a.c 

OBJS += \
./Components/Source/lcd204a.o 

C_DEPS += \
./Components/Source/lcd204a.d 


# Each subdirectory must supply rules for building sources it contributes
Components/Source/%.o Components/Source/%.su Components/Source/%.cyclo: ../Components/Source/%.c Components/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DNUCLEO_L053R8 -DSTM32 -DSTM32L0 -DSTM32L053R8Tx -DSTM32L053xx -c -I../Inc -I"/home/grego/STM32CubeIDE/workspace_1.13.1/STM32L0_Wi-Fi/CMSIS/Device/ST/STM32L0xx/Include" -I"/home/grego/STM32CubeIDE/workspace_1.13.1/STM32L0_Wi-Fi/CMSIS/Include" -I"/home/grego/STM32CubeIDE/workspace_1.13.1/STM32L0_Wi-Fi/Components/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Components-2f-Source

clean-Components-2f-Source:
	-$(RM) ./Components/Source/lcd204a.cyclo ./Components/Source/lcd204a.d ./Components/Source/lcd204a.o ./Components/Source/lcd204a.su

.PHONY: clean-Components-2f-Source

