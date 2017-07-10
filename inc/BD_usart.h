#ifndef __usart_H_dim
#define __usart_H_dim
#ifdef __cplusplus
 extern "C" {
#endif
 
#include "BD_main_USER.h"	 
#include "BD_general.h"
	 
typedef struct 
{
	// Function inited (must be inited by special function ones):
	uint32_t									USARTx_BaudRate;	
	__IO uint8_t							*during_get_adr;											// Указатель на принимаемый текущий элемент массива		

	bool											USARTx_init_was;
	uint32_t									USARTx_function_break;
	
	uint32_t									Rx_Calc;
	uint32_t									Tx_Calc;

	uint32_t									Rx_Calc_prev;
	uint32_t									Tx_Calc_prev;
	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	bool											USARTx_init;													// Allowes USARTx to be inited
	UART_HandleTypeDef*				husart_x;															// Structer of USARTx
	void											(*USARTx_Init_func)(void);						// USART init function
 
	__IO uint8_t							*data_send_8;													// Указатель на буфер отправки
	__IO uint8_t							*data_get_8;													// Указатель на буфер приема
	
	bool											One_Wire_Mode;	
	// Changing params (must be inited, may be reinited by developer):
	uint8_t										Num_packets_send;
	uint8_t										Num_packets_get;

	bool											reset_Callback;	
	// Logic inited params (mustn`t be inited, reinited):
	uint32_t									USARTx_BaudRate_prev;
	
	uint8_t										Num_packets_send_prev;
	uint8_t										Num_packets_get_prev;

//	__IO uint8_t							*temp_send_adr;
//	__IO uint8_t							*temp_get_adr;
} USARTx_Params_T;

void USART_Logical (USARTx_Params_T *p);
void USART_Logical_Init_Params (USARTx_Params_T *p);
void MX_USART_ReInit(USARTx_Params_T *p);
void MX_USART_GetData(USARTx_Params_T *p);
void MX_USART_SendData(USARTx_Params_T *p);
void MX_USART_ClearGettedData(USARTx_Params_T *p);
void BD_USART_Modbus_Send_data(USARTx_Params_T *p, uint8_t array_lenth);
void BD_USART_Reset_Get_Interrupt (USARTx_Params_T *p, bool Callback_call);
void BD_Zero_Func (void);

#ifdef __cplusplus
}
#endif
#endif
