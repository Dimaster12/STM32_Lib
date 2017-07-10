#ifndef __LCD_H_dim
#define __LCD_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"

#define STATIC 1
#define DYNAMIC 2

#if defined(WEH1602)
	#include "BD_lcd_winstar_1602.h"
#elif defined(SEGMENT_BUFFER_SPI)
	#include "BD_lcd_7_segments.h"
#endif

extern const uint8_t Clear_Text[DISPLAY_COLUMNS];	 
void BD_String_show (uint8_t row, uint8_t column, uint8_t *str, uint8_t sym_num, uint8_t MODE, uint8_t par_num);	 

#ifdef __cplusplus
}
#endif
#endif
