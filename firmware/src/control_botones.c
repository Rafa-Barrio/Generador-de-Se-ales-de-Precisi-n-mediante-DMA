/*
 * control_botones.c
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */


/*
* botones_control.c
*
*  Created on: 13 jun. 2026
*      Author: Usuario
*/
#include "../inc/botones_control.h"
#include "../inc/dac_dma_signals.h" // Para forzar el recalculo de la amplitud al cambiar de onda
#include "../inc/variables_globales.h"
extern void encoder_procesar_giro(void);
//Inicializa los pines de los botones con Pull-Up y habilita interrupciones por flanco de bajada
void botones_init(void) {
   PINSEL_CFG_Type pin_cfg;
   // Configurar pines P0.0, P0.1 y P0.4 como GPIO comunes (Función 0)
   pin_cfg.Portnum = BOTON_PORT;
   pin_cfg.Funcnum = PINSEL_FUNC_0;
   pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP; // Habilitamos resistencia de Pull-Up interna
   pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
   // Aplicamos la configuración pin por pin
   pin_cfg.Pinnum = PIN_SENO;
   PINSEL_ConfigPin(&pin_cfg);
   pin_cfg.Pinnum = PIN_TRIANG;
   PINSEL_ConfigPin(&pin_cfg);
   pin_cfg.Pinnum = PIN_CUAD;
   PINSEL_ConfigPin(&pin_cfg);
   // Configurar la dirección de los pines como ENTRADAS
   GPIO_SetDir(BOTON_PORT, (1 << PIN_SENO) | (1 << PIN_TRIANG) | (1 << PIN_CUAD), 0);
   // Habilitar la interrupción por GPIO para el Puerto 0 en flanco de bajada (Falling Edge)
   // en el driver no podemos hacer |
   LPC_GPIOINT->IO0IntEnF |= (1 << PIN_SENO) | (1 << PIN_TRIANG) | (1 << PIN_CUAD);
   // Habilitar el vector de interrupción EINT3
   NVIC_EnableIRQ(EINT3_IRQn);
}
//Handler de interrupción encargado de capturar las pulsaciones de los botones
void EINT3_IRQHandler(void) {
   // Si el cambio de onda está bloqueado de forma remota por la UART, ignoramos los botones físicos
   if (bloqueo_cambio_onda == 1) {
       // Limpiamos las banderas de interrupción por las dudas y salimos
       LPC_GPIOINT->IO0IntClr = (1 << PIN_SENO) | (1 << PIN_TRIANG) | (1 << PIN_CUAD);
       return;
   }
   //Interrupcion por puerto 0 ?
   if(LPC_GPIOINT ->IntStatus & 0x01){
   // --- BOTÓN 1: SEÑAL SENOIDAL (P0.0) ---
   if (LPC_GPIOINT->IO0IntStatF & (1 << PIN_SENO)) {
           tipo_senal_actual = 0; // 0 = Senoidal
       LPC_GPIOINT->IO0IntClr = (1 << PIN_SENO); // Limpiamos la bandera de interrupción de este pin
   }
   // --- BOTÓN 2: SEÑAL TRIANGULAR (P0.1) ---
   if (LPC_GPIOINT->IO0IntStatF & (1 << PIN_TRIANG)) {
           tipo_senal_actual = 1; // 1 = Triangular
       LPC_GPIOINT->IO0IntClr = (1 << PIN_TRIANG);
   }
   // --- BOTÓN 3: SEÑAL CUADRADA (P0.2) ---
   if (LPC_GPIOINT->IO0IntStatF & (1 << PIN_CUAD)) {
           tipo_senal_actual = 2; // 2 = Cuadrada
       LPC_GPIOINT->IO0IntClr = (1 << PIN_CUAD);
   }
   }
   //Interrupcion por puerto 2 (encoder) ?
   if(LPC_GPIOINT ->IntStatus & 0x04){
   	if(LPC_GPIOINT ->IO2IntStatF &(1<<0)){
   		encoder_procesar_giro();	//llamamos lógica del encoder
   		LPC_GPIOINT->IO2IntClr= (1<<0);	//limpio bandera
   	}
   }
}


