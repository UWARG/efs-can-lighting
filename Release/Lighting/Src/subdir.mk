################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Lighting/Src/conversions.cpp \
../Lighting/Src/lighting_controller.cpp \
../Lighting/Src/ws2812.cpp 

OBJS += \
./Lighting/Src/conversions.o \
./Lighting/Src/lighting_controller.o \
./Lighting/Src/ws2812.o 

CPP_DEPS += \
./Lighting/Src/conversions.d \
./Lighting/Src/lighting_controller.d \
./Lighting/Src/ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
Lighting/Src/%.o Lighting/Src/%.su Lighting/Src/%.cyclo: ../Lighting/Src/%.cpp Lighting/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../libcanard -I../libcanard/inc -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lighting-2f-Src

clean-Lighting-2f-Src:
	-$(RM) ./Lighting/Src/conversions.cyclo ./Lighting/Src/conversions.d ./Lighting/Src/conversions.o ./Lighting/Src/conversions.su ./Lighting/Src/lighting_controller.cyclo ./Lighting/Src/lighting_controller.d ./Lighting/Src/lighting_controller.o ./Lighting/Src/lighting_controller.su ./Lighting/Src/ws2812.cyclo ./Lighting/Src/ws2812.d ./Lighting/Src/ws2812.o ./Lighting/Src/ws2812.su

.PHONY: clean-Lighting-2f-Src

