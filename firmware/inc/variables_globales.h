/*
 * variables_globales.h
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

/* Declaracion de variables globales para todos los archivos
*
*/
/* Declaracion de variables globales para todos los archivos
*
*/
#ifndef VARIABLES_GLOBALES_H_
#define VARIABLES_GLOBALES_H_
#include "LPC17xx.h"
// ============================================================================
// VARIABLES DE ESTADO DEL INSTRUMENTO (Compartidas entre módulos)
// ============================================================================
// Frecuencia actual de la onda en Hertz (La lee el LCD, la escribe el ADC o la UART)
extern volatile uint32_t frecuencia_actual;
// Amplitud actual en porcentaje de 0 a 100 (La lee el DAC/DMA, la escribe el Encoder o la UART)
extern volatile uint8_t  amplitud_porcentaje;
// Tipo de onda activa: 0=Seno, 1=Triángulo, 2=Cuadrada, 3=Arbitraria/Usuario
extern volatile uint8_t  tipo_senal_actual;
// ============================================================================
// FLAGS DE SINCRONIZACIÓN (Para evitar bloqueos en el Main)
// ============================================================================
// Se pone en 1 cuando cambia algo (Frecuencia, Amplitud o Tipo de Onda) para avisarle al LCD que redibuje
extern volatile uint8_t  flag_actualizar_lcd;
// Se pone en 1 si el usuario configura el micro por UART. Bloquea el potenciómetro físico.
extern volatile uint8_t  bloqueo_cambio_onda;
// Se pone en 1 adentro del Handler de la UART cuando se detecta el fin de una cadena (\n)
extern volatile uint8_t  comando_listo_flag;
// ============================================================================
// BUFFERS DE MEMORIA
// ============================================================================
// El buffer de texto donde la interrupción de la UART va acumulando los caracteres que llegan de la PC
extern char buffer_rx[50];
// El índice que indica en qué posición del buffer de la UART estamos escribiendo
extern volatile uint8_t  indice_rx;
extern uint32_t tabla_usuario[512];
#endif /* VARIABLES_GLOBALES_H_ */
