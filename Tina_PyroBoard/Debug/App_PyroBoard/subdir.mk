################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App_PyroBoard/pyro_fsm.c 

OBJS += \
./App_PyroBoard/pyro_fsm.o 

C_DEPS += \
./App_PyroBoard/pyro_fsm.d 


# Each subdirectory must supply rules for building sources it contributes
App_PyroBoard/%.o App_PyroBoard/%.su App_PyroBoard/%.cyclo: ../App_PyroBoard/%.c App_PyroBoard/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F042x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I"/home/krissal1234/Documents/projects/rise/Tina_V2/Tina_PyroBoard/App_PyroBoard" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-App_PyroBoard

clean-App_PyroBoard:
	-$(RM) ./App_PyroBoard/pyro_fsm.cyclo ./App_PyroBoard/pyro_fsm.d ./App_PyroBoard/pyro_fsm.o ./App_PyroBoard/pyro_fsm.su

.PHONY: clean-App_PyroBoard

