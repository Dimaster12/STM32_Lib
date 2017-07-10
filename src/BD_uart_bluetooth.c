/* Includes ------------------------------------------------------------------*/
#include "BD_uart_bluetooth.h"

const uint8_t		AT_Test[] = {"AT\r\n"};												// Проверка свзяи
const uint8_t		AT_Reset[] = {"AT+RESET\r\n"};								// Пересброс устройства
const uint8_t		AT_ORGL[] = {"AT+ORGL\r\n"};									// Сброс на заводские настройки
const uint8_t		AT_Name[] = {"AT+NAME=STM32_BT\r\n"};					// Задать имя устройства
const uint8_t		AT_Role[] = {"AT+ROLE=0\r\n"};								// 0-Slave; 1-Master; 2-Slave-Loop				
const uint8_t		AT_PSWD[] = {"AT+PSWD=2233\r\n"};							// Пароль				
const uint8_t		AT_UART[] = {"AT+UART=115200,0,0\r\n"};				// Настройка интерфейса
const uint8_t		AT_OK[] = {"OK\r\n"};													// Принято

bool HC_bluetooth_send_command ( UART_HandleTypeDef *husart_x, USARTx_Params_T *p, uint8_t* sending_command, uint8_t array_lenth )
{
	__IO uint8_t *temp_send_adr = p->data_send_8;

	p->Num_packets_send = array_lenth;
	p->Num_packets_get = 4;												// Ждем подтверждения команды

	USART_Logical(p);															// Применяем изменения

	for(uint8_t i = 0; i < p->Num_packets_send_prev; i++)				// Заполняем буфер на отправку нужной командой
	{
		*temp_send_adr = (uint8_t)sending_command[i];
		temp_send_adr++;
	}
	MX_USART_ClearGettedData(p);									// Чистим буфер от старых значений
	
	MX_USART_SendData(p);													// Отправляем команду
	
	Delay_ms(200);																// Даем время BT модулю на ответ
	
	return HC_bluetooth_read_message(p);					// Проверяем ответ
}

bool HC_bluetooth_read_message ( USARTx_Params_T *p )
{	
	if ( memcmp((uint8_t*)AT_OK, (uint8_t*)p->data_get_8, p->Num_packets_get_prev ) != 0 ) return AT_ERR;
	else return AT_GOOD;
}

bool HC_bluetooth_init ( UART_HandleTypeDef *husart_x, USARTx_Params_T *p )
{
	if ( HC_bluetooth_send_command ( husart_x, p, (uint8_t*)AT_Test, (sizeof(AT_Test)-1) ) == AT_ERR ) return AT_ERR;
	if ( HC_bluetooth_send_command ( husart_x, p, (uint8_t*)AT_Name, (sizeof(AT_Name)-1) ) == AT_ERR ) return AT_ERR;
	if ( HC_bluetooth_send_command ( husart_x, p, (uint8_t*)AT_Role, (sizeof(AT_Role)-1) ) == AT_ERR ) return AT_ERR;
	if ( HC_bluetooth_send_command ( husart_x, p, (uint8_t*)AT_PSWD, (sizeof(AT_PSWD)-1) ) == AT_ERR ) return AT_ERR;
	if ( HC_bluetooth_send_command ( husart_x, p, (uint8_t*)AT_UART, (sizeof(AT_UART)-1) ) == AT_ERR ) return AT_ERR;	
	if ( HC_bluetooth_send_command ( husart_x, p, (uint8_t*)AT_Reset, (sizeof(AT_Reset)-1) ) == AT_ERR ) return AT_ERR;	
	return AT_GOOD;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
