/* Includes ------------------------------------------------------------------*/
#include "BD_menu.h"

Menu_T Menu_Params;
uint8_t TEXT_var[DISPLAY_COLUMNS] = {0};

void BD_Menu_Button_View (Menu_T *p)
{
	if (button_pushed (BUT_UP, &BUT_UP_Params) == BUT_PUSHED )
	{		
		button_reset(&BUT_DOWN_Params);
		#if MAX_LEVEL > 1
		button_reset(&BUT_IN_Params);
		button_reset(&BUT_OUT_Params);
		#endif
		p->during_Command = CMD_UP;
	}
	
	if (button_pushed (BUT_DOWN, &BUT_DOWN_Params) == BUT_PUSHED )
	{		
		button_reset(&BUT_UP_Params);
		#if MAX_LEVEL > 1
		button_reset(&BUT_IN_Params);
		button_reset(&BUT_OUT_Params);
		#endif
		p->during_Command = CMD_DOWN;
	}
	#if MAX_LEVEL > 1
	if (button_pushed (BUT_IN, &BUT_IN_Params) == BUT_PUSHED )
	{		
		button_reset(&BUT_UP_Params);
		button_reset(&BUT_DOWN_Params);
		button_reset(&BUT_OUT_Params);

		p->during_Command = CMD_IN;
	}

	if (button_pushed (BUT_OUT, &BUT_OUT_Params) == BUT_PUSHED )
	{		
		button_reset(&BUT_DOWN_Params);
		button_reset(&BUT_UP_Params);
		button_reset(&BUT_IN_Params);

		p->during_Command = CMD_OUT;
	}
	#endif
}

void BD_Menu_Change_Object ( Menu_T *p )
{	
	if ( p->during_Command != CMD_FREE && p->reset_flag) BD_Menu_Init_Params(p);
	if ( p->during_Command != CMD_FREE) p->reset_timer = 0;
	
	// Change Object
	if ( p->during_Command == CMD_UP )
	{
		p->identify_Object = true;
		p->during_Calc++;
		if (p->during_Calc >= p->during_Size)
		{
			p->during_Object = p->start_Object;					// Сбрасываем указатель на нулевой объект массива
			p->during_Calc = 0;
		}
		else p->during_Object++;											// Двигаем указатель на следующий объект массива
	}
	
	else if ( p->during_Command == CMD_DOWN )
	{
		p->identify_Object = true;
		if (p->during_Calc == 0)
		{
			p->during_Calc = p->during_Size - 1;

			p->during_Object = p->start_Object;
			p->during_Object += p->during_Size - 1;
		}
		else
		{
			p->during_Calc--;
			p->during_Object--;
		}
	}

	// Change Folder
	#if MAX_LEVEL > 1
	else if ( p->during_Command == CMD_IN )
	{
		if ( p->during_Object_Type == FOLDER )
		{
			// Size
			p->prev_Size[p->Menu_Level] = p->during_Size;
			p->during_Size = p->during_Folder->Size;
			// Calc
			p->prev_Calc[p->Menu_Level] = p->during_Calc;
			p->during_Calc = 0;
			// Object
			p->prev_Object[p->Menu_Level] = p->during_Object;			// Сохраняем указатель, чтобы потом вернуться
			p->during_Object = p->during_Folder->Content;					// Открываем папку
			p->start_Object = p->during_Object;										// Запоминаем указатель на первый элемент
			// Folder		
			p->during_Folder = (Menu_Folder_T*)p->during_Object->Pointer;

			p->Menu_Level++;
			p->identify_Object = true;						// Нужно идентифицировать новый объект и отобразить его заголовок
			
		}
		else p->during_Command = CMD_FREE;
	}
	else if ( p->during_Command == CMD_OUT )
	{
		if (p->Menu_Level > 0)
		{
			p->Menu_Level--;				
			p->during_Object = p->prev_Object[p->Menu_Level];									// Восстанавливаем доступ к исходной папке
			p->during_Calc = p->prev_Calc[p->Menu_Level];
			p->during_Size = p->prev_Size[p->Menu_Level];
			
			p->during_Object_Type = FOLDER;			
			p->during_Folder = (Menu_Folder_T*)p->during_Object->Pointer;	
			
			p->start_Object = p->during_Object;
			p->start_Object -= p->during_Calc;
			
			p->show_Object = true;																						// Отображаем папку
		}
		p->during_Command = CMD_FREE;
	}
	#endif

	if (p->identify_Object)
	{
		p->during_Command = CMD_FREE;
		p->identify_Object = false;
		p->show_Object = true;

		// Определяем, что за объект
		if (p->during_Object->Type == PARAM)															
		{
			p->during_Object_Type = PARAM;
			p->during_Param[0] = (Menu_Param_T*)p->during_Object->Pointer;					// Достаем данные

			if (p->during_Param[0]->View == DEC)
			{
				p->during_Param_DEC = (Param_DEC_T*)p->during_Param[0]->Pointer;
				p->during_Param_View = DEC;
			}
			else if (p->during_Param[0]->View == STR)
			{
				p->during_Param_STR = (Param_STR_T*)p->during_Param[0]->Pointer;
				p->during_Param_View = STR;
			}
			else if (p->during_Param[0]->View == COMB)
			{
				p->during_Param_COMB = (Param_COMB_T*)p->during_Param[0]->Pointer;
				p->during_Param_View = COMB;				
			}
			else if (p->during_Param[0]->View == UNION)
			{
				p->during_Param_UNION = (Param_UNION_T*)p->during_Param[0]->Pointer;
				p->during_Param_View = UNION;				
			}			
		}
		else if (p->during_Object->Type == FOLDER)
		{
			p->during_Object_Type = FOLDER;
			p->during_Folder = (Menu_Folder_T*)p->during_Object->Pointer;				// Достаем данные
		}
	}

	if (p->show_Object)
	{	
		p->show_Object = false;
		if ( p->during_Object_Type == PARAM )
		{
			if (p->during_Param_View == DEC)
			{
				BD_String_show ( FIRST_LINE, 0, (uint8_t*)(p->during_Param_DEC->Main_Str), DISPLAY_COLUMNS, STATIC, 0);
				BD_String_show ( SECOND_LINE, 0, (uint8_t*)Clear_Text, DISPLAY_COLUMNS ,STATIC, 0);
				BD_String_show ( SECOND_LINE, p->during_Param_DEC->Units_Pos, (uint8_t*)(p->during_Param_DEC->Units_Str), 4 ,STATIC, 0);
			}
			else if (p->during_Param_View == STR)
			{
				BD_String_show ( FIRST_LINE, 0, (uint8_t*)(p->during_Param_STR->Main_Str), DISPLAY_COLUMNS, STATIC, 0);
			}
			else if (p->during_Param_View == COMB)
			{
				BD_String_show ( FIRST_LINE, 0, (uint8_t*)(p->during_Param_COMB->Main_Str), DISPLAY_COLUMNS, STATIC, 0);
				BD_String_show ( SECOND_LINE, 0, (uint8_t*)(p->during_Param_COMB->Second_Str), DISPLAY_COLUMNS, STATIC, 0);
			}
			else if (p->during_Param_View == UNION)
			{
				BD_String_show ( FIRST_LINE, 0, (uint8_t*)(p->during_Param_UNION->Main_Str), DISPLAY_COLUMNS, STATIC, 0);
			}			
			p->LCD_update = true;
		}
		else if ( p->during_Object_Type == FOLDER )
		{
			BD_String_show ( FIRST_LINE, 0, (uint8_t*)(p->during_Folder->First_Str), DISPLAY_COLUMNS, STATIC, 0);
			BD_String_show ( SECOND_LINE, 0, (uint8_t*)(p->during_Folder->Second_Str), DISPLAY_COLUMNS, STATIC, 0);		
		}
	}
}

void BD_Menu_Init_Params ( Menu_T *p )
{
	p->Menu_Level = 0;
	p->during_Object = (Menu_Object_T*)g_Ram_menu;
	p->start_Object = (Menu_Object_T*)g_Ram_menu;
	
	p->during_Calc = 0;
	p->during_Size = g_Ram_menu_Size;
	p->during_Command = CMD_FREE;
	p->during_bit_num = 1;
	p->identify_Object = true;
	p->show_Object = false;
	p->LCD_update = true;
	p->num_symbols = 0;
	
	p->reset_timer = 0;
	p->reset_flag = false;
	p->objects_num = 1;
}

#if defined (MENU_RESET)
void BD_Menu_Reset ( Menu_T *p )
{
	if ( (float)p->reset_timer/(*p->inter_freq) >= p->reset_limit && !p->reset_flag )
	{
		p->reset_flag = true;
		p->during_Object = (Menu_Object_T*)g_Ram_res;
		p->during_Object_Type = PARAM;
		p->objects_num = g_Ram_res_Size;
		p->LCD_update = true;
		p->reset_init_flag = true;
		
		for (uint8_t i = 0; i < p->objects_num; i ++)
		{
			p->during_Param[i] = (Menu_Param_T*)p->during_Object->Pointer;					// Достаем данные
			p->during_Object++;
		}
	}
	else p->reset_timer++;
}
#endif

void BD_Menu_Update_Param ( Menu_T *p )
{
	uint32_t	Var_Value_U32;
	int32_t		Var_Value_32;	
	float			Var_Value_F;
	fix16_t		Var_Value_Q;	
	
	STR_T*		tmp_str;
	COMB_T*		tmp_comb;
	uint16_t	tmp_bit = 1;
	uint16_t	tmp_round = 0;	

	if (!p->reset_flag)
	{
		if ( (p->LCD_update || (float)p->display_timer/(*p->inter_freq) >= 0.4F) && p->during_Object_Type==PARAM )
		{
			p->display_timer = 0;
			p->LCD_update = false;
			
			if ( p->during_Param_View == DEC )
			{		
				if (!(p->during_Param_DEC->Var->Size & SIZE_FLOAT) && !(p->during_Param_DEC->Var->Size & SIZE_Q16))
				{
					if (!p->during_Param_DEC->Var->Size & SIGN)
					{
						if			( p->during_Param_DEC->Var->Size & SIZE_8  )	Var_Value_U32 = *(uint8_t*) p->during_Param_DEC->Var->Value;
						else if	( p->during_Param_DEC->Var->Size & SIZE_16 )	Var_Value_U32 = *(uint16_t*)p->during_Param_DEC->Var->Value;
						else if	( p->during_Param_DEC->Var->Size & SIZE_32 )	Var_Value_U32 = *(uint32_t*)p->during_Param_DEC->Var->Value;
						
						p->num_symbols = BD_Symbols_Count_UNSIGN ( Var_Value_U32, p->during_Param_DEC->Var->Size );
						BD_dig_to_str_UNSIGN (TEXT_var, Var_Value_U32, p->num_symbols);
					}
					else
					{				
						if			( p->during_Param_DEC->Var->Size & SIZE_8  )	Var_Value_32 = *(int8_t*) p->during_Param_DEC->Var->Value;
						else if	( p->during_Param_DEC->Var->Size & SIZE_16 )	Var_Value_32 = *(int16_t*)p->during_Param_DEC->Var->Value;
						else if	( p->during_Param_DEC->Var->Size & SIZE_32 )	Var_Value_32 = *(int32_t*)p->during_Param_DEC->Var->Value;

						p->num_symbols = BD_Symbols_Count_SIGN ( Var_Value_32, p->during_Param_DEC->Var->Size );
						BD_dig_to_str_SIGN (TEXT_var, Var_Value_32, &p->num_symbols);		
					}
					BD_String_show (SECOND_LINE, 6, TEXT_var, p->num_symbols, DYNAMIC, 0);
				}
				else
				{
					if (p->during_Param_DEC->Var->Size & SIZE_FLOAT)
					{
						Var_Value_F = *(float*)p->during_Param_DEC->Var->Value;							
						if				( p->during_Param_DEC->Var->Size & POINT_0 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 0 );
						else if		( p->during_Param_DEC->Var->Size & POINT_1 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 1 );
						else if		( p->during_Param_DEC->Var->Size & POINT_2 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 2 );
						else if		( p->during_Param_DEC->Var->Size & POINT_3 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 3 );
						else if		( p->during_Param_DEC->Var->Size & POINT_4 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 4 );
						BD_String_show (SECOND_LINE, 6, TEXT_var, p->num_symbols, DYNAMIC, 0);
					}
					else if (p->during_Param_DEC->Var->Size & SIZE_Q16)
					{
						Var_Value_Q = *(fix16_t*)p->during_Param_DEC->Var->Value;
						if				( p->during_Param_DEC->Var->Size & POINT_0 ) p->num_symbols = BD_float_to_string (TEXT_var, Q16toFL(Var_Value_Q), 0 );
						else if		( p->during_Param_DEC->Var->Size & POINT_1 ) p->num_symbols = BD_float_to_string (TEXT_var, Q16toFL(Var_Value_Q), 1 );
						else if		( p->during_Param_DEC->Var->Size & POINT_2 ) p->num_symbols = BD_float_to_string (TEXT_var, Q16toFL(Var_Value_Q), 2 );
						else if		( p->during_Param_DEC->Var->Size & POINT_3 ) p->num_symbols = BD_float_to_string (TEXT_var, Q16toFL(Var_Value_Q), 3 );
						else if		( p->during_Param_DEC->Var->Size & POINT_4 ) p->num_symbols = BD_float_to_string (TEXT_var, Q16toFL(Var_Value_Q), 4 );
						BD_String_show (SECOND_LINE, 6, TEXT_var, p->num_symbols, DYNAMIC, 0);
					}
				}
			}
			else if ( p->during_Param_View == STR )
			{
				if			( p->during_Param_STR->Var->Size & SIZE_8 )			Var_Value_32 = (uint32_t)*(uint8_t*) p->during_Param_STR->Var->Value;
				else if	( p->during_Param_STR->Var->Size & SIZE_16 )		Var_Value_32 = (uint32_t)*(uint16_t*)p->during_Param_STR->Var->Value;

				tmp_str = (STR_T*)p->during_Param_STR->Var_Str;
				tmp_str += Var_Value_32;
				BD_String_show (SECOND_LINE, 0, (uint8_t*)tmp_str, DISPLAY_COLUMNS, DYNAMIC, 0);
			}
			else if ( p->during_Param_View == COMB )
			{
				tmp_comb = (COMB_T*)p->during_Param_COMB->Var_array;

				for (uint8_t k = 0; k < p->during_Param_COMB->Var_Number; k ++)
				{
					if			( tmp_comb->Var_x->Size & SIZE_8 )	Var_Value_32 = (uint32_t)*(uint8_t*) tmp_comb->Var_x->Value;
					else if	( tmp_comb->Var_x->Size & SIZE_16 )	Var_Value_32 = (uint32_t)*(uint16_t*)tmp_comb->Var_x->Value;
					else if	( tmp_comb->Var_x->Size & SIZE_32 )	Var_Value_32 = (uint32_t)*(uint32_t*)tmp_comb->Var_x->Value;

					p->num_symbols = BD_Symbols_Count_UNSIGN ( Var_Value_32, tmp_comb->Var_x->Size );
					BD_dig_to_str_UNSIGN (TEXT_var, Var_Value_32, p->num_symbols);
					BD_String_show (SECOND_LINE, tmp_comb->Pos_x, TEXT_var, p->num_symbols, DYNAMIC, k);	
					tmp_comb++;
				}		
			}
			else if ( p->during_Param_View == UNION )
			{
				if			( p->during_Param_UNION->Var->Size & SIZE_8 )		Var_Value_32 = (uint32_t)*(uint8_t*) p->during_Param_UNION->Var->Value;
				else if	( p->during_Param_UNION->Var->Size & SIZE_16 )	Var_Value_32 = (uint32_t)*(uint16_t*)p->during_Param_UNION->Var->Value;		
				
				if (Var_Value_32)
				{
					tmp_bit <<= p->during_bit_num-1;
					while (tmp_round<2)
					{
						if ( p->during_bit_num > p->during_Param_UNION->Bits_Num )
						{
							tmp_bit = 1;
							p->during_bit_num = 1;
						}						
						p->during_bit_num++;						
						if ( tmp_bit & Var_Value_32 ) break;
						tmp_bit<<=1;
					}
					BD_String_show (SECOND_LINE, 0, (uint8_t*)(&p->during_Param_UNION->Var_Str[p->during_bit_num-1]), DISPLAY_COLUMNS, DYNAMIC, 0);
				}
				else
				{
					BD_String_show (SECOND_LINE, 0, (uint8_t*)(&p->during_Param_UNION->Var_Str[0]), DISPLAY_COLUMNS, DYNAMIC, 0);
					p->during_bit_num = 1;
				}
			}			
		}
		else p->display_timer++;
	}
}

void BD_Menu_Update_Param_Reset ( Menu_T *p )
{
	uint32_t	Var_Value_32;
	float			Var_Value_F;	
	
	STR_T*		tmp_str;
	COMB_T*		tmp_comb;
	if (p->reset_flag)
	{
		if ( (p->LCD_update || (float)p->display_timer/(*p->inter_freq) >= 0.4F) && p->during_Object_Type==PARAM )
		{
			p->display_timer = 0;
			p->LCD_update = false;

			for (uint8_t i = 0; i < p->objects_num; i ++)
			{
				if (p->during_Param[i]->View == DEC)
				{
					p->during_Param_DEC_RES = (Param_DEC_RES_T*)p->during_Param[i]->Pointer;
					p->during_Param_View = DEC;
					
					if (p->reset_init_flag)
					{
						BD_String_show ( p->during_Param_DEC_RES->Str_Num, p->during_Param_DEC_RES->Units_Pos, (uint8_t*)(p->during_Param_DEC_RES->Units_Str), 4 ,STATIC, 0);						
					}
				}
				else if (p->during_Param[i]->View == STR)
				{
					p->during_Param_STR_RES = (Param_STR_RES_T*)p->during_Param[i]->Pointer;
					p->during_Param_View = STR;
				}
				else if (p->during_Param[i]->View == COMB)
				{
					p->during_Param_COMB_RES = (Param_COMB_RES_T*)p->during_Param[i]->Pointer;
					p->during_Param_View = COMB;
					if (p->reset_init_flag)
					{
						BD_String_show ( p->during_Param_COMB_RES->Str_Num, 0, (uint8_t*)(p->during_Param_COMB_RES->Second_Str), DISPLAY_COLUMNS, STATIC, 0);
					}					
				}
				
				if ( p->during_Param_View == DEC )
				{		
					if (!(p->during_Param_DEC_RES->Var->Size & SIZE_FLOAT))
					{
						if			( p->during_Param_DEC_RES->Var->Size & SIZE_8  )	Var_Value_32 = (uint32_t)*(uint8_t*) p->during_Param_DEC_RES->Var->Value;
						else if	( p->during_Param_DEC_RES->Var->Size & SIZE_16 )	Var_Value_32 = (uint32_t)*(uint16_t*)p->during_Param_DEC_RES->Var->Value;
						else if	( p->during_Param_DEC_RES->Var->Size & SIZE_32 )	Var_Value_32 = (uint32_t)*(uint32_t*)p->during_Param_DEC_RES->Var->Value;

						p->num_symbols = BD_Symbols_Count_UNSIGN ( Var_Value_32, p->during_Param_DEC_RES->Var->Size );
						BD_dig_to_str_UNSIGN (TEXT_var, Var_Value_32, p->num_symbols);
						BD_String_show (p->during_Param_DEC_RES->Str_Num, p->during_Param_DEC_RES->Var_Pos, TEXT_var, p->num_symbols, DYNAMIC, i);
					}
					else
					{
						Var_Value_F = *(float*)p->during_Param_DEC_RES->Var->Value;							
						if				( p->during_Param_DEC_RES->Var->Size & POINT_1 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 1 );
						else if		( p->during_Param_DEC_RES->Var->Size & POINT_2 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 2 );
						else if		( p->during_Param_DEC_RES->Var->Size & POINT_3 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 3 );
						else if		( p->during_Param_DEC_RES->Var->Size & POINT_4 ) p->num_symbols = BD_float_to_string (TEXT_var, Var_Value_F, 4 );
						BD_String_show (p->during_Param_DEC_RES->Str_Num, p->during_Param_DEC_RES->Var_Pos, TEXT_var, p->num_symbols, DYNAMIC, i);
					}
				}
				else if ( p->during_Param_View == STR )
				{
					if			( p->during_Param_STR_RES->Var->Size & SIZE_8 )			Var_Value_32 = (uint32_t)*(uint8_t*) p->during_Param_STR_RES->Var->Value;
					else if	( p->during_Param_STR_RES->Var->Size & SIZE_16 )		Var_Value_32 = (uint32_t)*(uint16_t*)p->during_Param_STR_RES->Var->Value;

					tmp_str = (STR_T*)p->during_Param_STR_RES->Var_Str;
					tmp_str += Var_Value_32;
					BD_String_show (p->during_Param_STR_RES->Str_Num, 0, (uint8_t*)tmp_str, DISPLAY_COLUMNS, DYNAMIC, i);
				}
				else if ( p->during_Param_View == COMB )
				{
					tmp_comb = (COMB_T*)p->during_Param_COMB_RES->Var_array;

					for (uint8_t k = 0; k < p->during_Param_COMB_RES->Var_Number; k ++)
					{
						if			( tmp_comb->Var_x->Size & SIZE_8 )	Var_Value_32 = (uint32_t)*(uint8_t*) tmp_comb->Var_x->Value;
						else if	( tmp_comb->Var_x->Size & SIZE_16 )	Var_Value_32 = (uint32_t)*(uint16_t*)tmp_comb->Var_x->Value;
						else if	( tmp_comb->Var_x->Size & SIZE_32 )	Var_Value_32 = (uint32_t)*(uint32_t*)tmp_comb->Var_x->Value;

						p->num_symbols = BD_Symbols_Count_UNSIGN ( Var_Value_32, tmp_comb->Var_x->Size );
						BD_dig_to_str_UNSIGN (TEXT_var, Var_Value_32, p->num_symbols);
						BD_String_show (p->during_Param_COMB_RES->Str_Num, tmp_comb->Pos_x, TEXT_var, p->num_symbols, DYNAMIC, k);
						tmp_comb++;
					}		
				}
			}
			p->reset_init_flag = false;
		}
		else p->display_timer++;
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
