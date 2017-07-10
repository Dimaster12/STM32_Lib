/* Includes ------------------------------------------------------------------*/
#include "BD_flash.h"
#include "BD_g_Ram_USER.h"


Flash_T Flash_Params;

void BD_Menu_Flash_Update ( Flash_T *p )
{
	if ( p->Par_Rewrite_flag || p->Journal.Rewrite_flag )
	{
		#if defined (JOURNAL_USED)
		if ( p->Journal.Rewrite_flag )
		{
			if ( p->State == FREE )
			{
				p->Array_Addr = 0;
				if ( p->Journal.Writing_Addr <= ( (uint32_t)( JOURNAL_START_ADDRESS + (FLASH_PAGE_SIZE*p->Journal.Page_Num)-4) ) )
				{
					BD_Menu_Journal_Write_IT(p);
				}
				else
				{
					p->Journal.Page_Num++;					
					if (p->Journal.Page_Num > JOURNAL_PAGES)
					{
						p->Journal.Page_Num = 1;
						p->Journal.Writing_Addr = JOURNAL_START_ADDRESS;
					}
					
					p->Journal.Erase_timer++;
					BD_Flash_Erase_IT(p->Journal.Writing_Addr, p);
				}
			}
			else if ( p->State == ERASED || p->State == JOURN_WRITTEN )
			{
				BD_Menu_Journal_Write_IT(p);
			}			
		}		
		else if ( p->Par_Rewrite_flag )
		#else			
		if ( p->Par_Rewrite_flag )			
		#endif			
		{
			if ( p->State == FREE )
			{
				p->Array_Addr = 0;
				p->Par_Erase_timer++;
				
			#if defined (F1XX)
				BD_Flash_Erase_IT(PARAMS_START_ADDRESS, p);
			#elif defined(F4XX)
				BD_Flash_Erase_IT(PARAMS_SECTOR_NUM, p);				
			#endif	
			}
			else if ( p->State == ERASED || p->State == PAR_WRITTEN )
			{
				BD_Menu_Flash_Write_IT(p);
			}			
		}
	}	
}

void BD_FLASH_Callback ( Flash_T *p )
{
	if (p->State == ERASING) p->State = ERASED;
	#if defined (JOURNAL_USED)
	else if (p->State == JOURN_WRITTING)
	{		
		if ( p->Array_Addr < g_Ram_journal_Size - 1 )
		{
			p->State = JOURN_WRITTEN;
			p->Array_Addr++;
			p->Journal.Writing_Addr+=4;
		}
		else
		{
			p->State = FREE;
			p->Journal.Rewrite_flag = false;	
			p->Par_Rewrite_flag = true;				// Переписываем адрес журнала
			p->Journal.Writing_Addr+=4;
		}
	}
	#endif	
	else if (p->State == PAR_WRITTING)
	{
		if ( p->Array_Addr < g_Ram_flash_Size - 1 )
		{
			p->State = PAR_WRITTEN;
			p->Array_Addr++;
		}
		else
		{
			p->State = FREE;
			p->Par_Rewrite_flag = false;
		}
	}	
}

void HAL_FLASH_EndOfOperationCallback ( uint32_t ReturnValue )
{
	BD_FLASH_Callback(&Flash_Params);
}

#if defined (JOURNAL_USED)
void BD_Menu_Journal_Write_IT ( Flash_T *p )
{	
	uint16_t writing_data_16;
	uint32_t writing_data_32;
	
	p->State = JOURN_WRITTING;
	p->Journal.Flash_write_timer++;
	
	if (g_Ram_journal[p->Array_Addr].Var->Size & SIZE_8)
	{
		writing_data_16 = (uint16_t)*(uint8_t*)g_Ram_journal[p->Array_Addr].Var->Value;
		HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_HALFWORD, p->Journal.Writing_Addr, writing_data_16);
	}		
	else if (g_Ram_journal[p->Array_Addr].Var->Size & SIZE_16)
	{
		writing_data_16 = *(uint16_t*)g_Ram_journal[p->Array_Addr].Var->Value;
		HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_HALFWORD, p->Journal.Writing_Addr, writing_data_16);
	}
	else if ( (g_Ram_journal[p->Array_Addr].Var->Size & SIZE_32	  ) ||
					  (g_Ram_journal[p->Array_Addr].Var->Size & SIZE_FLOAT) ||
					  (g_Ram_journal[p->Array_Addr].Var->Size & SIZE_Q16	) )
	{		
		writing_data_32 = *(uint32_t*)g_Ram_journal[p->Array_Addr].Var->Value;
		HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_WORD, p->Journal.Writing_Addr, writing_data_32);
	}
}
#endif

void BD_Menu_Flash_Write_IT ( Flash_T *p )
{	
	uint16_t writing_data_16;
	uint32_t writing_data_32;
	uint32_t Memory_Addr = PARAMS_START_ADDRESS + ((uint32_t)g_Ram_flash[p->Array_Addr].Addr*4);
	
	p->State = PAR_WRITTING;
	p->Par_Flash_write_timer++;
	
	if (g_Ram_flash[p->Array_Addr].Var->Size & SIZE_8)
	{
		writing_data_16 = (uint16_t)*(uint8_t*)g_Ram_flash[p->Array_Addr].Var->Value;
		HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_HALFWORD, Memory_Addr, writing_data_16);
	}
	else if (g_Ram_flash[p->Array_Addr].Var->Size & SIZE_16)
	{
		writing_data_16 = *(uint16_t*)g_Ram_flash[p->Array_Addr].Var->Value;
		HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_HALFWORD, Memory_Addr, writing_data_16);
	}
	else if ((g_Ram_flash[p->Array_Addr].Var->Size & SIZE_32	 ) || 
					 (g_Ram_flash[p->Array_Addr].Var->Size & SIZE_FLOAT) ||
					 (g_Ram_flash[p->Array_Addr].Var->Size & SIZE_Q16	 ))
	{
		writing_data_32 = *(uint32_t*)g_Ram_flash[p->Array_Addr].Var->Value;
		HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_WORD, Memory_Addr, writing_data_32);
	}
}

void BD_Flash_Erase_IT (uint32_t Sector_Addr, Flash_T *p)
{
  FLASH_EraseInitTypeDef s_eraseinit;

#if defined (F1XX)
	s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.PageAddress = Sector_Addr;
  s_eraseinit.NbPages     = 1;
#elif defined(F4XX)	
  s_eraseinit.TypeErase = FLASH_TYPEERASE_SECTORS;
  s_eraseinit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  s_eraseinit.Sector = Sector_Addr;
  s_eraseinit.NbSectors = 1;
#endif
	p->State = ERASING;
	HAL_FLASHEx_Erase_IT(&s_eraseinit);	
}

void BD_Flash_Init_Params (Flash_T *p)
{
	p->State = FREE;
	p->Array_Addr = 0;
	// Par var
	p->Par_Rewrite_flag = 0;
	p->Par_Erase_timer = 0;
	p->Par_Flash_write_timer = 0;
	
	// Journ var
	p->Journal.Rewrite_flag = 0;
	p->Journal.Erase_timer = 0;
	p->Journal.Flash_write_timer = 0;

	BD_Menu_Read_Params(p);
}

void BD_Menu_Read_Params (Flash_T *p)
{
	uint32_t							during_addr;	
	Var_Flash_8_T					tmp_8;
	Var_Flash_U8_T				tmp_U8;	
	Var_Flash_16_T				tmp_16;
	Var_Flash_U16_T				tmp_U16;	
	Var_Flash_32_T				tmp_32;
	Var_Flash_U32_T				tmp_U32;	
	Var_Flash_FL_T				tmp_FL;
	Var_Flash_Q16_T				tmp_Q16;		

	HAL_FLASH_Unlock();
	for (uint16_t i = 0; i < g_Ram_flash_Size; i++)
	{
		during_addr = PARAMS_START_ADDRESS+((uint32_t)g_Ram_flash[i].Addr*4);

		if (g_Ram_flash[i].Var->Size & SIZE_8)
		{
			if (g_Ram_flash[i].Var->Size & SIGN)
			{
				tmp_8.Readen_Value = (*(__IO int8_t*)during_addr);
				tmp_8.Writing_Value = (int8_t*)g_Ram_flash[i].Var->Value;
				tmp_8.Min = (int8_t*)g_Ram_flash[i].Var->Min;
				tmp_8.Max = (int8_t*)g_Ram_flash[i].Var->Max;
				tmp_8.Factory = (int8_t*)g_Ram_flash[i].Factory;

				if (tmp_8.Readen_Value >= *tmp_8.Min && tmp_8.Readen_Value <= *tmp_8.Max)
				{
					*tmp_8.Writing_Value = tmp_8.Readen_Value;
				}
				else
				{
					p->Par_Rewrite_flag = true;
					*tmp_8.Writing_Value = *tmp_8.Factory;
				}
			}
			else
			{
				tmp_U8.Readen_Value = (*(__IO uint8_t*)during_addr);
				tmp_U8.Writing_Value = (uint8_t*)g_Ram_flash[i].Var->Value;
				tmp_U8.Min = (uint8_t*)g_Ram_flash[i].Var->Min;
				tmp_U8.Max = (uint8_t*)g_Ram_flash[i].Var->Max;
				tmp_U8.Factory = (uint8_t*)g_Ram_flash[i].Factory;

				if (tmp_U8.Readen_Value >= *tmp_U8.Min && tmp_U8.Readen_Value <= *tmp_U8.Max)
				{
					*tmp_U8.Writing_Value = tmp_U8.Readen_Value;
				}
				else
				{
					p->Par_Rewrite_flag = true;
					*tmp_U8.Writing_Value = *tmp_U8.Factory;
				}	
			}
		}
		else if (g_Ram_flash[i].Var->Size & SIZE_16)
		{
			if (g_Ram_flash[i].Var->Size & SIGN)
			{
				tmp_16.Readen_Value = (*(__IO int16_t*)during_addr);
				tmp_16.Writing_Value = (int16_t*)g_Ram_flash[i].Var->Value;
				tmp_16.Min = (int16_t*)g_Ram_flash[i].Var->Min;
				tmp_16.Max = (int16_t*)g_Ram_flash[i].Var->Max;
				tmp_16.Factory = (int16_t*)g_Ram_flash[i].Factory;

				if (tmp_16.Readen_Value >= *tmp_16.Min && tmp_16.Readen_Value <= *tmp_16.Max)
				{
					*tmp_16.Writing_Value = tmp_16.Readen_Value;
				}
				else
				{
					p->Par_Rewrite_flag = true;
					*tmp_16.Writing_Value = *tmp_16.Factory;
				}				
			}
			else
			{
				tmp_U16.Readen_Value = (*(__IO uint16_t*)during_addr);
				tmp_U16.Writing_Value = (uint16_t*)g_Ram_flash[i].Var->Value;
				tmp_U16.Min = (uint16_t*)g_Ram_flash[i].Var->Min;
				tmp_U16.Max = (uint16_t*)g_Ram_flash[i].Var->Max;
				tmp_U16.Factory = (uint16_t*)g_Ram_flash[i].Factory;

				if (tmp_U16.Readen_Value >= *tmp_U16.Min && tmp_U16.Readen_Value <= *tmp_U16.Max)
				{
					*tmp_U16.Writing_Value = tmp_U16.Readen_Value;
				}
				else
				{
					p->Par_Rewrite_flag = true;
					*tmp_U16.Writing_Value = *tmp_U16.Factory;
				}				
			}
		}
		else if (g_Ram_flash[i].Var->Size & SIZE_32)
		{
			if (g_Ram_flash[i].Var->Size & SIGN)
			{
				tmp_32.Readen_Value = (*(__IO int32_t*)during_addr);
				tmp_32.Writing_Value = (int32_t*)g_Ram_flash[i].Var->Value;
				tmp_32.Min = (int32_t*)g_Ram_flash[i].Var->Min;
				tmp_32.Max = (int32_t*)g_Ram_flash[i].Var->Max;
				tmp_32.Factory = (int32_t*)g_Ram_flash[i].Factory;

				if (tmp_32.Readen_Value >= *tmp_32.Min && tmp_32.Readen_Value <= *tmp_32.Max)
				{
					*tmp_32.Writing_Value = tmp_32.Readen_Value;
				}
				else
				{
					p->Par_Rewrite_flag = true;
					*tmp_32.Writing_Value = *tmp_32.Factory;
				}				
			}
			else
			{
				tmp_U32.Readen_Value = (*(__IO uint32_t*)during_addr);
				tmp_U32.Writing_Value = (uint32_t*)g_Ram_flash[i].Var->Value;
				tmp_U32.Min = (uint32_t*)g_Ram_flash[i].Var->Min;
				tmp_U32.Max = (uint32_t*)g_Ram_flash[i].Var->Max;
				tmp_U32.Factory = (uint32_t*)g_Ram_flash[i].Factory;

				if (tmp_U32.Readen_Value >= *tmp_U32.Min && tmp_U32.Readen_Value <= *tmp_U32.Max)
				{
					*tmp_U32.Writing_Value = tmp_U32.Readen_Value;
				}
				else
				{
					p->Par_Rewrite_flag = true;
					*tmp_U32.Writing_Value = *tmp_U32.Factory;
				}
			}
		}
		else if (g_Ram_flash[i].Var->Size & SIZE_FLOAT)
		{
			tmp_FL.Readen_Value = (*(__IO float*)during_addr);
			tmp_FL.Writing_Value = (float*)g_Ram_flash[i].Var->Value;
			tmp_FL.Min = (float*)g_Ram_flash[i].Var->Min;
			tmp_FL.Max = (float*)g_Ram_flash[i].Var->Max;
			tmp_FL.Factory = (float*)g_Ram_flash[i].Factory;

			if (tmp_FL.Readen_Value >= *tmp_FL.Min && tmp_FL.Readen_Value <= *tmp_FL.Max)
			{
				*tmp_FL.Writing_Value = tmp_FL.Readen_Value;
			}
			else
			{
				p->Par_Rewrite_flag = true;
				*tmp_FL.Writing_Value = *tmp_FL.Factory;
			}
		}
		else if (g_Ram_flash[i].Var->Size & SIZE_Q16)
		{
			tmp_Q16.Readen_Value = (*(__IO fix16_t*)during_addr);
			tmp_Q16.Writing_Value = (fix16_t*)g_Ram_flash[i].Var->Value;
			tmp_Q16.Min = (fix16_t*)g_Ram_flash[i].Var->Min;
			tmp_Q16.Max = (fix16_t*)g_Ram_flash[i].Var->Max;
			tmp_Q16.Factory = (fix16_t*)g_Ram_flash[i].Factory;

			if (tmp_Q16.Readen_Value >= *tmp_Q16.Min && tmp_Q16.Readen_Value <= *tmp_Q16.Max)
			{
				*tmp_Q16.Writing_Value = tmp_Q16.Readen_Value;
			}
			else
			{
				p->Par_Rewrite_flag = true;
				*tmp_Q16.Writing_Value = *tmp_Q16.Factory;
			}
		}		
	}
	#if defined (JOURNAL_USED)
	for ( uint8_t i = 1; i <= JOURNAL_PAGES; i++ ) // Поиск текущего листа Flash памяти
	{
		if ( p->Journal.Writing_Addr <= (uint32_t)(JOURNAL_START_ADDRESS + (FLASH_PAGE_SIZE*i) - 4 ) )
		{
			p->Journal.Page_Num = i;
			return;
		}
	}
	#endif	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
