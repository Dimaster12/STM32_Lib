/* Includes ------------------------------------------------------------------*/
#include "BD_modbus_interface.h"

void BD_Modbus_Rx_Chack_Mode(Modbus_Params_T *p, uint8_t Tx_data_lenth)
{
#ifdef USB_MODBUS
	if (p->Modbus_Mode == USB_MODBUS_MODE)
	{
		CDC_Transmit_FS(USB_Modbus.ModbusTxBuffer, Tx_data_lenth);
	}
#endif
	
#ifdef BT_MODBUS
	if (p->Modbus_Mode == BT_MODBUS_MODE)
	{	
		BD_USART_Modbus_Send_data(&USART_BT_Params, Tx_data_lenth);
	}
#endif

#ifdef RS485_MODBUS
	if (p->Modbus_Mode == RS485_MODBUS_MODE)
	{
		Tx_allowed();
		BD_USART_Modbus_Send_data(&USART_RS485_Params, Tx_data_lenth);
	}
#endif
}

void BD_Modbus_Tx_Chack_Mode(Modbus_Params_T *p)
{
#ifdef RS485_MODBUS	
	if (p->Modbus_Mode == RS485_MODBUS_MODE) Rx_allowed();
#endif	
}

#if defined(BT_MODBUS) || defined(RS485_MODBUS)

	void BD_Modbus_Rx_Callback(UART_HandleTypeDef *huart)
	{
	#ifdef BT_MODBUS
		if (huart == USART_BT_Params.husart_x)
		{	
			MX_USART_GetData(&USART_BT_Params);		
			BD_Menu_Modbus (&BT_Modbus);
		}
	#endif
		
	#ifdef RS485_MODBUS	
		if (huart == USART_RS485_Params.husart_x)
		{	
			MX_USART_GetData(&USART_RS485_Params);
			BD_Menu_Modbus(&RS485_Modbus);
		}
	#endif
	}

#ifdef RS485_MODBUS
	void BD_Modbus_Tx_Callback(UART_HandleTypeDef *huart)
	{
		if (huart == USART_RS485_Params.husart_x) Rx_allowed();
	}
#endif

#endif	

void BD_Modbus_Reset_Receiving(Modbus_Params_T *p)
{
#ifdef BT_MODBUS
	if (p->Modbus_Mode == BT_MODBUS_MODE)
	{
		BD_USART_Reset_Get_Interrupt (&USART_BT_Params, USART_BT_Params.reset_Callback);
		BD_MODBUS_ClearGettedData(&USART_BT_Params, USART_BT_Params.Num_packets_get_prev);
	}
#endif

#ifdef RS485_MODBUS
	if (p->Modbus_Mode == RS485_MODBUS_MODE)
	{
		BD_USART_Reset_Get_Interrupt (&USART_RS485_Params, USART_RS485_Params.reset_Callback);
		BD_MODBUS_ClearGettedData(p, USART_RS485_Params.Num_packets_get_prev);
	}
#endif
	
#ifdef USB_MODBUS
	if (p->Modbus_Mode == USB_MODBUS_MODE)
	{
		BD_Menu_Modbus(&USB_Modbus);
		BD_MODBUS_ClearGettedData(p, USART_RS485_Params.Num_packets_get_prev);
	}
#endif	
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
