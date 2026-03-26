################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Testing/test_main.cpp 

CPP_DEPS += \
./Testing/test_main.d 

OBJS += \
./Testing/test_main.o 


# Each subdirectory must supply rules for building sources it contributes
Testing/%.o: ../Testing/%.cpp Testing/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/grego/STM32CubeIDE/vn/embedded_device/Common" -I"/home/grego/STM32CubeIDE/vn/embedded_device/Testing" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Testing

clean-Testing:
	-$(RM) ./Testing/test_main.d ./Testing/test_main.o

.PHONY: clean-Testing

