# Generador de Señales de Precisión mediante DMA
> **Asignatura:** Electrónica Digital III - Universidad Nacional de Córdoba
> **Integrantes:**
> * Rafael Barrio
> * Lianca Belén Aimar Ortellado
> **Profesor:** Marcos Blasco

---

## 🚀 1. Descripción General del Proyecto (Común a DII y DIII)
La idea central de este proyecto consiste en el diseño e implementación de un Generador de Señales de Precisión controlado digitalmente mediante un microcontrolador de arquitectura ARM Cortex-M3 (LPC1769). La arquitectura principal se basa en el uso del periférico de Acceso Directo a Memoria (DMA). Mediante la configuración de estructuras de datos circulares enlazadas (LLI) el sistema se encarga de transferir de forma autónoma los arreglos de datos de las ondas almacenadas en la memoria RAM directamente hacia el registro de datos del DAC. Al delegar completamente el flujo de datos al bus del DMA, la CPU queda totalmente liberada durante la generación de la señal, permitiendo que el sistema responda a eventos externos sin distorsionar la onda de salida.

Se presentan dos interacciones principales con el usuario. Por un lado, el control de parámetros analógicos, utilizando 1 canal del ADC para muestrear las posiciones de un potenciómetro modificando la frecuencia de la señal y un encoder que por interrupción de GPIO modifica dinámicamente la amplitud. Por otro lado, una interfaz de comandos digitales a través del periférico UART, donde el sistema expone una consola de comandos a 9600 baudios que procesa cadenas de texto en tiempo real para seleccionar el tipo de onda o formar una desde cero. Finalmente, se integra una etapa de acondicionamiento analógico externo para superar las limitaciones nativas del DAC.

### 🎯 Alcances del Proyecto (¿Qué hace y qué NO hace el sistema?)
* **El sistema SÍ es capaz de:** Generar ondas de tipo Senoidal, Triangular, Cuadrada y de usuario en un rango de frecuencias de 10Hz a 1KHz con una resolución de 10 bits (0 a 1023). Procesar comandos por consola serie UART a 9600 baudios, y acondicionar la salida mediante un amplificador operacional en configuración de acoplamiento activo para remover la componente de continua y obtener una señal simétrica respecto a cero.
* **El sistema NO incluye (Fuera de alcance):** Conectividad inalámbrica ni barrido de frecuencias automático.

### ⏩ Posibles Etapas Siguientes (Líneas Futuras)
* Desarrollar una interfaz gráfica propia para no depender de Programas externos (ej:putty).
* Aumentar el rango de frecuencias posibles para las señales de salida.
* Expandir la memoria para poder tener mejor resolución.

---

## 📐 2. Arquitectura del Sistema: Hardware y Software (Común)

### 🔌 Hardware & Interconexión
* **Diagrama de Bloques:** (en docs)
* **Esquemático del Circuito:** (en docs)
* **Descripción del Circuito y Consideraciones de Diseño:** 
* **Descripción del circuito:** posee la placa LPC1769 del fabricante NXP, amplificador operacional LM358, Encoder Rotacional, Potenciómetro (10Kohms), resistencias de 4,7k,10k y 100k, Pantalla LCD 16x2, capacitor de 10uF, pulsadores y cables dupont
* **Consideraciones de diseño:** Dado que la salida nativa del DAC del microcontrolador se encuentra limitada al rango de tensiones de alimentación digital (0 V a 3.3 V) y posee una baja capacidad de corriente, se integra una etapa de acondicionamiento analógico externo. Utilizando un amplificador operacional en configuración de acoplamiento activo, se remueve la componente de continua, y se ajusta la ganancia de tensión para obtener una señal simétrica respecto a cero.

### 💻 Arquitectura de Software (Firmware)
* **Diagrama de Flujo o Máquina de Estados:** (en docs)



---

## ⚡ 3. Especificaciones Eléctricas, Alimentación y Entorno (Específico por Asignatura)

### 🔌 Parámetros de Alimentación y Consumo (Común a ambas materias)
* **Tensión de operación del sistema:** 5V
* **Método de alimentación:** Alimentación por fuente de 5V
* **Consumo estimado o medido:** 1.2mA aprox

* **IDE y SDK:** MCUXpresso IDE v25.6.136 con CMSIS v2.00
* **Microcontrolador Principal:** NXP LPC1769
* **Periféricos Avanzados Utilizados:** NVIC, ADC, DAC, DMA
* **Estrategia de Concurrencia:** Bare-Metal con máquina de estados cooperativa



## 🔄 4. Proceso de Integración y Desarrollo (Común)
* **Organización del programa:**

**Etapa 1:**
1. declaración de variables externas
    
2. dac_dma_signals: 
    

- generar las tablas de señales y rellenarlas 
    
- función para actualizar la amplitud y luego actualizar la salida del DMA con la señal correspondiente
    
- configurar DAC y DMA con sus respectivas LLIs
    
**Etapa 2:**

3. UART: configuración, handler de interrupción y función para procesar lo recibido
    
4. ADC: configuración, y funciones para procesar lo convertido. Aplicamos filtros de histéresis para cancelar el ruido del potenciómetro. 
    
5. EINT encoder: configuración de interrupción por GPIO, procesamos el giro y calculamos la amplitud.
    
6. control_botones: interrupción por botones para cambio de señal
    
7. LCD: configuración para funcionamiento y actualización de la pantalla LCD
    
8. main: loop de chequear banderas de UART, cambio de señal y refresco de pantalla LCD.

**Etapa 3:**

10. Integración lógica de los módulos, acondicionamiento de prioridades de interrupción y testeo de funcionamiento integral 

**Etapa 4:**

11. Test de Sistema completo y sometimiento a pruebas de estrés 

---

## 📊 5. Ensayos, Pruebas y Resultados (Común)
En la carpeta Docs esta adjunta la evidencia fotografica.
Se realizaron pruebas funcionales sobre la pantalla LCD, sobre el UART y Amplificador Operacional:

**Pantalla LCD:** Se uso el modulo pantalla_LCD pero se diseño un main para inyectar datos harcodeados y lograr testear el funcionamento de las conexiones y el codigo del modulo (No se poseen imagenes)

**UART:** Se uso el modulo correspondiente a UART con datos harcodeados para poder testear la funcionalidad (No se poseen imagenes)

**Amplificador Operacional:** Se uso el codigo del proyecto para disparar la señal por defecto (Senoidal), y se testeo la correcta funcion del LM358 mirando la señal antes y despues de ser amplificada, en el osciloscopio. (Imagen en docs)
