#ifndef __delay_H_dim
#define __delay_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"	 
#include "BD_general.h"	 

void Delay_ms (uint32_t set);			// Miliseconds
void Delay_us(uint32_t set);			// Microseconds

#ifdef __cplusplus
}
#endif
#endif
