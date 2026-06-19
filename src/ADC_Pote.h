/*
 * ADC_Pote.h
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

/*
* Declaracion de variables que usamos en ADC_Pote.c y que puede leer el main
*/
#ifndef ADC_POTE_H_
#define ADC_POTE_H_
#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
// --- CONFIGURACIONES DEL INSTRUMENTO ---
#define ADC_MAX_VAL      4095      // El ADC es de 12 bits (0 a 4095)
#define FREQ_MIN_HZ      10         // Frecuencia mínima del instrumento: 5 Hz
#define FREQ_MAX_HZ      1000     // Frecuencia máxima del instrumento: 10 kHz (10 MHz)
#define CCLK_HZ          100000000 // Reloj del sistema: 100 MHz
#define PUNTOS_TABLA     512       // Tamaño uniforme de nuestras señales
// --- PROTOTIPOS DE FUNCIONES ---
void adc_pot_init(void);
void adc_pot_actualizar(void);
uint32_t calcular_timeout_dac(uint32_t frecuencia_hz);
#endif /* ADC_POTE_H_ */




