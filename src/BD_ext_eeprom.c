/* Includes ------------------------------------------------------------------*/
#include "BD_ext_eeprom.h"

EEPROM_T EEPROM_Params;

void BD_Memory_Update ( EEPROM_T *p )
{
	if (p->Par_searching_flag)
	{
		p->Par_searching_flag = false;
		if (BD_EEPROM_Var_Search(p) == MEM_PARAM_FOUND_OK ) p->Par_writing_flag = true;
	}
	
	if ( p->Par_writing_flag )
	{	
		if      (p->State == FREE) BD_EEPROM_CMD_Write_en_IT(p);
		else if (p->State == CMD_WRITTEN ) BD_EEPROM_Write_Param_IT(p);
	}
	else if ( p->Stat_reading_flag )
	{
		if (p->State == FREE || p->State == STAT_READEN)
		{
			BD_EEPROM_Read_Status_IT(p);
		}
	}
	else if (p->Par_reading_flag)
	{
		if (p->State == PAR_READEN || p->State == FREE)
		{
			BD_EEPROM_Read_Param_IT(p);
		}
	}
}

void BD_EEPROM_Callback ( SPI_HandleTypeDef *hspi, EEPROM_T *p )
{
	if (hspi == SPI_EEPROM_Params.hspi_x)
	{
		HAL_GPIO_WritePin(SPI_EEPROM_Params.CS_GPIO_Port, SPI_EEPROM_Params.CS_GPIO_Pin, GPIO_PIN_SET);
		
		if ( p->State == CMD_WRITING ) p->State = CMD_WRITTEN;
		else if ( p->State == PAR_WRITTING )
		{
			p->State = FREE;
			p->Par_writing_flag = false;
		}
		else if (p->State == STAT_READING)
		{
			p->State = STAT_READEN;
			if (BD_EEPROM_Read_Stat_Callback(p) == EEPROM_READY)
			{
				p->Stat_reading_flag = false;
				p->State = FREE;
			}	
		}
		else if (p->State == PAR_READING)
		{
			if ( BD_EEPROM_Read_Params_Callback(p) == GOOD_MEMORY_DATA )
			{
				if ( p->Reading_Array_Addr < g_Ram_memory_Size-1 )
				{
					p->Reading_Array_Addr++;
					p->State = PAR_READEN;
				}
				else
				{
					p->Par_reading_flag = false;
					p->Reading_Array_Addr = 0;
					p->State = FREE;
				}
			}
			else
			{
				p->State = FREE;
				p->Par_writing_flag = true;
				
				if ( p->Reading_Array_Addr < g_Ram_memory_Size-1 )
				{
					p->Reading_Array_Addr++;
				}
				else
				{
					p->Par_reading_flag = false;
					p->Reading_Array_Addr = 0;					
				}
			}
		}
	}
}

bool BD_EEPROM_Var_Search ( EEPROM_T *p )
{
	for (uint8_t i = 0; i < g_Ram_memory_Size; i++)
	{
		if ( g_Ram_memory[i].Var == p->Var_pointer )
		{
			p->Writing_Array_Addr = i;
			return MEM_PARAM_FOUND_OK;
		}
	}
	return MEM_PARAM_NOT_FOUND;
}

void BD_EEPROM_Write_Param_IT ( EEPROM_T *p )
{
	uint8_t		writing_data_8;
	uint16_t	writing_data_16;
	uint32_t	writing_data_32;
	uint16_t Memory_Addr = g_Ram_memory[p->Writing_Array_Addr].Addr*4;

	p->Stat_reading_flag = true;						//Говорим что надо будет потом считать статус
	p->State = PAR_WRITTING;
	p->Par_EEPROM_write_timer++;

	BD_SPI_DEVICEx_Logical(&SPI_EEPROM_Params);

	SPI_EEPROM_Params.data_send[0] = _WRITE_;																						// Говорим что будем записывать	
	SPI_EEPROM_Params.data_send[1] = (uint8_t)(Memory_Addr>>8);													// Говорим, в какие адреса будем писать
	SPI_EEPROM_Params.data_send[2] = (uint8_t)Memory_Addr;
		
	if (g_Ram_memory[p->Writing_Array_Addr].Var->Size & SIZE_8)
	{
		writing_data_8 = *(uint8_t*)g_Ram_memory[p->Writing_Array_Addr].Var->Value;				// Считываем значение переменной
		SPI_EEPROM_Params.data_send[3] = writing_data_8;																	// Пихаем данные на отправку		
		SPI_EEPROM_Params.Num_packets = 4;
		BD_SPI_Data(&SPI_EEPROM_Params);
	}
	else if (g_Ram_memory[p->Writing_Array_Addr].Var->Size & SIZE_16)
	{
		writing_data_16 = *(uint16_t*)g_Ram_memory[p->Writing_Array_Addr].Var->Value;			// Считываем значение переменной
		SPI_EEPROM_Params.data_send[3] = (uint8_t)(writing_data_16>>8);															
		SPI_EEPROM_Params.data_send[4] = (uint8_t)writing_data_16;														
		SPI_EEPROM_Params.Num_packets = 5;
		BD_SPI_Data(&SPI_EEPROM_Params);
	}
	else if ( (g_Ram_memory[p->Writing_Array_Addr].Var->Size & SIZE_32	 ) || 
					  (g_Ram_memory[p->Writing_Array_Addr].Var->Size & SIZE_FLOAT) ||
					  (g_Ram_memory[p->Writing_Array_Addr].Var->Size & SIZE_Q16  ) )
	{
		writing_data_32 = *(uint32_t*)g_Ram_memory[p->Writing_Array_Addr].Var->Value;			// Считываем значение переменной
		SPI_EEPROM_Params.data_send[3] = (uint8_t)(writing_data_32>>24);															
		SPI_EEPROM_Params.data_send[4] = (uint8_t)(writing_data_32>>16);															
		SPI_EEPROM_Params.data_send[5] = (uint8_t)(writing_data_32>>8);
		SPI_EEPROM_Params.data_send[6] = (uint8_t)writing_data_32;														
		SPI_EEPROM_Params.Num_packets = 7;
		BD_SPI_Data(&SPI_EEPROM_Params);
	}
}

void BD_EEPROM_CMD_Write_en_IT ( EEPROM_T *p )
{
	p->State = CMD_WRITING;

	BD_SPI_DEVICEx_Logical(&SPI_EEPROM_Params);
	
	SPI_EEPROM_Params.data_send[0] = _WREN_;
	SPI_EEPROM_Params.Num_packets = 1;
		
	BD_SPI_Data(&SPI_EEPROM_Params);	
}

void BD_EEPROM_Read_Param_IT ( EEPROM_T *p )
{
	uint16_t	Memory_Addr = g_Ram_memory[p->Reading_Array_Addr].Addr*4;

	p->State = PAR_READING;
	p->Par_EEPROM_read_timer++;

	BD_SPI_DEVICEx_Logical(&SPI_EEPROM_Params);

	SPI_EEPROM_Params.data_send[0] = _READ_;																						// Говорим что будем записывать	
	SPI_EEPROM_Params.data_send[1] = (uint8_t)(Memory_Addr>>8);													// Говорим, в какие адреса будем писать
	SPI_EEPROM_Params.data_send[2] = (uint8_t)Memory_Addr;
		
	if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_8)									SPI_EEPROM_Params.Num_packets = 4;
	else if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_16)						SPI_EEPROM_Params.Num_packets = 5;
	else if ( (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_32	 ) || 
					  (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_FLOAT) ||
					  (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_Q16  ) )		SPI_EEPROM_Params.Num_packets = 7;
	
	BD_SPI_Data(&SPI_EEPROM_Params);
}

void BD_EEPROM_Read_Status_IT(EEPROM_T *p)
{
	p->State = STAT_READING;
	p->Stat_read_timer++;

	BD_SPI_DEVICEx_Logical(&SPI_EEPROM_Params);

	SPI_EEPROM_Params.data_send[0] = _RDSR_;																						// Команда на чтение статуса

	SPI_EEPROM_Params.Num_packets = 2;
	BD_SPI_Data(&SPI_EEPROM_Params);
}

void BD_EEPROM_Init_Params(EEPROM_T *p)
{
	p->State = FREE;
	p->Status.all = 0;
	p->Writing_Array_Addr = 0;
	p->Reading_Array_Addr = 0;

	p->Par_writing_flag = 0;
	p->Par_reading_flag = 1;
	p->Stat_reading_flag = 1;
	
	p->Par_EEPROM_write_timer = 0;
	p->Par_EEPROM_read_timer = 0;
	p->Stat_read_timer = 0;
}

bool BD_EEPROM_Read_Stat_Callback(EEPROM_T *p)
{
	p->Status.all = SPI_EEPROM_Params.data_get[1];
	if (!p->Status.bit.RDY_inv) return EEPROM_READY;
	else return EEPROM_BUSY;
}

bool BD_EEPROM_Read_Params_Callback ( EEPROM_T *p )
{
	Var_Memory_8_T					tmp_8;
	Var_Memory_U8_T					tmp_U8;
	Var_Memory_16_T					tmp_16;
	Var_Memory_U16_T				tmp_U16;
	Var_Memory_32_T					tmp_32;
	Var_Memory_U32_T				tmp_U32;
	Var_Memory_FL_T					tmp_FL;
	Var_Memory_Q16_T				tmp_Q16;

	if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_8)
	{
		if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIGN)
		{
			tmp_8.Readen_Value = (int8_t)SPI_EEPROM_Params.data_get[3];
			tmp_8.Var_fixing_value = (int8_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
			tmp_8.Min = (int8_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
			tmp_8.Max = (int8_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
			tmp_8.Factory = (int8_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

			if (tmp_8.Readen_Value >= *tmp_8.Min && tmp_8.Readen_Value <= *tmp_8.Max)
			{
				*tmp_8.Var_fixing_value = tmp_8.Readen_Value;
				return GOOD_MEMORY_DATA;
			}
			else
			{
				p->Writing_Array_Addr = p->Reading_Array_Addr;
				*tmp_8.Var_fixing_value = *tmp_8.Factory;
				return BAD_MEMORY_DATA;	
			}
		}
		else
		{
			tmp_U8.Readen_Value = (uint8_t)SPI_EEPROM_Params.data_get[3];
			tmp_U8.Var_fixing_value = (uint8_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
			tmp_U8.Min = (uint8_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
			tmp_U8.Max = (uint8_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
			tmp_U8.Factory = (uint8_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

			if (tmp_U8.Readen_Value >= *tmp_U8.Min && tmp_U8.Readen_Value <= *tmp_U8.Max)
			{
				*tmp_U8.Var_fixing_value = tmp_U8.Readen_Value;
				return GOOD_MEMORY_DATA;
			}
			else
			{
				p->Writing_Array_Addr = p->Reading_Array_Addr;
				*tmp_U8.Var_fixing_value = *tmp_U8.Factory;
				return BAD_MEMORY_DATA;			
			}	
		}
	}
	else if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_16)
	{
		if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIGN)
		{
			tmp_16.Readen_Value = ((int16_t)SPI_EEPROM_Params.data_get[3])<<8;
			tmp_16.Readen_Value += (int16_t)SPI_EEPROM_Params.data_get[4];
			
			tmp_16.Var_fixing_value = (int16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
			tmp_16.Min = (int16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
			tmp_16.Max = (int16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
			tmp_16.Factory = (int16_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

			if (tmp_16.Readen_Value >= *tmp_16.Min && tmp_16.Readen_Value <= *tmp_16.Max)
			{
				*tmp_16.Var_fixing_value = tmp_16.Readen_Value;
				return GOOD_MEMORY_DATA;
			}
			else
			{
				p->Writing_Array_Addr = p->Reading_Array_Addr;
				*tmp_16.Var_fixing_value = *tmp_16.Factory;
				return BAD_MEMORY_DATA;	
			}
		}
		else
		{
			tmp_U16.Readen_Value = ((uint16_t)SPI_EEPROM_Params.data_get[3])<<8;
			tmp_U16.Readen_Value += (uint16_t)SPI_EEPROM_Params.data_get[4];

			tmp_U16.Var_fixing_value = (uint16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
			tmp_U16.Min = (uint16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
			tmp_U16.Max = (uint16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
			tmp_U16.Factory = (uint16_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

			if (tmp_U16.Readen_Value >= *tmp_U16.Min && tmp_U16.Readen_Value <= *tmp_U16.Max)
			{
				*tmp_U16.Var_fixing_value = tmp_U16.Readen_Value;
				return GOOD_MEMORY_DATA;
			}
			else
			{
				p->Writing_Array_Addr = p->Reading_Array_Addr;
				*tmp_U16.Var_fixing_value = *tmp_U16.Factory;
				return BAD_MEMORY_DATA;			
			}	
		}
	}
	else if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_32)
	{
		if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIGN)
		{
			tmp_32.Readen_Value  = ((int32_t)SPI_EEPROM_Params.data_get[3])<<24;
			tmp_32.Readen_Value += ((int32_t)SPI_EEPROM_Params.data_get[4])<<16;
			tmp_32.Readen_Value += ((int32_t)SPI_EEPROM_Params.data_get[5])<<8;
			tmp_32.Readen_Value += (int32_t)SPI_EEPROM_Params.data_get[6];
			
			tmp_32.Var_fixing_value = (int32_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
			tmp_32.Min = (int32_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
			tmp_32.Max = (int32_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
			tmp_32.Factory = (int32_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

			if (tmp_32.Readen_Value >= *tmp_32.Min && tmp_32.Readen_Value <= *tmp_32.Max)
			{
				*tmp_32.Var_fixing_value = tmp_32.Readen_Value;
				return GOOD_MEMORY_DATA;
			}
			else
			{
				p->Writing_Array_Addr = p->Reading_Array_Addr;
				*tmp_32.Var_fixing_value = *tmp_32.Factory;
				return BAD_MEMORY_DATA;	
			}
		}
		else
		{
			tmp_U32.Readen_Value  = ((uint32_t)SPI_EEPROM_Params.data_get[3])<<24;
			tmp_U32.Readen_Value += ((uint32_t)SPI_EEPROM_Params.data_get[4])<<16;
			tmp_U32.Readen_Value += ((uint32_t)SPI_EEPROM_Params.data_get[5])<<8;
			tmp_U32.Readen_Value += (uint32_t)SPI_EEPROM_Params.data_get[6];

			tmp_U32.Var_fixing_value = (uint32_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
			tmp_U32.Min = (uint32_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
			tmp_U32.Max = (uint32_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
			tmp_U32.Factory = (uint32_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

			if (tmp_U32.Readen_Value >= *tmp_U32.Min && tmp_U32.Readen_Value <= *tmp_U32.Max)
			{
				*tmp_U32.Var_fixing_value = tmp_U32.Readen_Value;
				return GOOD_MEMORY_DATA;
			}
			else
			{
				p->Writing_Array_Addr = p->Reading_Array_Addr;
				*tmp_U32.Var_fixing_value = *tmp_U32.Factory;
				return BAD_MEMORY_DATA;			
			}	
		}
	}
	else if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_FLOAT)
	{
		tmp_U32.Readen_Value  = ((uint32_t)SPI_EEPROM_Params.data_get[3])<<24;
		tmp_U32.Readen_Value += ((uint32_t)SPI_EEPROM_Params.data_get[4])<<16;
		tmp_U32.Readen_Value += ((uint32_t)SPI_EEPROM_Params.data_get[5])<<8;
		tmp_U32.Readen_Value += (uint32_t)SPI_EEPROM_Params.data_get[6];
		tmp_FL.Readen_Value  = *(float*)&tmp_U32.Readen_Value;
		
		tmp_FL.Var_fixing_value = (float*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
		tmp_FL.Min = (float*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
		tmp_FL.Max = (float*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
		tmp_FL.Factory = (float*)g_Ram_memory[p->Reading_Array_Addr].Factory;

		if (tmp_FL.Readen_Value >= *tmp_FL.Min && tmp_FL.Readen_Value <= *tmp_FL.Max)
		{
			*tmp_FL.Var_fixing_value = tmp_FL.Readen_Value;
			return GOOD_MEMORY_DATA;
		}
		else
		{
			p->Writing_Array_Addr = p->Reading_Array_Addr;
			*tmp_FL.Var_fixing_value = *tmp_FL.Factory;
			return BAD_MEMORY_DATA;			
		}	
	}	
	else if (g_Ram_memory[p->Reading_Array_Addr].Var->Size & SIZE_Q16)
	{
		tmp_Q16.Readen_Value  = (fix16_t)(((uint32_t)SPI_EEPROM_Params.data_get[3])<<24);
		tmp_Q16.Readen_Value += (fix16_t)(((uint32_t)SPI_EEPROM_Params.data_get[4])<<16);
		tmp_Q16.Readen_Value += (fix16_t)(((uint32_t)SPI_EEPROM_Params.data_get[5])<<8);
		tmp_Q16.Readen_Value += (fix16_t)SPI_EEPROM_Params.data_get[6];

		tmp_Q16.Var_fixing_value = (fix16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Value;
		tmp_Q16.Min = (fix16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Min;
		tmp_Q16.Max = (fix16_t*)g_Ram_memory[p->Reading_Array_Addr].Var->Max;
		tmp_Q16.Factory = (fix16_t*)g_Ram_memory[p->Reading_Array_Addr].Factory;

		if (tmp_Q16.Readen_Value >= *tmp_Q16.Min && tmp_Q16.Readen_Value <= *tmp_Q16.Max)
		{
			*tmp_Q16.Var_fixing_value = tmp_Q16.Readen_Value;
			return GOOD_MEMORY_DATA;
		}
		else
		{
			p->Writing_Array_Addr = p->Reading_Array_Addr;
			*tmp_Q16.Var_fixing_value = *tmp_Q16.Factory;
			return BAD_MEMORY_DATA;			
		}	
	}
	return GOOD_MEMORY_DATA;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
