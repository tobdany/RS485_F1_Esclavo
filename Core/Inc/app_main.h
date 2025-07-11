#ifndef INC_APP_MAIN_H_
#define INC_APP_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // Incluye main.h, que a su vez incluye los headers de HAL

typedef enum {
    RS485_MODE_TRANSMIT,
    RS485_MODE_RECEIVE
} rs485_mode_e;

// Prototipos de funciones públicas (accesibles desde otros archivos .c)

/**
 * @brief Envía una cadena de caracteres a través de la UART.
 * Configura el transceptor RS485 en modo transmisión antes de enviar
 * y lo vuelve a modo recepción después.
 * @param huart Puntero a la estructura de manejo de UART (ej. &huart1).
 * @param str Puntero a la cadena de caracteres a enviar.
 */
void UART_SendString(UART_HandleTypeDef *huart, const char *str);

/**
 * @brief Configura el transceptor RS485 en modo transmisión o recepción.
 * Esta función controla el pin DE (Driver Enable) del transceptor RS485.
 * @param mode El modo deseado (RS485_MODE_TRANSMIT o RS485_MODE_RECEIVE).
 */
void _rs485_set_mode(rs485_mode_e mode);

/**
 * @brief Envía un valor de 32 bits a través de la UART para comunicación RS485.
 * Configura el transceptor RS485 en modo transmisión antes de enviar
 * y lo vuelve a modo recepción después.
 * @param Value El valor de 32 bits a enviar.
 */
void _rs485_write32(uint32_t Value);

/**
 * @brief Inicializa el sistema de comunicación RS485.
 * Configura el transceptor en modo recepción e inicia la interrupción de recepción UART.
 */
void _rs485_init(void);

/**
 * @brief Función principal de la aplicación. Contiene el bucle infinito de la lógica del esclavo RS485.
 */
void app_main(void);


#ifdef __cplusplus
}
#endif

#endif /* INC_APP_MAIN_H_ */
