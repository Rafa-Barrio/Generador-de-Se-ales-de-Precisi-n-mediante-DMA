################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ADC_Pote.c \
../src/ProyectoFinal.c \
../src/UART_bi.c \
../src/control_botones.c \
../src/crp.c \
../src/dac_dma_signals.c \
../src/encoder_amp.c \
../src/pantalla_LCD.c 

C_DEPS += \
./src/ADC_Pote.d \
./src/ProyectoFinal.d \
./src/UART_bi.d \
./src/control_botones.d \
./src/crp.d \
./src/dac_dma_signals.d \
./src/encoder_amp.d \
./src/pantalla_LCD.d 

OBJS += \
./src/ADC_Pote.o \
./src/ProyectoFinal.o \
./src/UART_bi.o \
./src/control_botones.o \
./src/crp.o \
./src/dac_dma_signals.o \
./src/encoder_amp.o \
./src/pantalla_LCD.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\USUARIO\Documents\MCUXpressoIDE_25.6.136\workspace\CMSISv2p00_LPC17xx\inc" -I"C:\Users\USUARIO\Documents\MCUXpressoIDE_25.6.136\workspace\CMSISv2p00_LPC17xx\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/ADC_Pote.d ./src/ADC_Pote.o ./src/ProyectoFinal.d ./src/ProyectoFinal.o ./src/UART_bi.d ./src/UART_bi.o ./src/control_botones.d ./src/control_botones.o ./src/crp.d ./src/crp.o ./src/dac_dma_signals.d ./src/dac_dma_signals.o ./src/encoder_amp.d ./src/encoder_amp.o ./src/pantalla_LCD.d ./src/pantalla_LCD.o

.PHONY: clean-src

