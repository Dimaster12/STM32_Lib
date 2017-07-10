/* Includes ------------------------------------------------------------------*/
#include "BD_spi.h"

/*
void BD_SPI_Logical (SPIx_Params_T *p)
{	
	if (!p->SPIx_init_was)
	{
		p->SPIx_Init_func();
		p->SPIx_init_was = true;

		p->Reg_Params.DataSize = p->hspi_x->Init.DataSize;
		p->Reg_Params.Presc = p->hspi_x->Init.BaudRatePrescaler;
		p->Reg_Params.CRC_en = p->hspi_x->Init.CRCCalculation;
		
		p->Reg_Params_prev = p->Reg_Params;
	}
	else if ( memcmp(&p->Reg_Params, &p->Reg_Params_prev, sizeof(p->Reg_Params)) != 0 )
	{	
		p->Reg_Params_prev = p->Reg_Params;
		BD_SPI_ReInit(p);
	}
	
	if (p->Num_packets != p->Num_packets_prev )
	{
		p->Num_packets_prev = p->Num_packets;	
	}
}
*/

void BD_SPI_DEVICEx_Logical (SPI_DEVICEx_Params_T *p)
{
	bool reinit_flag = false;
	
	if (p->hspi_x->Init.DataSize != p->Reg_Params.DataSize)
	{
		p->hspi_x->Init.DataSize = p->Reg_Params.DataSize;
		reinit_flag = true;
	}
	if (p->hspi_x->Init.BaudRatePrescaler != p->Reg_Params.BaudRatePrescaler)
	{
		p->hspi_x->Init.BaudRatePrescaler = p->Reg_Params.BaudRatePrescaler;
		reinit_flag = true;		
	}
	if (p->hspi_x->Init.CRCCalculation != p->Reg_Params.CRCCalculation)
	{
		p->hspi_x->Init.CRCCalculation = p->Reg_Params.CRCCalculation;
		reinit_flag = true;
	}	
	if (reinit_flag) HAL_SPI_Init(p->hspi_x);
}
/*
void BD_SPI_Logical_Init_Params (SPIx_Params_T *p)
{
	p->SPIx_init_was 							= false;
	p->SPIx_function_break 				=	0;
}

void BD_SPI_ReInit (SPIx_Params_T *p)
{
	p->hspi_x->Init.DataSize = p->Reg_Params.DataSize;
	p->hspi_x->Init.BaudRatePrescaler = p->Reg_Params.Presc;
  p->hspi_x->Init.CRCCalculation = p->Reg_Params.CRC_en;
	HAL_SPI_Init(p->hspi_x);
}
*/

void BD_SPI_Data (SPI_DEVICEx_Params_T *p)
{
	if (p->hspi_x->Init.Mode == SPI_MODE_MASTER) HAL_GPIO_WritePin(p->CS_GPIO_Port, p->CS_GPIO_Pin, GPIO_PIN_RESET);

	if (p->data_mode == TX_EN)					HAL_SPI_Transmit_DMA(p->hspi_x, (uint8_t*)p->data_send, p->Num_packets);
	else if (p->data_mode == RX_EN)			HAL_SPI_Receive_DMA(p->hspi_x, (uint8_t*)p->data_get, p->Num_packets);
	else if (p->data_mode == RX_TX_EN)	HAL_SPI_TransmitReceive_DMA(p->hspi_x, (uint8_t*)p->data_send, (uint8_t*)p->data_get, p->Num_packets);
}

/*
void BD_SPI_Data (SPI_DEVICEx_Params_T *p)
{
	#ifdef SPI_MASTER
	__IO uint8_t *temp_send_adr = p->data_send_8;
	__IO uint8_t *temp_get_adr = p->data_get_8;

	if (p->SPIx_hspi->Init.Mode == SPI_MODE_MASTER && p->CS_reset_allowed) SPI_EN;
	if (p->HAL_Delay_Mode) Delay_mks(p->HAL_Delay_ms_2);
	#endif
	if (!SPI_Params.delay_packet_mode)
	{
		if (!p->SPIx_init_packet || !p->SPIx_preamble_allowed)
		{
			if (p->SPIx_DataSize_prev == SPI_DATASIZE_8BIT)
			{
				HAL_SPI_TransmitReceive_IT(p->SPIx_hspi, (uint8_t*)p->data_send_8, (uint8_t*)p->data_get_8, p->Num_packets_prev);
			}
			else if (p->SPIx_DataSize_prev == SPI_DATASIZE_16BIT)
			{		
				HAL_SPI_TransmitReceive_IT(p->SPIx_hspi, (uint8_t*)p->data_send_16, (uint8_t*)p->data_get_16, p->Num_packets_prev);
			}
		} 
		else
		{
			#ifdef SPI_MASTER
			p->SPIx_init_packet = false;
			#endif
			
			if (p->SPIx_preamble_bites == 8)
			{
				HAL_SPI_TransmitReceive_IT(p->SPIx_hspi, (uint8_t*)p->data_send_init_8, (uint8_t*)p->data_get_8, p->Num_packets_preamb);
			}
			else if (p->SPIx_preamble_bites == 16)
			{		
				HAL_SPI_TransmitReceive_IT(p->SPIx_hspi, (uint8_t*)p->data_send_init_16, (uint8_t*)p->data_get_16, p->Num_packets_preamb);
			}
		}
	}
	#ifdef SPI_MASTER
	else 
	{
		for (uint8_t i = 0; i < p->Num_packets_prev; i++)
		{
			HAL_SPI_TransmitReceive_IT(p->SPIx_hspi, (uint8_t*)temp_send_adr, (uint8_t*)temp_get_adr, 1);
			if (!p->HAL_Delay_Mode) while ( HAL_SPI_GetState(p->SPIx_hspi) != HAL_SPI_STATE_READY);
			temp_send_adr++;
			temp_get_adr++;
		}
	}	
	
	if (p->SPIx_hspi->Init.Mode == SPI_MODE_MASTER )
	{
		if (!SPI_Params.delay_packet_mode)
		{
			if (!p->HAL_Delay_Mode) while ( HAL_SPI_GetState(p->SPIx_hspi) != HAL_SPI_STATE_READY);
		}
		if (p->HAL_Delay_Mode) Delay_mks(p->HAL_Delay_ms_1);
		if (p->CS_reset_allowed) SPI_DIS;
	}
	#endif
}
*/

/*
void BD_SPI_Send_Data (SPIx_Params_T *p)
{
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
	
	HAL_SPI_Transmit_DMA(p->hspi_x, (uint8_t*)p->data_send_8, p->Num_packets_prev);
}
*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
