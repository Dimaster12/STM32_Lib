#ifndef __dyn_array_H_dim
#define __dyn_array_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "dim_lib_spi.h"

#include <stdbool.h>
#include "rt_heap.h"

typedef struct 
{
	// Changing params (must be inited, may be reinited by developer):
	uint16_t								Num_packets;

	// Function inited (must be inited by special function ones):
//	bool								array_was_inited;
	bool								first_array_size_calc;

	uint16_t								Num_packets_prev;
	SPI_DataSize_T					SPI_DataSize_prev;
	
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t									*dyn_get_array_8;
	uint8_t									*dyn_send_array_8;

	uint16_t								*dyn_get_array_16;
	uint16_t								*dyn_send_array_16;	
	
	uint16_t								sizeof_var;
	
} SPI_Dyn_Array_Params_T;

void SPI_dyn_array_Init_Reinit (SPI_Dyn_Array_Params_T *p, SPI_DataSize_T SPIx_DataSize);
void SPI_Dyn_Arrays_Init_Params (SPI_Dyn_Array_Params_T *p);
void MX_SPI_Data(SPI_Dyn_Array_Params_T *p, SPI_HandleTypeDef* hspix);

void *malloc (unsigned int size);
void free (void *p);

#define SPI_MASTER
#ifdef SPI_MASTER

	#define SPI_EN	HAL_GPIO_WritePin(SPI_EN_GPIO_Port, SPI_EN_Pin, GPIO_PIN_RESET)
	#define SPI_DIS	HAL_GPIO_WritePin(SPI_EN_GPIO_Port, SPI_EN_Pin, GPIO_PIN_SET)

#endif

#ifdef __cplusplus
}
#endif
#endif
