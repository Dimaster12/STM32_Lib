#ifndef __bt_HC_dim
#define __bt_HC_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_general.h"
#include "BD_usart.h"
#include "BD_delay.h"		 

#define AT_GOOD (bool)1
#define AT_ERR (bool)0	 

bool HC_bluetooth_send_command ( UART_HandleTypeDef *husart_x, USARTx_Params_T *p, uint8_t* sending_command, uint8_t array_lenth );
bool HC_bluetooth_read_message ( USARTx_Params_T *p );
bool HC_bluetooth_init ( UART_HandleTypeDef *husart_x, USARTx_Params_T *p );

#ifdef __cplusplus
}
#endif
#endif
