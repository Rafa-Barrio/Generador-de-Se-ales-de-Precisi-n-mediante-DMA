/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#include "LPC17xx.h"
#include "LPC17xx_uart.h"
#include "string.h"
#include <stdio.h>

#include "../inc/ADC_Pote.h"
#include "../inc/botones_control.h"
#include "../inc/dac_dma_signals.h"
#include "../inc/encoder_amp.h"
#include "../inc/pantalla_LCD.h"
#include "../inc/UART_bi.h"
#include "../inc/variables_globales.h" // Incluimos las declaraciones
// AQUÍ SE CREAN FÍSICAMENTE EN MEMORIA AL ARRANCAR EL MICRO:
volatile uint32_t frecuencia_actual   = 1000; // Arranca en 1 kHz
volatile uint8_t  amplitud_porcentaje = 100;  // Arranca al 100%
volatile uint8_t  tipo_senal_actual   = 0;    // Arranca en modo Senoidal
//--------------------LINEA DE PRUEBA
//volatile uint8_t  flag_actualizar_lcd = 1;
//----------------------------------------
volatile uint8_t  flag_actualizar_lcd = 1;    // 1 para que dibuje la pantalla la primera vez
volatile uint8_t  bloqueo_cambio_onda = 0;    // Arranca desbloqueado (pote activo)
volatile uint8_t  comando_listo_flag  = 0;
char buffer_rx[50];
volatile uint8_t  indice_rx           = 0;
int main(void) {
	//variable local para detectar cambio de tipo de señal
	uint8_t ultima_senal_procesada = 0;
	//CONFIGURACION DEL SISTEMA
	SystemInit();
	//--- INICIALIZACION DE MÓDULOS DE GENERACION ---
	signals_generar_tablas_fijas();	//Genera las tablas con señales y guarda en memoria
	signals_actualizar_amplitud();	//genera el buffer de salida con amplitud inicial
	signals_init();		//Configura DAC y enciende el DMA con señal senoidal
	//--- INICIALIZACION DE CONTROLES ---
	adc_pot_init();
	botones_init();
	encoder_init();
	//--- SETEO DE PRIORIDAD ---
	//maxima prioridad a los botones y encoder (EINT3)
	NVIC_SetPriority(EINT3_IRQn, 0);
	NVIC_SetPriority(UART0_IRQn, 1);
	//--- INICIALIZA COMUNICACION Y VISUALIZACION ---
	//init de UART
	UART_cfg();
	//init de LCD
	lcd_pins_init();
	LCD_Init();
	LCD_EscribirFrase("Generador Imagen");
	LCD_EnviarByte(0xC0, 1); // Pasar al segundo renglón
	LCD_EscribirFrase("Inicializando...");
	for(volatile int i = 0; i < 10000000; i++);

   while(1) {
   	//EVENTO A: procesar comandos remotos provenientes de la PC
   	if(comando_listo_flag ==1){
   		ProcesarComando_UART();
   	}
   	//EVENTO B: leer constantemente el potenciometro de frecuencia
   	        adc_pot_actualizar();
   	//EVENTO C: detector de cambio de forma de onda
   	//Si los botones físicos o la UART modificaron 'tipo_senal_actual'
   	// recalculamos de inmediato el buffer del DMA
   	if(tipo_senal_actual != ultima_senal_procesada || flag_actualizar_lcd == 1){
   		signals_actualizar_amplitud();	//regenera la Tabla_Salida_DMA con la nueva señal
   		ultima_senal_procesada = tipo_senal_actual;	//actualizamos
   		//flag_actualizar_lcd = 1;
   		//EVENTO D: Refresco del LCD
   		Actualizar_Telemetria();
   		flag_actualizar_lcd=0;
   	}
   }
   return 0 ;
}
