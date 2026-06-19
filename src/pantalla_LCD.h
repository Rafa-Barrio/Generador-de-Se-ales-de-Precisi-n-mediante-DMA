/*
 * pantalla_LCD.h
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

#ifndef PANTALLA_LCD_H_
#define PANTALLA_LCD_H_
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#endif
#include <stdint.h>
#define LCD_PIN_RS 3
#define LCD_PIN_E 4
void LCD_Init(void);
void LCD_Pulso(void);
void LCD_EnviarByte(uint8_t dato, uint8_t es_comando);
void LCD_EscribirFrase(char *frase);
void Actualizar_Telemetria(void);
void lcd_pins_init(void) ;
#endif /* PANTALLA_LCD_H_ */
