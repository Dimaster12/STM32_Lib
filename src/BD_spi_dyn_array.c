/* Includes ------------------------------------------------------------------*/
#include "dim_spi_dyn_array.h"


__IO uint16_t data_send_16[25], data_get_16[25];
__IO uint8_t data_send_8[25], data_get_8[25];

void SPI_dyn_array_Init_Reinit (SPI_Dyn_Array_Params_T *p, SPI_DataSize_T SPIx_DataSize)
{
	//if (SPIx_DataSize != p->SPIx_DataSize_prev ||  )
	//if ( memcmp(&v.Array_Recalc, &p->Array_Recalc_prev, sizeof(p->Array_Recalc_prev)) != 0 )
	if (SPIx_DataSize != p->SPI_DataSize_prev || p->Num_packets != p->Num_packets_prev)
	{
		if (!p->first_array_size_calc)
		{
			if (p->SPI_DataSize_prev == Data_Size_8BIT)
			{
				free(p->dyn_get_array_8);
				free(p->dyn_send_array_8);
			}
			else if (p->SPI_DataSize_prev == Data_Size_16BIT) 
			{
				free(p->dyn_get_array_16);
				free(p->dyn_send_array_16);
			}				
		}	
		/*
		else
		{
			if (SPIx_DataSize == Data_Size_8BIT) p->sizeof_var = sizeof(uint8_t);
			else if (SPIx_DataSize == Data_Size_16BIT) p->sizeof_var = sizeof(uint16_t);
		}
		*/
		p->SPI_DataSize_prev = SPIx_DataSize;
		p->Num_packets_prev = p->Num_packets;
		
		p->first_array_size_calc = false;
		
		if (p->SPI_DataSize_prev == Data_Size_8BIT)
		{
			p->sizeof_var = sizeof(uint8_t);
			p->dyn_get_array_8 = (uint8_t*) malloc((uint16_t)(p->sizeof_var*p->Num_packets_prev));
			p->dyn_send_array_8 = (uint8_t*) malloc((uint16_t)(p->sizeof_var*p->Num_packets_prev));
		}
		else if (p->SPI_DataSize_prev == Data_Size_16BIT)
		{
			p->sizeof_var = sizeof(uint16_t);
			p->dyn_get_array_16 = (uint16_t*) malloc((uint16_t)(p->sizeof_var*p->Num_packets_prev));
			p->dyn_send_array_16 = (uint16_t*) malloc((uint16_t)(p->sizeof_var*p->Num_packets_prev));			
		}
	}	
}

void SPI_Dyn_Arrays_Init_Params (SPI_Dyn_Array_Params_T *p)
{
	p->first_array_size_calc = true;
	p->Num_packets_prev = 0;
	//p->SPI_DataSize_prev = 0;
	//p->array_was_inited = true;
}

void MX_SPI_Data(SPI_Dyn_Array_Params_T *p, SPI_HandleTypeDef* hspix)
{
	SPI_EN;
	if (p->SPI_DataSize_prev == Data_Size_8BIT)
	{
		HAL_SPI_TransmitReceive_IT(hspix, (uint8_t*)data_send_8, (uint8_t*)data_get_8, p->Num_packets_prev);
		//HAL_SPI_TransmitReceive_IT(hspix, (uint8_t*)p->dyn_send_array_8, (uint8_t*)p->dyn_get_array_8, p->Num_packets_prev);
	}
	else if (p->SPI_DataSize_prev == Data_Size_16BIT)
	{		
		HAL_SPI_TransmitReceive_IT(hspix, (uint8_t*)data_send_16, (uint8_t*)data_get_16, p->Num_packets_prev);
		//HAL_SPI_TransmitReceive_IT(hspix, (uint8_t*)p->dyn_send_array_16, (uint8_t*)p->dyn_get_array_16, p->Num_packets_prev);	
	}
	while ( HAL_SPI_GetState(hspix) != HAL_SPI_STATE_READY);
	SPI_DIS; 
}
