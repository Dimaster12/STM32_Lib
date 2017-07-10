#include "BD_num_string.h"

uint8_t BD_Symbols_Count_FL (uint8_t* str, uint8_t signs )
{
	uint8_t* tmp_str = str;
	uint8_t num = 0;
	
	for (uint8_t i = 0; i < 16; i++ )
	{
		if ( *tmp_str == 46 )
		{
			num += (signs+1);
			return(num);
		}
		else num++;
		tmp_str++;
	}
	return(num);
}

uint8_t BD_float_to_string ( uint8_t *buffer_array, float var, uint8_t signs )
{
	uint32_t		tmp_32;
	float				tmp_FLOAT;
	uint8_t			num_symbols;
	uint8_t			symbol_num = 0;
	uint8_t			array_num = 0;
	uint8_t			k = 0;	
	uint8_t			tmp_array[16] = {0};
	
	tmp_FLOAT = var;
	if (var < 0.0F)
	{
		tmp_FLOAT *= -1.0F;		
		buffer_array[array_num] = '-';
		array_num++;
		k++;
	}

	if ( tmp_FLOAT < 1.0F )
	{
		buffer_array[array_num] = '0';
		array_num++;
		k++;		
	}

	if				( signs == 0 ) tmp_FLOAT *= 1.0F;	
	else if		( signs == 1 ) tmp_FLOAT *= 10.0F;
	else if		( signs == 2 ) tmp_FLOAT *= 100.0F;
	else if		( signs == 3 ) tmp_FLOAT *= 1000.0F;
	else if		( signs == 4 ) tmp_FLOAT *= 10000.0F;

	tmp_32 = (uint32_t)tmp_FLOAT;
	num_symbols = BD_Symbols_Count_UNSIGN(tmp_32, U32B);
	BD_dig_to_str_UNSIGN(tmp_array, tmp_32, num_symbols);

	for (; array_num < (num_symbols - signs)+k; array_num++)
	{
		buffer_array[array_num] = tmp_array[symbol_num];
		symbol_num++;
	}
	
	if (signs != 0)
	{
		buffer_array[array_num] = '.';
		array_num++;
	}
	else return array_num;
		
	for (; array_num < num_symbols+k+1; array_num++ )
	{	
		buffer_array[array_num] = tmp_array[symbol_num];
		symbol_num++;
	}
	return array_num;
}

uint8_t BD_Symbols_Count_SIGN (int32_t dig, uint8_t f_n_bit)
{
	uint8_t		num = 0;
	
	if ( f_n_bit == U8B )
	{
		if( (dig / 100) != 0) num = 3;
		else if( (dig / 10) != 0) num = 2;
		else num = 1;
	}	
	else if ( f_n_bit == U16B )
	{
		if( (dig / 10000) != 0) num = 5;
		else if( (dig / 1000) != 0) num = 4;
		else if( (dig / 100) != 0) num = 3;
		else if( (dig / 10) != 0) num = 2;
		else num = 1;
	}	
	else if ( f_n_bit == U32B )
	{
		if( (dig / 1000000000) != 0) num = 10;
		else if( (dig / 100000000) != 0) num = 9;
		else if( (dig / 10000000) != 0) num = 8;
		else if( (dig / 1000000) != 0) num = 7;
		else if( (dig / 100000) != 0) num = 6;
		else if( (dig / 10000) != 0) num = 5;
		else if( (dig / 1000) != 0) num = 4;
		else if( (dig / 100) != 0) num = 3;
		else if( (dig / 10) != 0) num = 2;
		else num = 1;
	}
	return(num);
}

uint8_t BD_Symbols_Count_UNSIGN (uint32_t dig, uint8_t f_n_bit)
{
	uint8_t		num = 0;
	
	if ( f_n_bit == U8B )
	{
		if( (dig / 100) != 0) num = 3;
		else if( (dig / 10) != 0) num = 2;
		else num = 1;
	}	
	else if ( f_n_bit == U16B )
	{
		if( (dig / 10000) != 0) num = 5;
		else if( (dig / 1000) != 0) num = 4;
		else if( (dig / 100) != 0) num = 3;
		else if( (dig / 10) != 0) num = 2;
		else num = 1;
	}	
	else if ( f_n_bit == U32B )
	{
		if( (dig / 1000000000) != 0) num = 10;
		else if( (dig / 100000000) != 0) num = 9;
		else if( (dig / 10000000) != 0) num = 8;
		else if( (dig / 1000000) != 0) num = 7;
		else if( (dig / 100000) != 0) num = 6;
		else if( (dig / 10000) != 0) num = 5;
		else if( (dig / 1000) != 0) num = 4;
		else if( (dig / 100) != 0) num = 3;
		else if( (dig / 10) != 0) num = 2;
		else num = 1;
	}
	return(num);
}

void BD_dig_to_str_UNSIGN (uint8_t *str, uint32_t dig, uint8_t num)
{
	switch(num)
	{
		case 1:
			str[0] = (dig % 10) + 0x30;
		break;
		
		case 2:
			str[0] = ((dig % 100) / 10) + 0x30;
			str[1] = (dig % 10) + 0x30;
		break;
		
		case 3:
			str[0] = ((dig % 1000) / 100) + 0x30;
			str[1] = ((dig % 100) / 10) + 0x30;
			str[2] = (dig % 10) + 0x30;		
		break;
		
		case 4:
			str[0] = ((dig % 10000) / 1000) + 0x30;
			str[1] = ((dig % 1000) / 100) + 0x30;
			str[2] = ((dig % 100) / 10) + 0x30;
			str[3] = (dig % 10) + 0x30;				
		break;
		
		case 5:
			str[0] = ((dig % 100000) / 10000) + 0x30;
			str[1] = ((dig % 10000) / 1000) + 0x30;
			str[2] = ((dig % 1000) / 100) + 0x30;
			str[3] = ((dig % 100) / 10) + 0x30;
			str[4] = (dig % 10) + 0x30;		
		break;
		
		case 6:
			str[0] = ((dig % 1000000) / 100000) + 0x30;
			str[1] = ((dig % 100000) / 10000) + 0x30;
			str[2] = ((dig % 10000) / 1000) + 0x30;
			str[3] = ((dig % 1000) / 100) + 0x30;
			str[4] = ((dig % 100) / 10) + 0x30;
			str[5] = (dig % 10) + 0x30;		
		break;
		
		case 7:
			str[0] = ((dig % 10000000) / 1000000) + 0x30;
			str[1] = ((dig % 1000000) / 100000) + 0x30;
			str[2] = ((dig % 100000) / 10000) + 0x30;
			str[3] = ((dig % 10000) / 1000) + 0x30;
			str[4] = ((dig % 1000) / 100) + 0x30;
			str[5] = ((dig % 100) / 10) + 0x30;
			str[6] = (dig % 10) + 0x30;		
		break;
		
		case 8:
			str[0] = ((dig % 100000000) / 10000000) + 0x30;
			str[1] = ((dig % 10000000) / 1000000) + 0x30;
			str[2] = ((dig % 1000000) / 100000) + 0x30;
			str[3] = ((dig % 100000) / 10000) + 0x30;
			str[4] = ((dig % 10000) / 1000) + 0x30;
			str[5] = ((dig % 1000) / 100) + 0x30;
			str[6] = ((dig % 100) / 10) + 0x30;
			str[7] = (dig % 10) + 0x30;			
		break;
		
		case 9:
			str[0] = ((dig % 1000000000) / 100000000) + 0x30;
			str[1] = ((dig % 100000000) / 10000000) + 0x30;
			str[2] = ((dig % 10000000) / 1000000) + 0x30;
			str[3] = ((dig % 1000000) / 100000) + 0x30;
			str[4] = ((dig % 100000) / 10000) + 0x30;
			str[5] = ((dig % 10000) / 1000) + 0x30;
			str[6] = ((dig % 1000) / 100) + 0x30;
			str[7] = ((dig % 100) / 10) + 0x30;
			str[8] = (dig % 10) + 0x30;		
		break;
		
		case 10:
			str[0] = ((dig % 10000000000) / 1000000000) + 0x30;
			str[1] = ((dig % 1000000000) / 100000000) + 0x30;
			str[2] = ((dig % 100000000) / 10000000) + 0x30;
			str[3] = ((dig % 10000000) / 1000000) + 0x30;
			str[4] = ((dig % 1000000) / 100000) + 0x30;
			str[5] = ((dig % 100000) / 10000) + 0x30;
			str[6] = ((dig % 10000) / 1000) + 0x30;
			str[7] = ((dig % 1000) / 100) + 0x30;
			str[8] = ((dig % 100) / 10) + 0x30;
			str[9] = (dig % 10) + 0x30;
		break;
	}
}

void BD_dig_to_str_SIGN (uint8_t *str, int32_t dig_sign, uint8_t* num)
{
	uint32_t dig;
	uint8_t sign_shift = 0;
	bool sign_flag = false;	
	
	if (dig_sign<0) 
	{	
		dig = dig_sign*(-1);
		str[0] = '-';
		sign_shift++;
		sign_flag = true;
	}
	else dig = dig_sign;
	
	switch(*num)
	{
		case 1:
			str[sign_shift] = (dig % 10) + 0x30;
		break;
		
		case 2:
			str[sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;
		break;
		
		case 3:
			str[sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 4:
			str[sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;				
		break;
		
		case 5:
			str[sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 6:
			str[sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 7:
			str[sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 8:
			str[sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;			
		break;
		
		case 9:
			str[sign_shift] = ((dig % 1000000000) / 100000000) + 0x30;
			str[++sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 10:
			str[sign_shift] = ((dig % 10000000000) / 1000000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000000) / 100000000) + 0x30;
			str[++sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;
		break;
	}
	if (sign_flag) (*num)++;
}
