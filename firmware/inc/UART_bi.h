/*
 * UART_bi.h
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

#ifndef UART_BI_H_
#define UART_BI_H_
#include "LPC17xx.h"
#include "LPC17xx_uart.h"
#include "lpc17xx_pinsel.h"
void UART_cfg(void);
void Consola_EnviaMensaje(char *mensaje);
void UART_SendString(LPC_UART_TypeDef* UARTx, char* str);
void UART_SendNum(LPC_UART_TypeDef* UARTx, uint32_t numero);
void UART_ReportarEstado(LPC_UART_TypeDef* UARTx, char* tipo_onda, uint32_t frecuencia, uint32_t amplitud);
void ProcesarComando_UART(void);
void generar_onda_polinomial(float a, float b, float c);
#endif /* UART_BI_H_ */
