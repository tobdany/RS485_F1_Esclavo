#include "app_main.h"
#include <stdio.h> // Necesario para sprintf si lo usas
#include <string.h> // Necesario para strlen

extern UART_HandleTypeDef huart1;
uint32_t rxValue_aux,rxValue,txvalue;
uint8_t flagRx=0, Rs485_Conn=0; //Rs_485_Conn se pone en 1 al recibir datos;
uint32_t ConnTimeOut;
uint32_t Tick1000;

typedef enum{
	RS485_MODE_TRANSMIT,
	RS485_MODE_RECEIVE
}rs485_mode_e;


//uart callbacks
//se activa cuando ocurre la interrupción
void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart){
	ConnTimeOut=HAL_GetTick(); //lo usa para saber cuando fue la última recepción
	flagRx=1; //indica que se recibió el dato
	HAL_GPIO_WritePin(LED_RX_GPIO_Port,LED_RX_Pin,GPIO_PIN_SET); //indica que se está recibiendo datos
	rxValue=rxValue_aux;
	HAL_UART_Receive_IT(huart,(uint8_t*)&rxValue_aux,sizeof(uint32_t)); //vuelve a habilitar la recepción por interrupión

}

void HAL_UART_txCptlCallback(UART_HandleTypeDef *huart){

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(huart,(uint8_t*)&rxValue_aux,sizeof(uint32_t));
	//en caso de error vuelve a habilitar la recepción
}

/*RS485*/
void _rs485_set_mode(rs485_mode_e mode){
	//funcion que establece el modo de transmisión
	switch(mode){
	case RS485_MODE_TRANSMIT:
		HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin,GPIO_PIN_SET);
		break;
	case RS485_MODE_RECEIVE:
		HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin,GPIO_PIN_RESET);
		break;
	}

}

void _rs485_write32(uint32_t Value){
	_rs485_set_mode(RS485_MODE_TRANSMIT);
	// Considera un pequeño delay aquí si tienes problemas de latencia al cambiar DE
	// HAL_Delay(1); // Descomentar si es necesario, especialmente para transceivers lentos
	HAL_UART_Transmit(&huart1,(uint8_t*)&Value,sizeof(uint32_t),20);
	_rs485_set_mode(RS485_MODE_RECEIVE); // Siempre vuelve a recibir después de una transmisión
}

// --- FUNCIÓN _rs485_init() CORREGIDA ---
void _rs485_init(){
	txvalue=0;
	// Cambiamos el modo inicial a TRANSMITIR
	_rs485_set_mode(RS485_MODE_TRANSMIT); // <--- CAMBIO AQUÍ
    // Opcional: Podrías enviar el primer dato aquí si quieres transmitir inmediatamente
    // _rs485_write32(txvalue); // Esto haría la primera transmisión al iniciar
    // Luego volver al modo RECEIVE si la comunicación es semidúplex y no estás transmitiendo constantemente
    // _rs485_set_mode(RS485_MODE_RECEIVE); // Si solo quieres iniciar en TX y luego operar normalmente

    // Si quieres iniciar en TX para enviar un primer mensaje y luego pasar a RX para recibir normalmente
    // después de la inicialización completa:
    HAL_UART_Receive_IT(&huart1,(uint8_t*)&rxValue_aux,sizeof(uint32_t)); // Habilitar la recepción por interrupción

}
// --- FIN DE LA FUNCIÓN _rs485_init() CORREGIDA ---

void app_main(void)
{

	HAL_Delay(200);
	_rs485_init(); // Ahora _rs485_init() pondrá el transceptor en modo TRANSMIT


	Tick1000=HAL_GetTick();
	while(1){
		if((HAL_GetTick()-Tick1000)>1000){
			Tick1000=HAL_GetTick();
			txvalue++;
			_rs485_write32(txvalue); // Esta función ya gestiona el cambio a TX y luego a RX
		}
		if(Rs485_Conn==1 && (HAL_GetTick()-ConnTimeOut)>1500){
			//para detectar si la comunicación cayó
			//los datos se envian cada segundo
			//si desde la ultima recepción ha pasado más de un segundo
			//quiere decir que la comunicación cayó
			Rs485_Conn=0;
		}

		if(flagRx==1){
			flagRx=0;
			HAL_Delay(50);

			HAL_GPIO_WritePin(LED_RX_GPIO_Port,LED_RX_Pin,GPIO_PIN_RESET);
			if(Rs485_Conn==0){
				Rs485_Conn=1;
			}
		}
	}
}
