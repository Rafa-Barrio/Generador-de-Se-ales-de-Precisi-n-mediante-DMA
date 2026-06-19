/*
 * ADC_Pote.c
 *
 *  Created on: 15 jun. 2026
 *      Author: USUARIO
 */

/*
* ADC_Pote.c
*
*  Created on: 13 jun. 2026
*      Author: Usuario
*/
#include "ADC_Pote.h"
#include "variables_globales.h" // Para actualizar 'frecuencia_actual' y 'flag_actualizar_lcd'
#include "lpc17xx_dac.h"
// Variable estática local para almacenar la última lectura válida y filtrar el ruido
static uint32_t ultima_lectura_adc = 0;
//Configuracion del periferico en canal 0
void adc_pot_init(void){
	LPC_SC->PCONP |= (1 << 12);
	PINSEL_CFG_Type pin_conf;
	pin_conf.Portnum=0;
	pin_conf.Pinnum=23;
	pin_conf.Funcnum=1;
	pin_conf.Pinmode= PINSEL_PINMODE_TRISTATE;	//Modo analogico (sin pull-up ni pull-down)
	pin_conf.OpenDrain= PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_conf);
	//Inicializamos el ADC a tasa de muestreo estandar de 200KHz
	ADC_Init(LPC_ADC, 200000);
	//Habilitamos canal 0
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
}
//funcion para realizar interpolacion lineal (mapeo matemático)
static uint32_t mapear_adc_a_frecuencia(uint32_t valor_adc){
	//regla de tres simple, 0=Freq_min y 4095=freq_max
	return FREQ_MIN_HZ + ((valor_adc * (FREQ_MAX_HZ - FREQ_MIN_HZ)) / ADC_MAX_VAL);
}
//funcion para convertir la frecuencia en Herts a ciclos de reloj para el timeout
uint32_t calcular_timeout_dac(uint32_t frecuencia_hz){
	if(frecuencia_hz==0) return 0xFFFF;	//proteccion contra division por cero
	uint32_t timeout_ciclos= 25000000/(frecuencia_hz * PUNTOS_TABLA);
	//El registro timeout del DAC es de 16bits, limitamos el maximo permitido
	if(timeout_ciclos > 65535){
		timeout_ciclos=65535;
	}
	return timeout_ciclos;
}
//Lee el potenciometro, aplica filtro de ruido y actualiza hardware del DAC
void adc_pot_actualizar(void){
	uint32_t lectura_actual=0;
	int32_t diferencia=0;
	// Si el usuario tomó el control remoto por UART, bloqueamos la lectura del potenciómetro
	    /*if (bloqueo_cambio_onda == 1) {
	        return;
	    }
	    */
	//Iniciamos conversion de forma manual
	ADC_StartCmd(LPC_ADC, ADC_START_NOW);
	//Esperamos por Polling hasta que termine la conversion
	while(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE) == RESET);





	//Leemos el dato
	lectura_actual=ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
	//Filtro de Histéresis (antiruido)
	diferencia= (int32_t)lectura_actual - (int32_t)ultima_lectura_adc;
	//obtenemos el valor absoluto de la diferencia
	if(diferencia < 0){
		diferencia= -diferencia;
	}
	//Filtro solo procesa el cambio si el potenciometro se movió más de 15 unidades
	//evita que el ruido electrico de protoboard
	if(diferencia > 15){
		ultima_lectura_adc = lectura_actual;
		//Paso 1: transformar los bits crudos en Hertz reales
		uint32_t nueva_frecuencia = mapear_adc_a_frecuencia(lectura_actual);
		//Paso 2: convertir Hertz en ciclos de reloj
		uint32_t nuevo_timeout= calcular_timeout_dac(nueva_frecuencia);
		//Paso 3: pasar la nueva frecuencia al DAC
		DAC_SetDMATimeOut(LPC_DAC, nuevo_timeout);
		//actualizar variables globales de estado para el main y LCD
		frecuencia_actual= nueva_frecuencia;
		flag_actualizar_lcd=1;
	}
}
