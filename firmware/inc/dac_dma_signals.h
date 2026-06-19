/*
 * dac_dma_signals.h
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

#ifndef DAC_DMA_SIGNALS_H_
#define DAC_DMA_SIGNALS_H_
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#define TAM_TABLA 512
/* Estructura LLI para DMA Circular (Requerimiento Hardware LPC1769) */
typedef struct {
   uint32_t SourceAddr;
   uint32_t DestAddr;
   uint32_t NextLLI;
   uint32_t Control;
} DMA_LLI_Struct;
/* Funciones de Inicialización y Control */
void signals_init(void);
void signals_generar_tablas_fijas(void);
void signals_actualizar_amplitud(void);
#endif /* DAC_DMA_SIGNALS_H_ */


