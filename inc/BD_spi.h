#ifndef __spi_H_dim
#define __spi_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"
#include "BD_general.h"

typedef enum 
{
	RX_EN					= 0,
	TX_EN					= 1,
	RX_TX_EN			= 2,
} DATA_MODE_CONST_T;	 
	 
typedef struct 
{
	uint32_t									DataSize;
	uint32_t									BaudRatePrescaler;
	uint32_t									CRCCalculation;
} SPIx_Reg_Params_T;

/*
typedef struct
{
	// Function inited (must be inited by special function ones):
	bool											SPIx_init_was;
	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	SPI_HandleTypeDef*				hspi_x;
	void											(*SPIx_Init_func)(void);

	// Changing params (must be inited, may be reinited by developer):
	SPIx_Reg_Params_T					Reg_Params;
	uint8_t										Num_packets;
	
	// Logic inited params (mustn`t be inited, reinited):
	SPIx_Reg_Params_T					Reg_Params_prev;
	uint8_t										Num_packets_prev;		
} SPIx_Params_T;
*/

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	SPI_HandleTypeDef*				hspi_x;
	
__IO 	uint8_t								*data_send;
__IO 	uint8_t								*data_get;
	DATA_MODE_CONST_T					data_mode;

	GPIO_TypeDef							*CS_GPIO_Port;	
	uint16_t									CS_GPIO_Pin;
	// Changing params (must be inited, may be reinited by developer):
	SPIx_Reg_Params_T					Reg_Params;
	uint8_t										Num_packets;
	// Logic inited params (mustn`t be inited, reinited):
} SPI_DEVICEx_Params_T;

void BD_SPI_DEVICEx_Logical(SPI_DEVICEx_Params_T *p);
void BD_SPI_Data(SPI_DEVICEx_Params_T *p);

/*
void BD_SPI_Logical (SPIx_Params_T *p);
void BD_SPI_Logical_Init_Params (SPIx_Params_T *p);
void BD_SPI_ReInit (SPIx_Params_T *p);
void BD_SPI_Send_Data (SPIx_Params_T *p);
*/

#ifdef __cplusplus
}
#endif
#endif
