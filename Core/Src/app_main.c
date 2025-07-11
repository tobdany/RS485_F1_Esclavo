#include "app_main.h"
#include <stdio.h>
#include <string.h> // Para strlen

extern UART_HandleTypeDef huart1;
uint32_t rxValue_aux, rxValue, txvalue; // txvalue: contador que enviará
uint8_t flagRx = 0, Rs485_Conn = 0;
uint32_t ConnTimeOut;
uint32_t Tick1000;
const uint8_t MY_SLAVE_ID = 1;


void UART_SendString(UART_HandleTypeDef *huart, const char *str) {
    _rs485_set_mode(RS485_MODE_TRANSMIT);
    HAL_UART_Transmit(huart, (uint8_t*)str, strlen(str), 100);
    _rs485_set_mode(RS485_MODE_RECEIVE);
}

//uart callbacks
void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart) {
    ConnTimeOut = HAL_GetTick();
    flagRx = 1;
    HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
    rxValue = rxValue_aux;
    HAL_UART_Receive_IT(huart, (uint8_t*)&rxValue_aux, sizeof(uint32_t));
}

void HAL_UART_txCptlCallback(UART_HandleTypeDef *huart) {
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    HAL_UART_Receive_IT(huart, (uint8_t*)&rxValue_aux, sizeof(uint32_t));
}

/*RS485*/
void _rs485_set_mode(rs485_mode_e mode) {
    switch (mode) {
    case RS485_MODE_TRANSMIT:
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
        break;
    case RS485_MODE_RECEIVE:
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
        break;
    }
}

void _rs485_write32(uint32_t Value) {
    _rs485_set_mode(RS485_MODE_TRANSMIT);
    HAL_UART_Transmit(&huart1, (uint8_t*)&Value, sizeof(uint32_t), 20);
    _rs485_set_mode(RS485_MODE_RECEIVE);
}

void _rs485_init() {
    txvalue = 0; // Inicialización del contador
    _rs485_set_mode(RS485_MODE_RECEIVE);
    HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxValue_aux, sizeof(uint32_t));
}

void app_main(void) {
    HAL_Delay(200);
    _rs485_init();
    UART_SendString(&huart1, "ESCLAVO: Iniciando (ID ");
    char my_id_str[10];
    sprintf(my_id_str, "%u)...\r\n", MY_SLAVE_ID);
    UART_SendString(&huart1, my_id_str);


    Tick1000 = HAL_GetTick();
    while (1) {
        // Solo responde cuando se le solicita.

        // ESCLAVO: Procesar solicitud del Maestro
        if (flagRx == 1) {
            flagRx = 0;
            // HAL_Delay(50);
            HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET); // Apaga LED de RX

            char rx_str[50];
            sprintf(rx_str, "ESCLAVO: Solicitud recibida: %lu\r\n", rxValue);
            UART_SendString(&huart1, rx_str);

            if (Rs485_Conn == 0) {
                Rs485_Conn = 1;
            }

            if (rxValue == MY_SLAVE_ID) {
                txvalue++;
                _rs485_write32(txvalue);
                UART_SendString(&huart1, "ESCLAVO: Respondiendo con mi contador.\r\n");
            } else {
                 UART_SendString(&huart1, "ESCLAVO: Solicitud no para mi.\r\n");
            }
        }

        // Detectar si la comunicación cayó (timeout de recepción)
        if (Rs485_Conn == 1 && (HAL_GetTick() - ConnTimeOut) > 1500) {
            Rs485_Conn = 0;
            UART_SendString(&huart1, "ESCLAVO: Comunicacion con Maestro caida.\r\n");
        }
    }
}
