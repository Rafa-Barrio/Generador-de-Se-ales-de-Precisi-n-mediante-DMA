/*
 * encoder_amp.h
 *
 *  Created on: 18 jun. 2026
 *      Author: USUARIO
 */

#ifndef ENCODER_AMP_H_
#define ENCODER_AMP_H_
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
// Definición de pines para el Puerto 2
#define ENCODER_PORT    PINSEL_PORT_2
#define ENCODER_PIN_A   PINSEL_PIN_0   // P2.0 (Interrupción)
#define ENCODER_PIN_B   PINSEL_PIN_1   // P2.1 (Lectura de dirección)
// Prototipos de funciones públicas
void encoder_init(void);
void encoder_procesar_giro(void);
#endif /* ENCODER_AMP_H_ */
