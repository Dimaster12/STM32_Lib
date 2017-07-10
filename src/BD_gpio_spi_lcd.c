/* Includes ------------------------------------------------------------------*/
#include "BD_gpio_spi_lcd.h"

const uint8_t Clear_Text[DISPLAY_COLUMNS] = {"                "};

void BD_String_show (uint8_t row, uint8_t column, uint8_t *str, uint8_t sym_num, uint8_t MODE, uint8_t par_num)
{
	#if defined(WEH1602)
	BD_Winstar_1602_String_Show (row, column, str, sym_num, MODE, par_num);
	#elif defined(SEGMENT_BUFFER_SPI)
		
	#endif	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
