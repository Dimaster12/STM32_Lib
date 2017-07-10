/* Includes ------------------------------------------------------------------*/
#include "BD_delay.h"

void Delay_ms (uint32_t set)			// Miliseconds
{
	volatile uint32_t nCount;

	nCount=(HAL_RCC_GetHCLKFreq()/10000)*set;
	for (; nCount!=0; nCount--);
}

void Delay_us(uint32_t set)				// Microseconds
{
	volatile uint32_t nCount;

	nCount=(HAL_RCC_GetHCLKFreq()/10000000)*set;
	for (; nCount!=0; nCount--);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
