/*
 * encoder_amp.c
 *
 *  Created on: 18 jun. 2026
 *      Author: USUARIO
 */


/*
* encoder_amp.c
*
*  Created on: 15 jun. 2026
*      Author: Usuario
*/
#include "encoder_amp.h"
#include "variables_globales.h"
#include "dac_dma_signals.h"
//Inicializa los pines del encoder con Pull-up y habilita interrupcion en P2.0
void encoder_init(void){
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Portnum= ENCODER_PORT;
	pin_cfg.Funcnum= PINSEL_FUNC_0;
	pin_cfg.Pinmode= PINSEL_PINMODE_PULLUP;
	pin_cfg.Pinnum= ENCODER_PIN_A;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum= ENCODER_PIN_B;
	PINSEL_ConfigPin(&pin_cfg);
	//Setear ambos pines como entradas
	GPIO_SetDir(ENCODER_PORT, (1<<ENCODER_PIN_A) | (1<<ENCODER_PIN_B), 0);
	//Habilitar interrupcion por GPIO en el P2.0 por flanco de bajada
	LPC_GPIOINT->IO2IntEnF |= (1 << ENCODER_PIN_A);
	//Habilito handler de interrupcion EINT3 (aunque los botones ya lo hicieron, pero no rompe nada)
	NVIC_EnableIRQ(EINT3_IRQn);
}
//Funcion que procesa el giro y guarda info
void encoder_procesar_giro(void){


	//Leemos el estado del canal B (P2.1) para saber el sentido de giro
	uint32_t estado_canal_b= GPIO_ReadValue(ENCODER_PORT) & (1 << ENCODER_PIN_B);
	if(estado_canal_b !=0){
		//Sentido horario: subimos amplitud
		if(amplitud_porcentaje <100){	//verificamos si no llegamos al tope
			amplitud_porcentaje += 5;	//subimos de a 5%
			if(amplitud_porcentaje > 100) amplitud_porcentaje=100;	//tope
		}
	}
	else{
		//Sentido antihorario: bajamos amplitud
		if(amplitud_porcentaje >0){
			if(amplitud_porcentaje >= 5){	//verifico que no esté por llegar al fondo
				amplitud_porcentaje -= 5;	//disminuto 5%
			}
			else{
				amplitud_porcentaje=0;
			}
		}
	}
	//Re-calculamos la tabla de DMA con la nueva amplitud

	//Avisamos main que actualice amplitud en LCD
	flag_actualizar_lcd=1;
}

//ACTUALIZAMOS CÓDIGO EN control_botones xq ahí está EINT3 handler



