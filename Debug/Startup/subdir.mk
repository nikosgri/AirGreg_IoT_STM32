################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Startup/startup_stm32l053r8tx.s 

S_DEPS += \
./Startup/startup_stm32l053r8tx.d 

OBJS += \
./Startup/startup_stm32l053r8tx.o 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.s Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0plus -g3 -DDEBUG -c -I"/home/grego/STM32CubeIDE/workspace/AirGreg_IoT_STM32/Peripherals" -I"/home/grego/STM32CubeIDE/workspace/AirGreg_IoT_STM32/Peripherals/Src" -I"/home/grego/STM32CubeIDE/workspace/AirGreg_IoT_STM32/Peripherals/Inc" -I"/home/grego/STM32CubeIDE/workspace/AirGreg_IoT_STM32/CMSIS" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Startup

clean-Startup:
	-$(RM) ./Startup/startup_stm32l053r8tx.d ./Startup/startup_stm32l053r8tx.o

.PHONY: clean-Startup

