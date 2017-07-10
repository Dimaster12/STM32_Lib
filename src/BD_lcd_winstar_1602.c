/* Includes ------------------------------------------------------------------*/
#include "BD_gpio_spi_lcd.h"

void BD_WINSTAR_ClearDisplay(void)
{
	BD_WINSTAR_WriteIns(CLEAR_DISPLAY);
	Delay_ms(200);
}

void BD_WINSTAR_WriteData(uint8_t data)
{	
	RS(BIT_SET);
	RW(BIT_RESET);
	
	data = RusTable[data];

	DB7_adr(data);		
	DB6_adr(data);	
	DB5_adr(data);
	DB4_adr(data);
	#ifdef GPIO_LCD_4BIT_MODE
	BD_WINSTAR_DataReadWrite();
	
	DB7_adr(data<<4);		
	DB6_adr(data<<4);	
	DB5_adr(data<<4);
	DB4_adr(data<<4);	
	BD_WINSTAR_DataReadWrite();	
	
	#else
	DB3_adr(data);	
	DB2_adr(data);	
	DB1_adr(data);	
	DB0_adr(data);

	WINSTAR_DataReadWrite();
	#endif
}

void BD_WINSTAR_WriteIns(uint8_t data)
{	
	RS(BIT_RESET);
	RW(BIT_RESET);
	
	DB7_adr(data);
	DB6_adr(data);
	DB5_adr(data);
	DB4_adr(data);
	
	#ifdef GPIO_LCD_4BIT_MODE
	BD_WINSTAR_DataReadWrite();
	
	DB7_adr(data<<4);
	DB6_adr(data<<4);
	DB5_adr(data<<4);
	DB4_adr(data<<4);
	
	#else
	DB3_adr(data);
	DB2_adr(data);
	DB1_adr(data);
	DB0_adr(data);
	
	#endif
	BD_WINSTAR_DataReadWrite();
}

void BD_WINSTAR_InitDisplay(void)
{
	#ifdef GPIO_LCD_4BIT_MODE
	BD_WINSTAR_Send_Nibble(0x3);
	BD_WINSTAR_Send_Nibble(0x3);
	BD_WINSTAR_Send_Nibble(0x3);
	BD_WINSTAR_Send_Nibble(0x2);
	HAL_Delay(5);	
	BD_WINSTAR_WriteIns(FUNCTION_SET_4BIT_MODE);
	#else
	BD_WINSTAR_WriteIns(FUNCTION_SET_8BIT_MODE);
	#endif
	
	BD_WINSTAR_WriteIns(DISPLAY_OFF);
	BD_WINSTAR_WriteIns(ENTRY_MODE_SET);
	BD_WINSTAR_WriteIns(SET_GRAPHIC);
	BD_WINSTAR_ClearDisplay();
	BD_WINSTAR_WriteIns(RETURN_HOME);
	BD_WINSTAR_WriteIns(DISPLAY_ON);
}

#ifdef GPIO_LCD_4BIT_MODE
void BD_WINSTAR_Send_Nibble(uint8_t data)
{
	RS(BIT_RESET);
	RW(BIT_RESET);

	DB7_adr(data);
	DB6_adr(data);
	DB5_adr(data);
	DB4_adr(data);
	BD_WINSTAR_DataReadWrite();
}
#endif

void BD_WINSTAR_DataReadWrite(void)
{
	EN(BIT_SET);
	Delay_us(100);
	EN(BIT_RESET);
	Delay_us(100);
}

void BD_WINSTAR_SetDDRAMAddress(uint8_t data)
{
	RS(BIT_RESET);
	RW(BIT_RESET);
	
	DB7(BIT_SET);
	DB6_adr(data);
	DB5_adr(data);
	DB4_adr(data);

	#ifdef GPIO_LCD_4BIT_MODE
	BD_WINSTAR_DataReadWrite();
	
	DB7_adr(data<<4);
	DB6_adr(data<<4);
	DB5_adr(data<<4);
	DB4_adr(data<<4);
	BD_WINSTAR_DataReadWrite();
	
	#else
	DB3_adr(data);
	DB2_adr(data);
	DB1_adr(data);
	DB0_adr(data);

	BD_WINSTAR_DataReadWrite();
	#endif
}

void BD_WINSTAR_SET_ADR (uint8_t row, uint8_t column)
{
	uint8_t str;

	str = column + 0x80;
	if(row == 1) str+= 0x40;
	
	BD_WINSTAR_SetDDRAMAddress(str);
}

void BD_Winstar_1602_String_Show (uint8_t row, uint8_t column, uint8_t *str, uint8_t sym_num, uint8_t MODE, uint8_t par_num)
{
	static uint8_t prev_sym_num[3] = {0};
	static uint8_t prev_sym[3][DISPLAY_COLUMNS] = {0};
	uint8_t i;
	
	if (MODE == STATIC)
	{		
		// ќбнул€ем, чтобы при обновлении динамических данных первый раз точно не совпало
		for (i = 0; i < 3; i ++) prev_sym_num[i] = 0;
		
		BD_WINSTAR_SET_ADR(row,column);
		for (i = 0; i < sym_num; i ++) BD_WINSTAR_WriteData(str[i]);
	}
	else if (MODE == DYNAMIC)
	{
		// ≈сли есть нова€ информаци€ дл€ выведени€, только выводим ее на экран
		if ( sym_num != prev_sym_num[par_num] || memcmp(str, prev_sym[par_num], sym_num) != 0 )
		{			
			if ( sym_num < prev_sym_num[par_num] && sym_num < DISPLAY_COLUMNS )
			{
				BD_WINSTAR_SET_ADR ( row, column + (prev_sym_num[par_num]-(prev_sym_num[par_num]-sym_num)) );			
				for (i = 0; i < prev_sym_num[par_num]-sym_num; i++) BD_WINSTAR_WriteData(SPACE);		
			}
			prev_sym_num[par_num] = sym_num;
			BD_WINSTAR_SET_ADR(row,column);
			for (i = 0; i < sym_num; i ++)
			{
				BD_WINSTAR_WriteData(str[i]);
				prev_sym[par_num][i] = str[i];
			}	
		}		
	}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
