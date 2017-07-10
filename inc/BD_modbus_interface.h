#ifndef __modbus_interface_H_dim
#define __modbus_interface_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"	 
#include "BD_modbus.h"
	 
#ifdef USB_MODBUS  	
	#include "usbd_cdc_if.h"
#endif
 
#if defined (BT_MODBUS) || defined (RS485_MODBUS) 	
	#include "usart.h"

	void BD_Modbus_Rx_Callback(UART_HandleTypeDef *huart);
	void BD_Modbus_Tx_Callback(UART_HandleTypeDef *huart);

 #if defined (RS485_MODBUS)
		#include "gpio.h"
	#endif
#endif

#if defined (MEMORY_USED)

	#if defined (FLASH_USED)
		#include "BD_flash.h"
	#elif defined(EXT_EEPROM_USED)
		#include "BD_ext_eeprom.h"
	#endif

#endif

void BD_Modbus_Rx_Chack_Mode(Modbus_Params_T *p, uint8_t Tx_data_lenth);
void BD_Modbus_Tx_Chack_Mode(Modbus_Params_T *p );
void BD_Modbus_Reset_Receiving(Modbus_Params_T *p);

#ifdef __cplusplus
}
#endif
#endif
