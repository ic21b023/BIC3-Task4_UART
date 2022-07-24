################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyLibrary/Src/mylib_serialprot.c 

OBJS += \
./MyLibrary/Src/mylib_serialprot.o 

C_DEPS += \
./MyLibrary/Src/mylib_serialprot.d 


# Each subdirectory must supply rules for building sources it contributes
MyLibrary/Src/%.o MyLibrary/Src/%.su: ../MyLibrary/Src/%.c MyLibrary/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/reite/STM32CubeIDE/workspace_1.7.0/BIC3-Task4_UART/MyLibrary/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MyLibrary-2f-Src

clean-MyLibrary-2f-Src:
	-$(RM) ./MyLibrary/Src/mylib_serialprot.d ./MyLibrary/Src/mylib_serialprot.o ./MyLibrary/Src/mylib_serialprot.su

.PHONY: clean-MyLibrary-2f-Src

