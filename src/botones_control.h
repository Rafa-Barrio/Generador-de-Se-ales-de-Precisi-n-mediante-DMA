/*
 * botones_control.h
 *
 *  Created on: 17 jun. 2026
 *      Author: USUARIO
 */

#ifndef BOTONES_CONTROL_H_
#define BOTONES_CONTROL_H_
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h" // Driver para configurar pines
#include "lpc17xx_gpio.h"   // Driver para manejo de GPIO
// Definición de pines para los 3 botones en el Puerto 0
#define BOTON_PORT      PINSEL_PORT_0
#define PIN_SENO        PINSEL_PIN_0
#define PIN_TRIANG      PINSEL_PIN_1
#define PIN_CUAD        PINSEL_PIN_4
// Prototipo de la función de inicialización
void botones_init(void);
#endif /* BOTONES_CONTROL_H_ */


