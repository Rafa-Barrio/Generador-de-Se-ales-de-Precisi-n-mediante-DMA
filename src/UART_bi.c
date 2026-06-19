/*
 * UART_bi.c
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */
#include "UART_bi.h"
#include "LPC17xx_uart.h"
#include "variables_globales.h"
#include "dac_dma_signals.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h> // Para usar atof()
// Definición de los estados para la UART
typedef enum {
   ESPERANDO_COMANDO = 0,
   RECIBIENDO_A,
   RECIBIENDO_B,
   RECIBIENDO_C
} EstadoUart_t;
static EstadoUart_t estado_actual = ESPERANDO_COMANDO;
static float poly_a = 0.0f, poly_b = 0.0f, poly_c = 0.0f;
void UART_cfg(void){
	LPC_SC->PCONP |= (1 << 3);
	/*Conectamos los pines fisicos P0.2 y P0.3
	UART_PinConfig(UART_TX0_P0_2);
	UART_PinConfig(UART_RX0_P0_3);
	*/
	// 1. Configurar los pines P0.2 (TXD0) y P0.3 (RXD0)
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Portnum = PINSEL_PORT_0;
	pin_cfg.Funcnum = PINSEL_FUNC_1; // Función 1 = UART0
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinnum = PINSEL_PIN_2;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = PINSEL_PIN_3;
	PINSEL_ConfigPin(&pin_cfg);
	//Configuramos el contrato de comunicacion entre PC y Placa
	UART_CFG_Type cfgUART;
	cfgUART.Baud_rate = 9600;				// Velocidad pensada en el Informe
	cfgUART.Databits = UART_DATABIT_8;		// 8 bits de datos (Estandar ASCII)
	cfgUART.Parity   = UART_PARITY_NONE;	// Sin bit de paridad
	cfgUART.Stopbits = UART_STOPBIT_1;		// 1 bit de parada al final del paquete
	UART_FIFO_CFG_Type cfgFIFO;
	cfgFIFO.FIFO_ResetTxBuf = ENABLE;			  // Arrancamos con el buffer de transmision limpio
	cfgFIFO.FIFO_ResetRxBuf = ENABLE;			  // Arrancamos con el buffer de recepcion limpio
	cfgFIFO.FIFO_DMAMode    = DISABLE;			  // No usamos DMA para UART.
	cfgFIFO.FIFO_Level= UART_FIFO_TRGLEV0;   // Interrumpe cuando llega aunque sea 1 solo caracter
	//Inicializamos el periferico (Incluye PCONP y PCLKSEL)
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &cfgUART);
	//Aplicamos la configuracion de la FIFO
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &cfgFIFO);
	//Habilitamos las interrupciones por recepcion
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);

	//-------------------LINEA DE PRUEBA
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);
	//--------------------------------
	//Encendemos la Interrumpcion general de UART0 en el NVIC
	NVIC_EnableIRQ(UART0_IRQn);
}
void Consola_EnviaMensaje(char *mensaje){
	//Calculamos el largo de la cadena de char
	uint32_t largo = 0;
	while(mensaje[largo] != 0)largo++;
	//Enviamos el bloque. Aca podemos usar BLOCKING porque enviar unos comos bytes a 9600 baudios usando el FIFO de 16 bytes es rapidisimo
	UART_Send((LPC_UART_TypeDef *)LPC_UART0, (uint8_t *)mensaje, largo, BLOCKING);
}
void UART_SendString(LPC_UART_TypeDef* UARTx, char* str){
	uint32_t largo = 0;
	//Buscams el caracter nulo final que el compilador agrega automaticamente
	while(str[largo] != '\0'){
		largo++;
	}
	//Enviamos de forma bloqueante
	UART_Send((LPC_UART_TypeDef *)LPC_UART0, (uint8_t*)str, largo, BLOCKING);
}
void UART_SendNum(LPC_UART_TypeDef* UARTx, uint32_t numero){
	char digitos[10];
	int8_t i = 0;
	//caso base si el numero es exactamente cero
	if(numero == 0){
		UART_SendByte(UARTx,'0');
		return;
	}
	//Descomponemos el numero de atras para adelante usando modulo y division
	while(numero>0){
		digitos[i] = (numero%10)+ '0';   //Convertimos el digito a su valor ASCII
		numero = numero/10;
		i++;
	}
	//Como los guardamos al reves, los transmitimos desde el ultimo indice hasta el cero
	for (int8_t n = i - 1; n >= 0; n--) {
	    UART_SendByte(UARTx, digitos[n]);
	}
}
void UART_ReportarEstado(LPC_UART_TypeDef* UARTx, char* tipo_onda, uint32_t frecuencia, uint32_t amplitud){
	//Enviamos el tipo de onda
	UART_SendString(UARTx, "\r\n-- ESTADO GENERADOR ---\r\n");
	UART_SendString(UARTx, "Onda: ");
	UART_SendString(UARTx, tipo_onda);
	//Enviamos la frecuencia
	UART_SendString(UARTx, " | Freq: ");
	UART_SendNum(UARTx, frecuencia);
	UART_SendString(UARTx, " Hz");
	//Enviamos amplitud
	UART_SendString(UARTx, " | Amp: ");
	UART_SendNum(UARTx, amplitud);
	//Hacemos un salto de linea y retorno para dejar terminar limpio a la pc
	UART_SendString(UARTx, "\r\n-----------------------------------\r\n");
}
void UART0_IRQHandler(void) {
   uint32_t int_src = UART_GetIntId((LPC_UART_TypeDef *)LPC_UART0);
   // Verificamos si el origen de la interrupción es por recepción de datos (RDA o CTI)
   if ((int_src & 0x0E) == 0x04 || (int_src & 0x0E) == 0x0C) {
       // Leemos el carácter recibido (esto limpia la bandera de hardware de forma automática)
       char caracter = UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART0);
       // Si todavía tenemos espacio en el buffer de texto
       if (indice_rx < 49) {
           if (caracter == '\n' || caracter == '\r') {
               // Si el usuario apretó Enter, cerramos el string con el fin de cadena de C
               buffer_rx[indice_rx] = '\0';
               if (indice_rx > 0) {
                   comando_listo_flag = 1; // Avisamos al main() que hay trabajo por hacer
               }
           }
           else {
               // Almacenamos el carácter en el buffer y avanzamos el puntero
               buffer_rx[indice_rx] = caracter;
               indice_rx++;
           }
       }
       else {
           // Protección contra desbordes de memoria (Overflow protection)
           indice_rx = 0;
       }
   }
}
void ProcesarComando_UART(void) {
   // --- MÁQUINA DE ESTADOS PRINCIPAL ---
   if (estado_actual == ESPERANDO_COMANDO) {
       // Aquí adentro va tu lógica original de comandos fijos
       if (strcmp((char*)buffer_rx, "SENO") == 0) {
           tipo_senal_actual = 0;
           bloqueo_cambio_onda = 1;
       }
       else if (strcmp((char*)buffer_rx, "TRIANGULAR") == 0) {
           tipo_senal_actual = 1;
           bloqueo_cambio_onda = 1;
       }
       else if (strcmp((char*)buffer_rx, "CUADRADA") == 0) {
           tipo_senal_actual = 2;
           bloqueo_cambio_onda = 1;
       }
       else if (strcmp((char*)buffer_rx, "POLI") == 0) {
           // El usuario inicia la secuencia polinómica
           bloqueo_cambio_onda = 1;
           estado_actual = RECIBIENDO_A; // Avanzamos el "contador" al siguiente estado
       }
       else//(strcmp((char*)buffer_rx, "LIBERAR") == 0) {
       { bloqueo_cambio_onda = 0;
       }
   }
   else if (estado_actual == RECIBIENDO_A) {
       poly_a = (float)atof((char*)buffer_rx); // Convertimos el texto a float
       estado_actual = RECIBIENDO_B;           // Avanzamos el "contador"
   }
   else if (estado_actual == RECIBIENDO_B) {
       poly_b = (float)atof((char*)buffer_rx);
       estado_actual = RECIBIENDO_C;           // Avanzamos el "contador"
   }
   else if (estado_actual == RECIBIENDO_C) {
       poly_c = (float)atof((char*)buffer_rx);
       // ¡Llegó el último dato! Ejecutamos la matemática
       generar_onda_polinomial(poly_a, poly_b, poly_c);
       // Forzamos a que el sistema de actualización aplique los cambios
       tipo_senal_actual = 3; // Modo Arbitrario/Usuario
       // Reiniciamos la máquina volviendo al estado inicial
       estado_actual = ESPERANDO_COMANDO;
   }
   // Al final de la función (sin importar en qué estado estemos),
   // limpiamos el buffer de recepción para dejarlo listo para el próximo Enter
   memset(buffer_rx, 0, sizeof(buffer_rx));
   indice_rx = 0;
   comando_listo_flag = 0;
}
void generar_onda_polinomial(float a, float b, float c) {
   float x;
   float y_min = 999999.0f;
   float y_max = -999999.0f;
   float y_calculado[TAM_TABLA];
   // Paso 1: Calcular los valores puros del polinomio y encontrar los extremos
   // Normalizamos la X entre 0.0 y 1.0 para que los coeficientes sean fáciles de manejar
   for (int i = 0; i < TAM_TABLA; i++) {
       x = (float)i / (TAM_TABLA - 1); // X va de 0.0 a 1.0
       // Ecuación polinómica de segundo grado
       y_calculado[i] = (a * x * x) + (b * x) + c;
       // Buscamos el mínimo y máximo real calculados para poder auto-escalar la onda después
       if (y_calculado[i] > y_max) y_max = y_calculado[i];
       if (y_calculado[i] < y_min) y_min = y_calculado[i];
   }
   // Protección: Evitar división por cero si la onda es una línea plana
   if (y_max == y_min) {
       for (int i = 0; i < TAM_TABLA; i++) {
           tabla_usuario[i] = 512; // Línea media por defecto
       }
       return;
   }
   // Paso 2: Escalar y normalizar los datos mapeándolos estrictamente al rango del DAC (0 a 1023)
   for (int i = 0; i < TAM_TABLA; i++) {
       // Fórmula de mapeo analógico: (Valor - Min) * (Máximo_Deseado) / (Max_Real - Min_Real)
       float y_normalizado = (y_calculado[i] - y_min) * 1023.0f / (y_max - y_min);
       // Guardamos en la tabla de usuario convirtiendo a entero de 32 bits
       tabla_usuario[i] = (uint32_t)y_normalizado;
   }
   tipo_senal_actual = 3; // Cambiamos el estado a señal de "Usuario/Arbitraria"
}




