/*
 * pantalla_LCD.c
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */
#include "../inc/pantalla_LCD.h"

#include <string.h>
#include <stdlib.h>

#include "../inc/UART_bi.h"
#include "../inc/variables_globales.h"
// NUEVAS MÁSCARAS: Una para el bloque bajo (bits 5 al 8) y otra para el bloque alto (bits 10 al 13)
#define MASK_DATOS_BAJOS  (0x0F << 5)   // Cubre P2.5, P2.6, P2.7, P2.8
#define MASK_DATOS_ALTOS  (0x0F << 10)  // Cubre P2.10, P2.11, P2.12, P2.13
#define MASK_TOTAL_LCD    (MASK_DATOS_BAJOS | MASK_DATOS_ALTOS)
void lcd_pins_init(void) {
   PINSEL_CFG_Type pin_cfg;
   pin_cfg.Portnum   = PINSEL_PORT_2;
   pin_cfg.Funcnum   = PINSEL_FUNC_0;         // GPIO
   pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
   pin_cfg.Pinmode   = PINSEL_PINMODE_PULLUP;
   // Configurar RS (P2.3) y E (P2.4)
   pin_cfg.Pinnum  = LCD_PIN_RS;
   PINSEL_ConfigPin(&pin_cfg);
   GPIO_SetDir(PINSEL_PORT_2, (1 << LCD_PIN_RS), 1);
   pin_cfg.Pinnum  = LCD_PIN_E;
   PINSEL_ConfigPin(&pin_cfg);
   GPIO_SetDir(PINSEL_PORT_2, (1 << LCD_PIN_E), 1);
   // Configurar D0-D3 (P2.5 al P2.8)
   for (uint8_t i = 5; i <= 8; i++) {
       pin_cfg.Pinnum = i;
       PINSEL_ConfigPin(&pin_cfg);
       GPIO_SetDir(PINSEL_PORT_2, (1 << i), 1);
   }
   // EL PIN 2.9 NO SE TOCA Lo salteamos olimpicamente.
   // Configurar D4-D7 (P2.10 al P2.13)
   for (uint8_t i = 10; i <= 13; i++) {
       pin_cfg.Pinnum = i;
       PINSEL_ConfigPin(&pin_cfg);
       GPIO_SetDir(PINSEL_PORT_2, (1 << i), 1);
   }
}
void LCD_Pulso(void){
   LPC_GPIO2->FIOSET = (1 << LCD_PIN_E);              // P2.4 en ALTO
   for(volatile int i = 0; i < 5000; i++);
   LPC_GPIO2->FIOCLR = (1 << LCD_PIN_E);              // P2.4 en BAJO
   for(volatile int i = 0; i < 5000; i++);
}
void LCD_EnviarByte(uint8_t dato, uint8_t es_comando){
   if(es_comando == 1){
       LPC_GPIO2->FIOCLR = (1 << LCD_PIN_RS);          // RS = 0
   } else {
       LPC_GPIO2->FIOSET = (1 << LCD_PIN_RS);          // RS = 1
   }
   // 1. Limpiamos solo las dos islas de pines de la LCD (dejando el P2.9 intacto)
   LPC_GPIO2->FIOCLR = MASK_TOTAL_LCD;
   // 2. Acomodamos los 4 bits bajos (D0-D3) en el rango P2.5 a P2.8
   uint32_t parte_baja = (dato & 0x0F) << 5;
   // 3. Acomodamos los 4 bits altos (D4-D7) en el rango P2.10 a P2.13
   // Como los bits ya están en la parte alta del byte (posiciones 4-7),
   // para llevarlos a las posiciones 10-13 solo hay que correrlos 6 lugares.
   uint32_t parte_alta = ((dato >> 4) & 0x0F) << 10;
   // 4. Encendemos los pines correspondientes del tirón
   LPC_GPIO2->FIOSET = (parte_baja | parte_alta);
   LCD_Pulso();
}
void LCD_Init(void){
   // Espera de 50ms para estabilidad eléctrica del LCD
   for(volatile int i = 0; i < 500000; i++);
   // Inicialización nativa en modo de 8 bits
   LCD_EnviarByte(0x38, 1);                        // Comando de control: 8 bits, 2 líneas
   for(volatile int i = 0; i < 5000; i++);         // Pequeña espera de asentamiento
   LCD_EnviarByte(0x0C, 1);                        // Display ON, Cursor OFF
   LCD_EnviarByte(0x06, 1);                        // Escribir de izquierda a derecha
   LCD_EnviarByte(0x01, 1);                        // Limpiar pantalla por completo
   for(volatile int i = 0; i < 50000; i++);        // Espera larga obligatoria para el borrado
}
void LCD_EscribirFrase(char *frase){
   uint8_t i = 0;
   while(frase[i] != '\0'){
       LCD_EnviarByte(frase[i], 0);                // Envia el caracter con RS = 1
       i++;
   }
}
void LCD_EscribirNum(uint32_t numero) {
   char buffer_num[10];
   itoa(numero, buffer_num, 10);                   // Convertimos entero a ASCII nativo
   LCD_EscribirFrase(buffer_num);
}
void Actualizar_Telemetria(void){
   char* nombre_onda;
   switch(tipo_senal_actual){
       case 0:  nombre_onda = "SENO";   break;
       case 1:  nombre_onda = "TRIANG"; break;
       case 2:  nombre_onda = "CUADR";  break;
       case 3:  nombre_onda = "USER";    break;
       default: nombre_onda = "DESK";  break;
   }
   // 1. Reporte a la PC
   UART_ReportarEstado((LPC_UART_TypeDef *)LPC_UART0, nombre_onda, frecuencia_actual, amplitud_porcentaje);
   // 2. Reporte a la LCD
   LCD_EnviarByte(0x01, 1);                        // Limpiamos pantalla antes de sobreescribir
   for(volatile int i = 0; i < 20000; i++);
   // Renglón 1: Onda
   LCD_EscribirFrase("Onda: ");
   LCD_EscribirFrase(nombre_onda);
   // Pasamos al renglón 2 (Dirección de memoria 0xC0 en el HD44780)
   LCD_EnviarByte(0xC0, 1);
   // Renglón 2: Frecuencia y Amplitud
   LCD_EscribirFrase("F:");
   LCD_EscribirNum(frecuencia_actual);
   LCD_EscribirFrase("Hz A:");
   LCD_EscribirNum(amplitud_porcentaje);
   LCD_EscribirFrase("%");
}
