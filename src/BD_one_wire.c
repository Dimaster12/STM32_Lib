#include "usart.h"
void (*OW_Work_Tx_func)(OW_Params_T *p);
void (*OW_Work_Rx_func)(OW_Params_T *p);

/******************************************************************************/
void BD_OW_Tx(OW_Params_T *p)
{
	switch(p->operation_func)
	{
		case 0:
			BD_OW_Detect_Presence_Tx(p);			
			break;
		case 1:
			BD_OWI_Command(p, OWI_ROM_SKIP);		
			break;
		case 2:
			BD_OWI_Command(p, DS18B20_CONVERT_T);		
			break;
		case 3:
			BD_OW_WAIT_TEMP_GENERATION_Tx (p);
			break;
		case 4:
			BD_OW_Detect_Presence_Tx(p);
		#if MAX_DEVICES > 1
			p->tmp_adr = p->Sensor[p->num]->id;
		#endif
			break;
	#if MAX_DEVICES > 1
		case 5:
			BD_OWI_Command(p, OWI_ROM_MATCH);
			break;
		case 6:
			BD_OW_MatchRom_Tx(p);
			break;
	#else		
		case 5:
			BD_OWI_Command(p, OWI_ROM_SKIP);		
			break;
	#endif		
		case 7:
			BD_OWI_Command(p, DS18B20_READ_SCRATCHPAD);		
			break;
		case 8:
			BD_OWI_Receive_Data_Tx(p);		
			break;
		case 9:
			BD_OW_TEMP_CALC(p);		
			break;
	}
}

void BD_OW_Rx(OW_Params_T *p)
{
	switch(p->operation_func)
	{
		case 0:
			BD_OW_Detect_Presence_Rx(p);
			break;
		case 1:
			p->operation_func++;
			break;
		case 2:
			p->operation_func++;
			break;
		case 3:
			BD_OW_WAIT_TEMP_GENERATION_Rx(p);
			break;
		case 4:
			BD_OW_Detect_Presence_Rx(p);
			break;
	#if MAX_DEVICES > 1
		case 5:
			p->operation_func++;
			break;
		case 6:
			BD_OW_MatchRom_Rx(p);
			break;
	#else		
		case 5:
			p->operation_func+=2;
			break;
	#endif
		case 7:
			p->operation_func++;
			break;		
		case 8:
			BD_OWI_Receive_Data_Rx(p);
			break;
	}
}

/******************************************************************************/
// OW Init Presence:
void BD_OW_Detect_Presence_Tx(OW_Params_T *p)
{
	p->USARTx->Num_packets_send = 1;
	p->USARTx->Num_packets_get = 1;
	p->USARTx->USARTx_BaudRate = 9600;
	USART_Logical(p->USARTx);

	p->USARTx->data_send_8[0] = 0xF0;
	MX_USART_ClearGettedData(p->USARTx);
	MX_USART_SendData(p->USARTx);
}

void BD_OW_Detect_Presence_Rx(OW_Params_T *p)
{
	if (p->USARTx->data_get_8[0] == 0xE0)
	{
		p->USARTx->Rx_Calc_prev = p->USARTx->Rx_Calc;
		p->Presence_success_calc++;
		p->OW_Status = OW_DEVICES_PRESENSES;
		p->operation_func++;
	}
	else
	{
		p->Presence_mistake_calc++;
		p->OW_Status = OW_DEVICES_NOT_PRESENSES;	
	}
}
// :OW Init Presence
/******************************************************************************/

void BD_OWI_Command (OW_Params_T *p, uint8_t data)
{
	uint8_t temp;
	uint8_t i;
	
	p->data_send_8 = data;
	
	p->USARTx->Num_packets_send = 8;
	p->USARTx->Num_packets_get = 8;
	p->USARTx->USARTx_BaudRate = 115200;
	USART_Logical(p->USARTx);
	MX_USART_ClearGettedData(p->USARTx);
	
	for (i = 0; i < 8; i++)
	{		
		temp = p->data_send_8 & 0x01;
		if (temp) p->USARTx->data_send_8[i] = 0xFF;
		else p->USARTx->data_send_8[i] = 0x00;
		p->data_send_8 >>= 1;
	}
	MX_USART_SendData(p->USARTx);
}

/******************************************************************************/
// OWI Receive Data Function:
void BD_OWI_Receive_Data_Tx(OW_Params_T *p)
{  
	p->USARTx->Num_packets_send = 8;
	p->USARTx->Num_packets_get = 8;
	p->USARTx->USARTx_BaudRate = 115200;
	USART_Logical(p->USARTx);

	for (uint8_t i = 0; i < p->USARTx->Num_packets_send; i++)
	{
		p->USARTx->data_send_8[i] = 0xFF;
	}
	
	MX_USART_ClearGettedData(p->USARTx);
	MX_USART_SendData(p->USARTx);
}

void BD_OWI_Receive_Data_Rx(OW_Params_T *p)
{
	uint8_t temp = 0x1;
	p->data_get_8 = 0;

	for (uint8_t i = 0; i < p->USARTx->Num_packets_send; i++)
	{
		if (p->USARTx->data_get_8[i] == 0xFF) p->data_get_8 += temp;
		temp <<= 1;
	}
	
	p->readen_array[p->during_readen_byte] = p->data_get_8;
	if (p->during_readen_byte == 8)
	{
		p->operation_func++;
		p->during_readen_byte = 0;
	}
	else p->during_readen_byte++;
	
	p->Sensor[p->num]->Status = OW_OK;
}
// :OWI Receive Data Function
/******************************************************************************/

/******************************************************************************/
void BD_OW_Params (OW_Params_T *p)
{
	p->OW_Status = OW_ROM_SEARCHING;
	#if MAX_DEVICES > 1
	BD_OW_Searching_Params(p);
	#endif
	p->operation_func = 0;
	p->during_readen_byte = 0;
	p->during_written_byte = 0;	

	for (uint8_t i = 0; i < 9; i++) p->readen_array[i] = 0x00;
	
	p->data_send_8 = 0;
	p->data_get_8 = 0;	
	p->num = 0;
	
	p->Presence_mistake_calc = 0UL;
	p->Presence_success_calc = 0UL;
	
	#if MAX_DEVICES > 1
		OW_Work_Tx_func = BD_OW_Init_Tx;
		OW_Work_Rx_func = BD_OW_Init_Rx;	
	#else 
		OW_Work_Tx_func = BD_OW_Tx;
		OW_Work_Rx_func = BD_OW_Rx;			
	#endif
	for (uint8_t i = 0; i < MAX_DEVICES; i++) BD_OW_DeviceX_Params(p->Sensor[i]);
}

void BD_OW_DeviceX_Params (OW_DeviceX_Params_T *p)
{
	for (uint8_t i = 0; i < 8; i++) p->id[i] = 0x00;

	p->Status = OW_NO_DEVICE;

	p->int_tempr = 0;
	p->fract_tempr = 0;
	p->tempr = 0.0F;

	p->CRC_res = 0;
	p->CRC_mistake_timer = 0;
}
/******************************************************************************/

/******************************************************************************/
// OW WAIT TEMPRATURE GENERATION:
void BD_OW_WAIT_TEMP_GENERATION_Tx (OW_Params_T *p)
{
	p->USARTx->Num_packets_send = 1;
	p->USARTx->Num_packets_get = 1;
	p->USARTx->USARTx_BaudRate = 115200;
	USART_Logical(p->USARTx);

	p->USARTx->data_send_8[0] = 0xFF;
	MX_USART_ClearGettedData(p->USARTx);
	MX_USART_SendData(p->USARTx);
}

void BD_OW_WAIT_TEMP_GENERATION_Rx (OW_Params_T *p)
{
	static uint16_t test_timer = 0;
	
	test_timer++;
	
	if (p->USARTx->data_get_8[0] == 0xFF)
	{
		p->USARTx->Rx_Calc_prev = p->USARTx->Rx_Calc;
		p->OW_Status = OW_TEMP_CALCULATED;
		p->operation_func++;
	}
	else 
	{
		p->OW_Status = OW_WAIT_TEMP;
	}
}
// :OW WAIT TEMPRATURE GENERATION
/******************************************************************************/

/******************************************************************************/
void BD_OW_TEMP_CALC (OW_Params_T *p)
{
	uint16_t tmp = 0;
	float sign = 1.0F;
	
	OWI_Check_CRC(p, p->readen_array, 8);
	if ( p->Sensor[p->num]->Status == OW_CRC_OK )
	{
		if ((p->readen_array[1]&128) != 0)
		{
			tmp = ((unsigned int)p->readen_array[1]<<8)|p->readen_array[0];
			tmp = ~tmp + 1;
			p->readen_array[0] = tmp;
			p->readen_array[1] = tmp>>8;
			sign = -1.0F;
		}
		else sign = 1.0F;

		p->Sensor[p->num]->int_tempr = (p->readen_array[0]>>4)|((p->readen_array[1]&7)<<4);
		
		p->Sensor[p->num]->fract_tempr = (p->readen_array[0]&15);
		p->Sensor[p->num]->fract_tempr = (p->Sensor[p->num]->fract_tempr<<1) + (p->Sensor[p->num]->fract_tempr<<3);
		p->Sensor[p->num]->fract_tempr = (p->Sensor[p->num]->fract_tempr>>4);
		
		p->Sensor[p->num]->tempr = (float)p->Sensor[p->num]->int_tempr;
		p->Sensor[p->num]->tempr += (float)p->Sensor[p->num]->fract_tempr/10.0F;
		p->Sensor[p->num]->tempr *= sign;
	}
	else if (p->Sensor[p->num]->Status == OW_CRC_ERROR)
	{
		p->Sensor[p->num]->CRC_mistake_timer++;
	}
	
	if ( p->num+1 < MAX_DEVICES )
	{
		p->num++;
		p->operation_func = 4;
	}
	else
	{
		p->num = 0;
		p->operation_func = 0;
	}
}
/******************************************************************************/
void OWI_Check_CRC( OW_Params_T *p, uint8_t *chacking_array, uint8_t lenth)
{
	p->Sensor[p->num]->CRC_res = 0;	
	p->tmp_adr = chacking_array;
	
	for (uint8_t i = 0; i < lenth; i++)
	{
		p->Sensor[p->num]->CRC_res = OWI_ComputeCRC8(*p->tmp_adr, p->Sensor[p->num]->CRC_res);
		p->tmp_adr++;
	}

	if (p->Sensor[p->num]->CRC_res == *p->tmp_adr) p->Sensor[p->num]->Status = OW_CRC_OK;
	else p->Sensor[p->num]->Status = OW_CRC_ERROR; 
}

uint8_t OWI_ComputeCRC8(uint8_t inData, uint8_t seed)
{
	uint8_t bitsLeft;
	uint8_t temp;

	for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
	{
		temp = ((seed ^ inData) & 0x01);
		if (temp == 0)
		{
			seed >>= 1;
		}
		else
		{
			seed ^= 0x18;
			seed >>= 1;
			seed |= 0x80;
		}
		inData >>= 1;
	}
	return seed;    
}

#if MAX_DEVICES > 1
/******************************************************************************/
// OW Params Init Func:
void BD_OW_Searching_Params (OW_Params_T *p)
{
	p->Search_var.numFoundDevices = 0;
	p->Search_var.newID = p->Sensor[0]->id;
	p->Search_var.lastDeviation = 0;
	p->Search_var.currentID = p->Search_var.newID;
}

void BD_OW_New_Sensor_Zero (OW_Searching_Params_T *p)
{
	p->currentBit = 1;
	p->newDeviation = 0;
	p->bitMask = 0x01;
}
// OW Params Init Func:
/******************************************************************************/

/******************************************************************************/
// OWI Read A,B Bits:
void BD_OWI_Read_Two_Bits_Tx(OW_Params_T *p)
{  
	p->USARTx->Num_packets_send = 2;
	p->USARTx->Num_packets_get = 2;
	p->USARTx->USARTx_BaudRate = 115200;
	USART_Logical(p->USARTx);

	for (uint8_t i = 0; i < p->USARTx->Num_packets_send; i++)
	{
		p->USARTx->data_send_8[i] = 0xFF;
	}
	
	MX_USART_ClearGettedData(p->USARTx);
	MX_USART_SendData(p->USARTx);
}

void BD_OWI_Read_Two_Bits_Rx(OW_Params_T *p)
{
	p->data_get_8 = 0;
	
	for (uint8_t i = 0; i < p->USARTx->Num_packets_send; i++)
	{
		if (p->USARTx->data_get_8[i] == 0xFF) p->readen_array[i] = 1;
		else p->readen_array[i] = 0;
	}
	p->OW_Status = OW_ROM_SEARCHING;
	p->operation_func++;
}

// :OWI Read A,B Bits
/******************************************************************************/

void BD_Write_Bit (OW_Params_T *p, bool bit)
{
	p->USARTx->Num_packets_send = 1;
	p->USARTx->Num_packets_get = 1;
	p->USARTx->USARTx_BaudRate = 115200;
	USART_Logical(p->USARTx);
	MX_USART_ClearGettedData(p->USARTx);
	
	if (bit) p->USARTx->data_send_8[0] = 0xFF;
	else p->USARTx->data_send_8[0] = 0x00;

	MX_USART_SendData(p->USARTx);
}

/******************************************************************************/
// Match ROM:
void BD_OW_MatchRom_Tx(OW_Params_T *p)
{
	uint8_t temp;

	p->USARTx->Num_packets_send = 8;
	p->USARTx->Num_packets_get = 8;
	p->USARTx->USARTx_BaudRate = 115200;
	USART_Logical(p->USARTx);
	MX_USART_ClearGettedData(p->USARTx);

	p->data_send_8 = *p->tmp_adr;
	for (uint8_t i = 0; i < 8; i++)
	{		
		temp = p->data_send_8 & 0x01;
		if (temp) p->USARTx->data_send_8[i] = 0xFF;
		else p->USARTx->data_send_8[i] = 0x00;
		p->data_send_8 >>= 1;
	}
	MX_USART_SendData(p->USARTx);
}

void BD_OW_MatchRom_Rx(OW_Params_T *p)
{
	if (p->during_written_byte < 7)
	{
		p->during_written_byte++;
		p->tmp_adr++;
	}
	else 
	{
		p->during_written_byte = 0;
		p->operation_func++;
	}
}
// :Match ROM
/******************************************************************************/

/******************************************************************************/
void BD_OW_Init_Tx(OW_Params_T *p)
{
	switch(p->operation_func)
	{
		case 0:
			memcpy(p->Search_var.newID, p->Search_var.currentID, 8);
			BD_OW_Detect_Presence_Tx(p);
			break;
		case 1:
			BD_OW_New_Sensor_Zero(&p->Search_var);
			p->Search_var.newID = p->Sensor[p->num]->id;
			BD_OWI_Command(p, OWI_ROM_SEARCH);
			break;
		case 2:
			BD_OWI_Read_Two_Bits_Tx(p);
			break;
		case 3:
			BD_OWI_SearchRom_Tx(p);
			break;
		case 4:
			BD_ID_Chack(p);
			break;
	}
}

void BD_OW_Init_Rx(OW_Params_T *p)
{
	switch(p->operation_func)
	{
		case 0:
			BD_OW_Detect_Presence_Rx(p);
			break;
		case 1:
			p->operation_func++;
			break;
		case 2:
			BD_OWI_Read_Two_Bits_Rx(p);
			break;
		case 3:
			BD_OWI_SearchRom_Rx(p);		
			break;
	}
}
/******************************************************************************/

/******************************************************************************/
// SearchRom:
void BD_OWI_SearchRom_Tx(OW_Params_T *p)
{
	if (p->readen_array[0] && p->readen_array[1])
	{
		p->OW_Status = OW_ROM_SEARCH_FAILED;
	}
	else if (p->readen_array[0] ^ p->readen_array[1])
	{
		if (p->readen_array[0])
		{
			(*p->Search_var.newID) |= p->Search_var.bitMask;
		}
		else
		{
			(*p->Search_var.newID) &= ~p->Search_var.bitMask;
		}
	}
	else
	{
		if (p->Search_var.currentBit == p->Search_var.lastDeviation)
		{
			(*p->Search_var.newID) |= p->Search_var.bitMask;
		}
		else if (p->Search_var.currentBit > p->Search_var.lastDeviation)
		{
			(*p->Search_var.newID) &= ~p->Search_var.bitMask;
			p->Search_var.newDeviation = p->Search_var.currentBit;
		}
		else if ( !(*p->Search_var.newID & p->Search_var.bitMask)) p->Search_var.newDeviation = p->Search_var.currentBit; 
	}
	
	if ( (*p->Search_var.newID) & p->Search_var.bitMask ) BD_Write_Bit(p, 1);
	else BD_Write_Bit(p, 0);
}

void BD_OWI_SearchRom_Rx(OW_Params_T *p)
{
	p->Search_var.currentBit++;
	p->Search_var.bitMask <<= 1;
	if (!p->Search_var.bitMask)
	{
		p->Search_var.bitMask = 0x01;
		p->Search_var.newID++;
	}
	
	if (p->Search_var.currentBit <= 64) p->operation_func--;
	else
	{
		p->Search_var.lastDeviation = p->Search_var.newDeviation;	
		p->Search_var.currentID = p->Search_var.newID;
		p->Search_var.numFoundDevices++;
		p->operation_func++;
	}
}
// :SearchRom
/******************************************************************************/

void BD_ID_Chack(OW_Params_T *p)
{
	OWI_Check_CRC(p, p->Sensor[p->num]->id, 7);
	if (p->Sensor[p->num]->Status == OW_CRC_OK)
	{
		if (p->Search_var.lastDeviation == OW_ROM_SEARCH_FINISHED)
		{
			OW_Work_Tx_func = BD_OW_Tx;
			OW_Work_Rx_func = BD_OW_Rx;
			p->num = 0;			
			BD_OW_Zero_Params(p);
			
			if (p->Search_var.numFoundDevices < MAX_DEVICES)
			{
				p->OW_Status = OW_NOT_ALL_SENSORS_FOUND;
			}
			else p->OW_Status = OW_ALL_SENSORS_FOUND;
			
		}
		else
		{
			p->operation_func = 0;
			p->num++;
		}		
	}
	else if (p->Sensor[p->num]->Status == OW_CRC_ERROR)
	{		
		OW_Work_Tx_func = BD_OW_Zero_Func;	
		OW_Work_Rx_func =	BD_OW_Zero_Func;
		
		p->OW_Status = OW_ROM_SEARCH_CRC_ERROR;		
	}
}

void BD_OW_Zero_Params (OW_Params_T *p)
{
	p->operation_func = 0;
	p->during_readen_byte = 0;
	p->during_written_byte = 0;

	for (uint8_t i = 0; i < 9; i++) p->readen_array[i] = 0x00;
	
	p->data_send_8 = 0;
	p->data_get_8 = 0;	
	p->num = 0;
}

void BD_OW_Zero_Func (OW_Params_T *p)
{
	return;
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
