/*
 * dac_dma_signals.c
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

#include "../inc/dac_dma_signals.h"

#include "lpc17xx_clkpwr.h"
#include <math.h>

#include "../inc/variables_globales.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
/* Tablas Maestras */
uint32_t tabla_seno[TAM_TABLA];
uint32_t tabla_triang[TAM_TABLA];
uint32_t tabla_cuad[TAM_TABLA];
uint32_t tabla_usuario[TAM_TABLA];
/* Buffer activo para el DMA (Alineado en memoria para eficiencia) */
uint32_t tabla_salida_dma[TAM_TABLA] __attribute__((aligned(4)));
/* Descriptor de la Lista Ligada para modo circular */
GPDMA_LLI_Type LLI_Circular;
//rellena las 3 tablas
void signals_generar_tablas_fijas(void) {
   for (int i = 0; i < TAM_TABLA; i++) {
       // 1. Senoidal (0 a 1023)
       tabla_seno[i] = (uint32_t)((sin(2 * M_PI * i / TAM_TABLA) + 1) * 511.5);
       // 2. Triangular
       if (i < TAM_TABLA / 2)
           tabla_triang[i] = (uint32_t)((i * 1023) / (TAM_TABLA / 2));	//hace paso para arriba
       else
           tabla_triang[i] = (uint32_t)(1023 - ((i - TAM_TABLA / 2) * 1023) / (TAM_TABLA / 2));	//hace paso para abajo
       // 3. Cuadrada
       tabla_cuad[i] = (i < TAM_TABLA / 2) ? 1023 : 0;	//si es menos a 512/2 se mantiene arriba, si es mayor mantiene 0
   }
}
void signals_actualizar_amplitud(void) {
   uint32_t *tabla_ptr;
   float gain = (float)amplitud_porcentaje / 100.0f;
   // Selector de tabla origen
   switch(tipo_senal_actual) {
       case 0: tabla_ptr = tabla_seno; break;
       case 1: tabla_ptr = tabla_triang; break;
       case 2: tabla_ptr = tabla_cuad; break;
       case 3: tabla_ptr = tabla_usuario; break;
   }
   for (int i = 0; i < TAM_TABLA; i++) {
          int32_t val = (int32_t)tabla_ptr[i] - 512;
          val = (int32_t)(val * gain);
          uint32_t dac_val = (uint32_t)(val + 512);
          // Formato DACR: Bits [15:6] contienen el dato de 10 bits
          tabla_salida_dma[i] = (dac_val & 0x3FF) << 6;
      }
}
void signals_init(void) {
	PINSEL_CFG_Type pin_cfg;
	    // CONDICIÓN DE PIN: Configurar el pin P0.26 con su función alternativa 2 (AOUT)
	    pin_cfg.Portnum   = PINSEL_PORT_0;
	    pin_cfg.Pinnum    = PINSEL_PIN_26;
	    pin_cfg.Funcnum   = PINSEL_FUNC_2;         // Función 2 = AOUT (Salida Analógica del DAC)
	    pin_cfg.Pinmode   = PINSEL_PINMODE_TRISTATE; // Modo Tristate (Sin pull-up ni pull-down para no deformar la onda)
	    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	    PINSEL_ConfigPin(&pin_cfg);
	    // INICIALIZACIÓN: Ahora el DAC está energizado y su pin asociado está listo
	    DAC_Init(LPC_DAC);
   GPDMA_Channel_CFG_Type dma_channel_config;
   // Habilitar de forma combinada el modo Time-out y el canal de DMA del DAC
   DAC_CONVERTER_CFG_Type dac_config;
   dac_config.CNT_ENA = SET; // Activa el contador interno de timeout
   dac_config.DMA_ENA = SET; // Permite que el DAC pida datos al DMA
   DAC_ConfigDAConverterControl(LPC_DAC, &dac_config);
   //CONFIGURAR LA ESTRUCTURA LLI PARA EL MODO CIRCULAR
   // La estructura LLI le dice al DMA de dónde sacar el dato y a dónde llevarlo
   LLI_Circular.SrcAddr  = (uint32_t)tabla_salida_dma;
   LLI_Circular.DstAddr  = (uint32_t)&(LPC_DAC->DACR);
   LLI_Circular.NextLLI  = (uint32_t)&LLI_Circular;
   LLI_Circular.Control  = TAM_TABLA
                         | (GPDMA_WIDTH_WORD << 18)  // Origen de 32 bits
                         | (GPDMA_WIDTH_WORD << 21)  // Destino de 32 bits
                         | (1 << 26);                // Incrementar dirección de origen
   //INICIALIZAR EL CONTROLADOR GENERAL GPDMA
   GPDMA_Init();
   //CONFIGURAR EL CANAL 0 DEL DMA
   dma_channel_config.ChannelNum    = 0;                         // Canal 0 de uso general
   dma_channel_config.TransferSize  = TAM_TABLA;                 // 512 puntos
   dma_channel_config.TransferWidth = GPDMA_WIDTH_WORD;          // Bloques de 32 bits
   dma_channel_config.SrcMemAddr    = (uint32_t)tabla_salida_dma;// Puntero RAM
   dma_channel_config.DstMemAddr    = 0;                         // 0 porque el destino es un periférico
   dma_channel_config.TransferType  = GPDMA_TRANSFERTYPE_M2P;    // Memory to Peripheral
   dma_channel_config.SrcConn       = 0;                         // No aplica conexión de origen
   dma_channel_config.DstConn       = GPDMA_CONN_DAC;            // Destino enlazado al hardware del DAC
   dma_channel_config.DMALLI        = (uint32_t)&LLI_Circular;   // Enlazamos nuestra LLI circular
   // Aplicar la configuración al canal 0 y encender el motor
   GPDMA_Setup(&dma_channel_config);
   GPDMA_ChannelCmd(0, ENABLE);
}


