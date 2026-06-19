################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../firmware/src/ADC_Pote.c \
../firmware/src/ProyectoFinal.c \
../firmware/src/UART_bi.c \
../firmware/src/control_botones.c \
../firmware/src/crp.c \
../firmware/src/dac_dma_signals.c \
../firmware/src/encoder_amp.c \
../firmware/src/pantalla_LCD.c 

C_DEPS += \
./firmware/src/ADC_Pote.d \
./firmware/src/ProyectoFinal.d \
./firmware/src/UART_bi.d \
./firmware/src/control_botones.d \
./firmware/src/crp.d \
./firmware/src/dac_dma_signals.d \
./firmware/src/encoder_amp.d \
./firmware/src/pantalla_LCD.d 

OBJS += \
./firmware/src/ADC_Pote.o \
./firmware/src/ProyectoFinal.o \
./firmware/src/UART_bi.o \
./firmware/src/control_botones.o \
./firmware/src/crp.o \
./firmware/src/dac_dma_signals.o \
./firmware/src/encoder_amp.o \
./firmware/src/pantalla_LCD.o 


# Each subdirectory must supply rules for building sources it contributes
firmware/src/%.o: ../firmware/src/%.c firmware/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\USUARIO\Documents\MCUXpressoIDE_25.6.136\workspace\CMSISv2p00_LPC17xx\inc" -I"C:\Users\USUARIO\Documents\MCUXpressoIDE_25.6.136\workspace\CMSISv2p00_LPC17xx\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-firmware-2f-src

clean-firmware-2f-src:
	-$(RM) ./firmware/src/ADC_Pote.d ./firmware/src/ADC_Pote.o ./firmware/src/ProyectoFinal.d ./firmware/src/ProyectoFinal.o ./firmware/src/UART_bi.d ./firmware/src/UART_bi.o ./firmware/src/control_botones.d ./firmware/src/control_botones.o ./firmware/src/crp.d ./firmware/src/crp.o ./firmware/src/dac_dma_signals.d ./firmware/src/dac_dma_signals.o ./firmware/src/encoder_amp.d ./firmware/src/encoder_amp.o ./firmware/src/pantalla_LCD.d ./firmware/src/pantalla_LCD.o

.PHONY: clean-firmware-2f-src

