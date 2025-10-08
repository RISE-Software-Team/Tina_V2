################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App_Tina/Utils/logger.c 

OBJS += \
./App_Tina/Utils/logger.o 

C_DEPS += \
./App_Tina/Utils/logger.d 


# Each subdirectory must supply rules for building sources it contributes
App_Tina/Utils/%.o App_Tina/Utils/%.su App_Tina/Utils/%.cyclo: ../App_Tina/Utils/%.c App_Tina/Utils/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I"/home/krissal1234/Documents/projects/rise/Tina_V2/Tina_v2/Drivers/BSP/STM32WLxx_Nucleo" -I../SubGHz_Phy/App -I../SubGHz_Phy/Target -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/trace/adv_trace -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I"/home/krissal1234/Documents/projects/rise/Tina_V2/Tina_v2/App_Tina/Utils" -I"/home/krissal1234/Documents/projects/rise/Tina_V2/Tina_v2/App_Tina/FSM" -I"/home/krissal1234/Documents/projects/rise/Tina_V2/Tina_v2/App_Tina/Sensors" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-App_Tina-2f-Utils

clean-App_Tina-2f-Utils:
	-$(RM) ./App_Tina/Utils/logger.cyclo ./App_Tina/Utils/logger.d ./App_Tina/Utils/logger.o ./App_Tina/Utils/logger.su

.PHONY: clean-App_Tina-2f-Utils

