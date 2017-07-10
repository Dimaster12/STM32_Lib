#ifndef __BD_num_string_H_
#define __BD_num_string_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_general.h"

// Var Size
#define U8B					1
#define U16B				2
#define U32B				4

uint8_t str_to_dig_u8b(char *str, uint8_t num);
uint16_t str_to_dig_u16b(char *str, uint8_t num);
uint32_t str_to_dig_u32b(char *str, uint8_t num);

uint8_t BD_Symbols_Count_SIGN (int32_t dig, uint8_t f_n_bit);
uint8_t BD_Symbols_Count_UNSIGN (uint32_t dig, uint8_t f_n_bit);

void BD_dig_to_str_SIGN (uint8_t *str, int32_t dig_sign, uint8_t* num);
void BD_dig_to_str_UNSIGN (uint8_t *str, uint32_t dig_sign, uint8_t num);
	
uint8_t BD_Symbols_Count_FL (uint8_t* str, uint8_t signs);
	 
uint8_t BD_float_to_string ( uint8_t *buffer_array, float var, uint8_t signs );	 
	 
#ifdef __cplusplus
}
#endif
#endif

