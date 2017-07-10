/* Includes ------------------------------------------------------------------*/
#include "BD_modbus_interface.h"
#include "BD_g_Ram_USER.h"

Rx_Status_T BD_Modbus_Data_Rx ( Modbus_Params_T *p )
{
	// ID	
	p->Rx_Params.ID = p->ModbusRxBuffer[0];
	if (p->Rx_Params.ID != p->ID_Device)
	{
		p->ID_Mistake = true;
		p->Status = Data_get_ERR;
	}			
	else
	{
		p->ID_Mistake = false;
		p->Status = Receiving;
	}
	// Command	
	p->Rx_Params.Command = p->ModbusRxBuffer[1];
	if ( p->Rx_Params.Command != CMD_READ_DATA && p->Rx_Params.Command != CMD_WRITE_DATA )
	{
		p->Unknown_Command = true;
		p->Status = Data_get_ERR;
	}
	else
	{
		p->Unknown_Command = false;
		p->Status = Receiving;
	}
	// Data address	
	p->Rx_Params.Data_addr = p->ModbusRxBuffer[2]<< 8;
	p->Rx_Params.Data_addr += p->ModbusRxBuffer[3];

	// Number_of_registers
	p->Rx_Params.Number_of_registers = p->ModbusRxBuffer[4]<< 8;
	p->Rx_Params.Number_of_registers += p->ModbusRxBuffer[5];

	if ( !p->Unknown_Command )
	{
		if ( p->Rx_Params.Command == CMD_READ_DATA )
		{
			// CRC		
			p->Rx_Params.CRC_Res = p->ModbusRxBuffer[6] << 8;
			p->Rx_Params.CRC_Res += p->ModbusRxBuffer[7];
			
			if ( CRC16(p->ModbusRxBuffer, 6) == p->Rx_Params.CRC_Res )
			{					
				p->CRC_Mistake = false;
				if ( !p->ID_Mistake && !p->Unknown_Command ) p->Status = Data_get_OK;
				else p->Status = Data_get_ERR;			
			}
			else
			{
				p->CRC_Mistake = true;
				p->Status = Data_get_ERR;			
			}
		}
		else if ( p->Rx_Params.Command == CMD_WRITE_DATA )
		{
			// Bytes
			p->i = 6;
			p->Rx_Params.Bytes = p->ModbusRxBuffer[p->i];

			// Writing data
			if (p->Rx_Params.Number_of_registers <= MAX_RX_DATA_SIZE)
			{
				for ( p->k = 0; p->k < p->Rx_Params.Number_of_registers; p->k++ )
				{
					p->Rx_Params.Writing_Data[p->k] = p->ModbusRxBuffer[++p->i]<< 8;
					p->Rx_Params.Writing_Data[p->k] += p->ModbusRxBuffer[++p->i];
				}				
			}
			else
			{
				for ( p->k = 0; p->k < MAX_RX_DATA_SIZE; p->k++ )
				{
					p->Rx_Params.Writing_Data[p->k] = p->ModbusRxBuffer[++p->i]<< 8;
					p->Rx_Params.Writing_Data[p->k] += p->ModbusRxBuffer[++p->i];
				}
			}
			// CRC
			p->Rx_Params.CRC_Res = p->ModbusRxBuffer[++p->i] << 8;
			p->Rx_Params.CRC_Res += p->ModbusRxBuffer[++p->i];

			if ( CRC16(p->ModbusRxBuffer, --p->i) == p->Rx_Params.CRC_Res )
			{					
				p->CRC_Mistake = false;
				if ( !p->ID_Mistake && !p->Unknown_Command ) p->Status = Data_get_OK;
				else p->Status = Data_get_ERR;
			}
			else
			{
				p->CRC_Mistake = true;
				p->Status = Data_get_ERR;
			}	
		}
	}
	return p->Status;
}

void BD_Modbus_Data_Tx(Modbus_Params_T *p)
{
	p->i = 0;

	p->ModbusTxBuffer[p->i] = p->ID_Device;																						// ID
	p->ModbusTxBuffer[++p->i] = p->Rx_Params.Command;														// Команда
	
	if (p->Rx_Params.Command == CMD_READ_DATA)
	{
		p->ModbusTxBuffer[++p->i] = p->Rx_Params.Number_of_registers*2;
		// Посылаемые данные
		for ( p->k=0; p->k < p->Rx_Params.Number_of_registers; p->k++)
		{
			p->ModbusTxBuffer[++p->i] = (uint8_t)((p->Tx_Params.Reading_Data[p->k]&0xFF00)>>8);					// Старшая часть
			p->ModbusTxBuffer[++p->i] = (uint8_t)(p->Tx_Params.Reading_Data[p->k]&0xFF);									// Младшая часть
		}
	}
	else if (p->Rx_Params.Command == CMD_WRITE_DATA)
	{
		// Address
		p->ModbusTxBuffer[++p->i] = (uint8_t)((p->Rx_Params.Data_addr&0xFF00)>>8);
		p->ModbusTxBuffer[++p->i] = (uint8_t)(p->Rx_Params.Data_addr&0xFF);
		
		// Number of registers
		p->ModbusTxBuffer[++p->i] = (uint8_t)((p->Rx_Params.Number_of_registers&0xFF00)>>8);
		p->ModbusTxBuffer[++p->i] = (uint8_t)(p->Rx_Params.Number_of_registers&0xFF);
	}
	
	// CRC
	p->Tx_Params.CRC_Res = CRC16(p->ModbusTxBuffer, ++p->i);	
	p->ModbusTxBuffer[p->i] = (uint8_t)((p->Tx_Params.CRC_Res&0xFF00)>>8);				// Старшая часть
	p->ModbusTxBuffer[++p->i] = (uint8_t)(p->Tx_Params.CRC_Res&0xFF);						// Младшая часть
	
	BD_Modbus_Rx_Chack_Mode(p, ++p->i);
}

#if defined (BT_MODBUS) || defined (RS485_MODBUS)
void BD_Modbus_Init(Modbus_Params_T *p, USARTx_Params_T *USART)
#elif defined(USB_MODBUS)
void BD_Modbus_Init(Modbus_Params_T *p)
#endif
{
	
#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	#if defined (F1XX)
	p->Byte_timer = &USART->husart_x->hdmarx->Instance->CNDTR;
	#elif defined(F4XX)	
	p->Byte_timer = &USART->husart_x->hdmarx->Instance->NDTR;
	#endif
	p->Byte_timer_prev = USART->Num_packets_get_prev;
#elif defined(USB_MODBUS)	
	p->Byte_timer_prev = 0;
#endif
	p->Addr = 0;
	p->reset_Packet_timer = 0;
	p->mistake_Timer = 0;
	p->usb_Timer = 0;
	
	// Rx
	p->ID_Mistake = false;
	p->Unknown_Command = false;
	p->CRC_Mistake = false;
	p->Status = Data_waiting;

	for (uint8_t i = 0; i < MAX_RX_DATA_SIZE; i++ )
	{
		p->ModbusRxBuffer[i] = 0;
	}
	
	p->Rx_Params.ID = 0;
	p->Rx_Params.Command = CMD_NOT_SET;
	p->Rx_Params.Data_addr = 0;
	p->Rx_Params.Number_of_registers = 0;
	p->Rx_Params.Bytes = 0;

	for (uint8_t i = 0; i < MAX_RX_DATA_SIZE; i++ )
	{	
		p->Rx_Params.Writing_Data[i] = 0;
	}
	p->Rx_Params.CRC_Res = 0;

	for (uint8_t i = 0; i < MAX_TX_DATA_SIZE; i++ )
	{	
		p->ModbusTxBuffer[i] = 0;
	}
	p->Tx_Params.ID = 0;
	p->Tx_Params.Command = CMD_NOT_SET;
	
	for (uint8_t i = 0; i < MAX_TX_DATA_SIZE; i++ )
	{	
		p->Tx_Params.Reading_Data[i] = 0;
	}
	p->Tx_Params.CRC_Res = 0;	
	
	//Graphics{
	p->Graphic.Graphic_num = 0;
	for (uint16_t i = 0; i < MAX_GRAPH_BUFFER; i++ )
	{
		p->Graphic.Gr_Buffer[0][i] = 0;
		p->Graphic.Gr_Buffer[1][i] = 0;
	}
	p->Graphic.Monitor_flag = 0;
	p->Graphic.Point_offset = 0;
	p->Graphic.Buffer_addr = 0;
	p->Graphic.During_Addr = 0;
	//Graphics}

#if defined (MEMORY_USED)
	p->Par_searching_flag = &EEPROM_Params.Par_searching_flag;
	p->Var_pointer = &EEPROM_Params.Var_pointer;
#endif
	
	BD_Modbus_Tx_Chack_Mode ( p );
}

void BD_Modbus_Timeout_Reset(Modbus_Params_T *p, TIM_Calc_Params_T *m, TIMx_Params_T *v)
{
	if (*p->Byte_timer != p->Byte_timer_prev)
	{
		p->Byte_timer_prev = *p->Byte_timer;
		BD_TIM_Time_Calc (m, v, TIME_CALC_ACTIVE);
	}
	BD_TIM_Time_Calc (m, v, VIEW_RESULT);
	
	if (m->Result_time_us > p->Reset_Timeout)
	{
		m->Result_time_us = 0;
		p->reset_Packet_timer++;
		m->State = WAITING_INIT_TIME;
		BD_Modbus_Reset_Receiving(p);
	}
}

void BD_Menu_Modbus ( Modbus_Params_T *p )
{	
	if ( BD_Modbus_Data_Rx (p) == Data_get_OK )
	{
		if (BD_Modbus_Var_Search(p) == PARAM_FOUND_OK)
		{
			BD_Modbus_Size_Convert(p, p->Addr);
			BD_Modbus_Data_Tx(p);			
		}
		else if ( p->Graphic.Monitor_flag ) BD_Modbus_Graphics_Send(p);
		#if defined (JOURNAL_USED)
		else BD_Journal_Chack(p, &m->Journal);
		#endif
	}
	else p->mistake_Timer++;
}

void BD_Modbus_Size_Convert ( Modbus_Params_T *p, uint16_t Addr )
{
	Var_Modbus_8_T				tmp_str_8;
	Var_Modbus_U8_T				tmp_str_U8;
	Var_Modbus_16_T				tmp_str_16;
	Var_Modbus_U16_T			tmp_str_U16;	
	Var_Modbus_32_T				tmp_str_32;
	Var_Modbus_U32_T			tmp_str_U32;
	Var_Modbus_FL_T				tmp_str_FL;
	Var_Modbus_Q16_T			tmp_Q16;

	if ( g_Ram_modbus[Addr].Var->Size & SIZE_8 )
	{
		if (g_Ram_modbus[Addr].Var->Size & SIGN )
		{
			tmp_str_8.Value = (int8_t*)g_Ram_modbus[Addr].Var->Value;
			tmp_str_8.Min = (int8_t*)g_Ram_modbus[Addr].Var->Min;
			tmp_str_8.Max = (int8_t*)g_Ram_modbus[Addr].Var->Max;
			tmp_str_8.Mode = g_Ram_modbus[Addr].Mode;
			tmp_str_8.Wr_Data = (int16_t)p->Rx_Params.Writing_Data[0];
			
			if (p->Rx_Params.Command == CMD_WRITE_DATA)
			{
				if ( ( tmp_str_8.Mode & RW_M ) &&
				( tmp_str_8.Wr_Data >= *tmp_str_8.Min && tmp_str_8.Wr_Data <= *tmp_str_8.Max) )
				{
					*tmp_str_8.Value = tmp_str_8.Wr_Data;					
				#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
					if (tmp_str_8.Mode & MEM_FIX)
					{
						*p->Par_searching_flag = true;
						*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
					}
				#endif	
				}
			}
			p->Tx_Params.Reading_Data[0] = (uint16_t)(*tmp_str_8.Value);
		}
		else
		{
			tmp_str_U8.Value = (uint8_t*)g_Ram_modbus[Addr].Var->Value;
			tmp_str_U8.Min = (uint8_t*)g_Ram_modbus[Addr].Var->Min;
			tmp_str_U8.Max = (uint8_t*)g_Ram_modbus[Addr].Var->Max;
			tmp_str_U8.Mode = g_Ram_modbus[Addr].Mode;
			tmp_str_U8.Wr_Data = (uint16_t)p->Rx_Params.Writing_Data[0];
			
			if (p->Rx_Params.Command == CMD_WRITE_DATA)
			{
				if ( ( tmp_str_U8.Mode & RW_M || tmp_str_U8.Mode & WO_M ) &&
				( tmp_str_U8.Wr_Data >= *tmp_str_U8.Min && tmp_str_U8.Wr_Data <= *tmp_str_U8.Max) )
				{
					*tmp_str_U8.Value = tmp_str_U8.Wr_Data;
				#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
					if (tmp_str_U8.Mode & MEM_FIX) 
					{
						*p->Par_searching_flag = true;
						*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
					}
				#endif				
				}
			}			
			p->Tx_Params.Reading_Data[0] = (uint16_t)(*tmp_str_U8.Value);
		}		
	}
	else if ( g_Ram_modbus[Addr].Var->Size & SIZE_16 )
	{
		if (g_Ram_modbus[Addr].Var->Size & SIGN )
		{
			tmp_str_16.Value = (int16_t*)g_Ram_modbus[Addr].Var->Value;
			tmp_str_16.Min = (int16_t*)g_Ram_modbus[Addr].Var->Min;
			tmp_str_16.Max = (int16_t*)g_Ram_modbus[Addr].Var->Max;
			tmp_str_16.Mode = g_Ram_modbus[Addr].Mode;
			tmp_str_16.Wr_Data = (int16_t)p->Rx_Params.Writing_Data[0];			
			
			if (p->Rx_Params.Command == CMD_WRITE_DATA)
			{
				if ( ( tmp_str_16.Mode & RW_M  ) &&
				( tmp_str_16.Wr_Data >= *tmp_str_16.Min && tmp_str_16.Wr_Data <= *tmp_str_16.Max) )
				{
					*tmp_str_16.Value = tmp_str_16.Wr_Data;
				#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
					if (tmp_str_16.Mode & MEM_FIX) 
					{
						*p->Par_searching_flag = true;
						*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
					}
				#endif	
					
				}
			}		
			if ( tmp_str_16.Mode & GR_B_M ) p->Graphic.Monitor_flag = true;
			else p->Graphic.Monitor_flag = false;
			p->Tx_Params.Reading_Data[0] = (uint16_t)(*tmp_str_16.Value);
		}
		else
		{
			tmp_str_U16.Value = (uint16_t*)g_Ram_modbus[Addr].Var->Value;
			tmp_str_U16.Min = (uint16_t*)g_Ram_modbus[Addr].Var->Min;
			tmp_str_U16.Max = (uint16_t*)g_Ram_modbus[Addr].Var->Max;
			tmp_str_U16.Mode = g_Ram_modbus[Addr].Mode;
			tmp_str_U16.Wr_Data = (uint16_t)p->Rx_Params.Writing_Data[0];			
			
			if (p->Rx_Params.Command == CMD_WRITE_DATA)
			{
				if ( ( tmp_str_U16.Mode & RW_M || tmp_str_U16.Mode & WO_M ) &&
				( tmp_str_U16.Wr_Data >= *tmp_str_U16.Min && tmp_str_U16.Wr_Data <= *tmp_str_U16.Max) )
				{
					*tmp_str_U16.Value = tmp_str_U16.Wr_Data;
				#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
					if (tmp_str_U16.Mode & MEM_FIX) 
					{
						*p->Par_searching_flag = true;
						*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
					}
				#endif						
				}
			}		
			if ( tmp_str_U16.Mode & GR_B_M ) p->Graphic.Monitor_flag = true;
			else p->Graphic.Monitor_flag = false;
			p->Tx_Params.Reading_Data[0] = (uint16_t)(*tmp_str_U16.Value);		
		}
	}
	else if ( g_Ram_modbus[Addr].Var->Size & SIZE_32 )
	{
		if (g_Ram_modbus[Addr].Var->Size & SIGN )
		{
			tmp_str_32.Value = (int32_t*)g_Ram_modbus[Addr].Var->Value;
			tmp_str_32.Min = (int32_t*)g_Ram_modbus[Addr].Var->Min;
			tmp_str_32.Max = (int32_t*)g_Ram_modbus[Addr].Var->Max;
			tmp_str_32.Mode = g_Ram_modbus[Addr].Mode;
			tmp_str_32.Wr_Data = (int32_t)p->Rx_Params.Writing_Data[0];
			tmp_str_32.Wr_Data += (int32_t)p->Rx_Params.Writing_Data[1]<<16;
			
			if (p->Rx_Params.Command == CMD_WRITE_DATA)
			{
				if ( ( tmp_str_32.Mode & RW_M ) &&
				( tmp_str_32.Wr_Data >= *tmp_str_32.Min && tmp_str_32.Wr_Data <= *tmp_str_32.Max) )
				{
					*tmp_str_32.Value = tmp_str_32.Wr_Data;
				#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
					if (tmp_str_32.Mode & MEM_FIX) 
					{
						*p->Par_searching_flag = true;
						*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
					}
				#endif					
				}
			}
			p->Tx_Params.Reading_Data[0] = (uint16_t)(*tmp_str_32.Value);
			p->Tx_Params.Reading_Data[1] = (uint16_t)(*tmp_str_32.Value>>16);			
		}
		else
		{
			tmp_str_U32.Value = (uint32_t*)g_Ram_modbus[Addr].Var->Value;
			tmp_str_U32.Min = (uint32_t*)g_Ram_modbus[Addr].Var->Min;
			tmp_str_U32.Max = (uint32_t*)g_Ram_modbus[Addr].Var->Max;
			tmp_str_U32.Mode = g_Ram_modbus[Addr].Mode;
			tmp_str_U32.Wr_Data = (uint32_t)p->Rx_Params.Writing_Data[0];
			tmp_str_U32.Wr_Data += (uint32_t)p->Rx_Params.Writing_Data[1]<<16;
			
			if (p->Rx_Params.Command == CMD_WRITE_DATA)
			{
				if ( ( tmp_str_U32.Mode & RW_M || tmp_str_U32.Mode & WO_M ) &&
				( tmp_str_U32.Wr_Data >= *tmp_str_U32.Min && tmp_str_U32.Wr_Data <= *tmp_str_U32.Max) )
				{
					*tmp_str_U32.Value = tmp_str_U32.Wr_Data;
				#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
					if (tmp_str_U32.Mode & MEM_FIX) 
					{
						*p->Par_searching_flag = true;
						*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
					}
				#endif
				}
			}
			p->Tx_Params.Reading_Data[0] = (uint16_t)(*tmp_str_U32.Value);
			p->Tx_Params.Reading_Data[1] = (uint16_t)(*tmp_str_U32.Value>>16);		
		}
	}
	else if ( g_Ram_modbus[Addr].Var->Size & SIZE_FLOAT )
	{
		tmp_str_FL.Sign_Symbols = g_Ram_modbus[Addr].Var->Size;

		if			(tmp_str_FL.Sign_Symbols & POINT_0) tmp_str_FL.Wr_Data = ((float)((int16_t)p->Rx_Params.Writing_Data[0]));
		else if	(tmp_str_FL.Sign_Symbols & POINT_1) tmp_str_FL.Wr_Data = ((float)((int16_t)p->Rx_Params.Writing_Data[0]))/10.0F;
		else if (tmp_str_FL.Sign_Symbols & POINT_2) tmp_str_FL.Wr_Data = ((float)((int16_t)p->Rx_Params.Writing_Data[0]))/100.0F;
		else if (tmp_str_FL.Sign_Symbols & POINT_3) tmp_str_FL.Wr_Data = ((float)((int16_t)p->Rx_Params.Writing_Data[0]))/1000.0F;
		else if (tmp_str_FL.Sign_Symbols & POINT_4) tmp_str_FL.Wr_Data = ((float)((int16_t)p->Rx_Params.Writing_Data[0]))/10000.0F;
		
		tmp_str_FL.Value = (float*)g_Ram_modbus[Addr].Var->Value;
		tmp_str_FL.Min = (float*)g_Ram_modbus[Addr].Var->Min;
		tmp_str_FL.Max = (float*)g_Ram_modbus[Addr].Var->Max;
		tmp_str_FL.Mode = g_Ram_modbus[Addr].Mode;

		if (p->Rx_Params.Command == CMD_WRITE_DATA)
		{
			if ( ( tmp_str_FL.Mode & RW_M ) &&
			( tmp_str_FL.Wr_Data >= *tmp_str_FL.Min && tmp_str_FL.Wr_Data <= *tmp_str_FL.Max) )
			{
				*tmp_str_FL.Value = tmp_str_FL.Wr_Data;
			#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
				if (tmp_str_FL.Mode & MEM_FIX) 
				{
					*p->Par_searching_flag = true;
					*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
				}
			#endif			
			}
		}

		if			(tmp_str_FL.Sign_Symbols & POINT_0) p->Tx_Params.Reading_Data[0] = (int16_t)(*tmp_str_FL.Value);
		else if	(tmp_str_FL.Sign_Symbols & POINT_1) p->Tx_Params.Reading_Data[0] = (int16_t)(*tmp_str_FL.Value*10.0F);	
		else if (tmp_str_FL.Sign_Symbols & POINT_2) p->Tx_Params.Reading_Data[0] = (int16_t)(*tmp_str_FL.Value*100.0F);	
		else if (tmp_str_FL.Sign_Symbols & POINT_3) p->Tx_Params.Reading_Data[0] = (int16_t)(*tmp_str_FL.Value*1000.0F);	
		else if (tmp_str_FL.Sign_Symbols & POINT_4) p->Tx_Params.Reading_Data[0] = (int16_t)(*tmp_str_FL.Value*10000.0F);	
	}
	else if ( g_Ram_modbus[Addr].Var->Size & SIZE_Q16 )
	{
		tmp_Q16.Sign_Symbols = g_Ram_modbus[Addr].Var->Size;

		if			(tmp_Q16.Sign_Symbols & POINT_0) tmp_Q16.Wr_Data = INTtoQ16(p->Rx_Params.Writing_Data[0]);
		else if	(tmp_Q16.Sign_Symbols & POINT_1) tmp_Q16.Wr_Data = Q16_DIV(INTtoQ16(p->Rx_Params.Writing_Data[0]),F16(10.0));
		else if (tmp_Q16.Sign_Symbols & POINT_2) tmp_Q16.Wr_Data = Q16_DIV(INTtoQ16(p->Rx_Params.Writing_Data[0]),F16(100.0));
		else if (tmp_Q16.Sign_Symbols & POINT_3) tmp_Q16.Wr_Data = Q16_DIV(INTtoQ16(p->Rx_Params.Writing_Data[0]),F16(1000.0));
		else if (tmp_Q16.Sign_Symbols & POINT_4) tmp_Q16.Wr_Data = Q16_DIV(INTtoQ16(p->Rx_Params.Writing_Data[0]),F16(10000.0));
		
		tmp_Q16.Value = (fix16_t*)g_Ram_modbus[Addr].Var->Value;
		tmp_Q16.Min = (fix16_t*)g_Ram_modbus[Addr].Var->Min;
		tmp_Q16.Max = (fix16_t*)g_Ram_modbus[Addr].Var->Max;
		tmp_Q16.Mode = g_Ram_modbus[Addr].Mode;

		if (p->Rx_Params.Command == CMD_WRITE_DATA)
		{
			if ( ( tmp_Q16.Mode & RW_M ) &&
			( tmp_Q16.Wr_Data >= *tmp_Q16.Min && tmp_Q16.Wr_Data <= *tmp_Q16.Max) )
			{
				*tmp_Q16.Value = tmp_Q16.Wr_Data;
			#if defined( FLASH_USED) || defined(EXT_EEPROM_USED)
				if (tmp_Q16.Mode & MEM_FIX) 
				{
					*p->Par_searching_flag = true;
					*p->Var_pointer = (Var_T*)g_Ram_modbus[Addr].Var;
				}
			#endif				
			}
		}
		if			(tmp_Q16.Sign_Symbols & POINT_0) p->Tx_Params.Reading_Data[0] = Q16toINT(*tmp_Q16.Value);
		else if	(tmp_Q16.Sign_Symbols & POINT_1) p->Tx_Params.Reading_Data[0] = Q16toINT(Q16_MUL(*tmp_Q16.Value,F16(10.0)));
		else if (tmp_Q16.Sign_Symbols & POINT_2) p->Tx_Params.Reading_Data[0] = Q16toINT(Q16_MUL(*tmp_Q16.Value,F16(100.0)));
		else if (tmp_Q16.Sign_Symbols & POINT_3) p->Tx_Params.Reading_Data[0] = Q16toINT(Q16_MUL(*tmp_Q16.Value,F16(1000.0)));
		else if (tmp_Q16.Sign_Symbols & POINT_4) p->Tx_Params.Reading_Data[0] = Q16toINT(Q16_MUL(*tmp_Q16.Value,F16(10000.0)));		
	}
}

bool BD_Modbus_Var_Search (Modbus_Params_T *p)
{
	for (uint8_t i = 0; i < g_Ram_modbus_Size; i++)
	{
		if ( g_Ram_modbus[i].Addr == p->Rx_Params.Data_addr )
		{			
			p->Addr = i;
			return PARAM_FOUND_OK;
		}
	}
	return PARAM_NOT_FOUND_OK;
}

void BD_Modbus_Graphics_Send (Modbus_Params_T *p)
{
	if ( p->Rx_Params.Data_addr == p->Graphic.Addr_CH1 )
	{
		p->Graphic.Graphic_num = 1;
		p->Graphic.During_Addr = p->Graphic.Addr_CH1;
	}
	else if ( p->Rx_Params.Data_addr == p->Graphic.Addr_CH2 )
	{
		p->Graphic.Graphic_num = 2;
		p->Graphic.During_Addr = p->Graphic.Addr_CH2;
	}
	
	for (p->k = 0; p->k < 16; p->k++)
	{
		p->Tx_Params.Reading_Data[p->k] = p->Graphic.Gr_Buffer[p->Graphic.Graphic_num-1][p->k + (p->Rx_Params.Data_addr - p->Graphic.During_Addr) ];
	}
	BD_Modbus_Data_Tx(p);
}

#if defined (JOURNAL_USED)
void BD_Journal_Chack (Modbus_Params_T *p, Journal_T *journal)
{
	uint8_t buffer_shift = 0;
	uint32_t Requesting_Journal_String;
	
	if (p->Rx_Params.Data_addr >= journal->Modbus_End_Addr )
	{	
		Requesting_Journal_String = (p->Rx_Params.Data_addr - journal->Modbus_End_Addr)/p->Rx_Params.Number_of_registers;	// Определяем требуемую строку журнала
		journal->Reading_Addr = journal->Writing_Addr - Requesting_Journal_String*g_Ram_journal_Size*4;										// Пересчитываем в адрес для FLASH
		
		for ( uint8_t i = 0; i < g_Ram_journal_Size; i++ )		// Вычитываем записи журнала и проверяем их битность согласно еню
		{
			journal->Reading_Addr -= 4;		//Возвращаемся к предпоследней считанной переменной
			
			if ( journal->Reading_Addr < JOURNAL_START_ADDRESS )
			{
				journal->Reading_Addr = JOURNAL_END_ADDRESS;
			}

			// Заполняем массив с конца к началу строчки в журнале
			if (g_Ram_journal[(g_Ram_journal_Size-1) - i].Var->Size & SIZE_8)
			{
				if (g_Ram_journal[(g_Ram_journal_Size-1) - i].Var->Size & SIGN)
				{
					p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)*(__IO int8_t*)journal->Reading_Addr;
				}
				else
				{
					p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)*(__IO uint8_t*)journal->Reading_Addr;
				}
			}
			else if (g_Ram_journal[(g_Ram_journal_Size-1) - i].Var->Size & SIZE_16)
			{
				p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (*(__IO uint16_t*)journal->Reading_Addr);	
			}
			else if (g_Ram_journal[(g_Ram_journal_Size-1) - i].Var->Size & SIZE_32 )
			{
				p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)((*(__IO uint32_t*)journal->Reading_Addr)>>16);
				buffer_shift++;
				p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)(*(__IO uint32_t*)journal->Reading_Addr);
			}
			else if (g_Ram_journal[(g_Ram_journal_Size-1) - i].Var->Size & SIZE_FLOAT )
			{
				if			(g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_0) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)(*(__IO float*)journal->Reading_Addr);
				else if	(g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_1) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)(*(__IO float*)journal->Reading_Addr*10.0F);	
				else if (g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_2) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)(*(__IO float*)journal->Reading_Addr*100.0F);	
				else if (g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_3) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)(*(__IO float*)journal->Reading_Addr*1000.0F);	
				else if (g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_4) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = (uint16_t)(*(__IO float*)journal->Reading_Addr*10000.0F);
			}
			else if (g_Ram_journal[(g_Ram_journal_Size-1) - i].Var->Size & SIZE_Q16 )
			{
				if			(g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_0) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = Q16toINT(*(__IO fix16_t*)journal->Reading_Addr);
				else if	(g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_1) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = Q16toINT(Q16_MUL(*(__IO fix16_t*)journal->Reading_Addr,F16(10.0F)));
				else if (g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_2) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = Q16toINT(Q16_MUL(*(__IO fix16_t*)journal->Reading_Addr,F16(100.0F)));
				else if (g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_3) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = Q16toINT(Q16_MUL(*(__IO fix16_t*)journal->Reading_Addr,F16(1000.0F)));
				else if (g_Ram_journal[(g_Ram_journal_Size-1)-i].Var->Size & POINT_4) p->Tx_Params.Reading_Data[(p->Rx_Params.Number_of_registers-1) - (i+buffer_shift) ] = Q16toINT(Q16_MUL(*(__IO fix16_t*)journal->Reading_Addr,F16(10000.0F)));
			}
		}
		BD_Modbus_Data_Tx(p);	
	}	
}
#endif

void BD_MODBUS_ClearGettedData(Modbus_Params_T *p, uint8_t Num_packets_get_prev)
{
	if (p->Modbus_Mode == RS485_MODBUS_MODE || p->Modbus_Mode == BT_MODBUS_MODE)
	{
		for (uint8_t i = 0; i < Num_packets_get_prev; i++) p->ModbusRxBuffer[i] = 0;
	}
	else if (p->Modbus_Mode == USB_MODBUS_MODE)
	{
		for (uint8_t i = 0; i < p->usb_Timer; i++) p->ModbusRxBuffer[i] = 0;
		*p->Byte_timer = 0;
	}
	p->Byte_timer_prev = *p->Byte_timer;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
