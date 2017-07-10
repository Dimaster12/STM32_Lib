#ifndef __one_wire_H_dim
#define __one_wire_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

/****************************************************************************
 ROM commands
****************************************************************************/
#define     OWI_ROM_READ    0x33    //!< READ ROM command code.
#define     OWI_ROM_SKIP    0xCC    //!< SKIP ROM command code.
#define     OWI_ROM_MATCH   0x55    //!< MATCH ROM command code.
#define     OWI_ROM_SEARCH  0xF0    //!< SEARCH ROM command code.


/****************************************************************************
 Return codes
****************************************************************************/
#define DS18B20_FAMILY_ID                0x28 
#define DS18B20_CONVERT_T                0x44
#define DS18B20_READ_SCRATCHPAD          0xbe
#define DS18B20_WRITE_SCRATCHPAD         0x4e
#define DS18B20_COPY_SCRATCHPAD          0x48
#define DS18B20_RECALL_E                 0xb8
#define DS18B20_READ_POWER_SUPPLY        0xb4

#include "BD_main_USER.h"

typedef enum
{
	OW_ROM_SEARCH_FINISHED				= 0,
	OW_ROM_SEARCH_FAILED					= 1,
	OW_ROM_SEARCHING							= 2,
	OW_ROM_SEARCH_CRC_ERROR				= 3,	

	OW_DEVICES_PRESENSES					= 4,
	OW_DEVICES_NOT_PRESENSES			= 5,

	OW_WAIT_TEMP									= 6,
	OW_TEMP_CALCULATED						= 7,

	OW_ALL_SENSORS_FOUND					= 8,
	OW_NOT_ALL_SENSORS_FOUND			= 9	
	
	/*
	OW_ROM_SENSOR_X_FOUND					= 2,
	OW_ROM_SEARCH_CRC_ERROR				= 3,

	OW_DEVICES_FOUND							= 4,
	
	OW_NO_DEVICES									= 5,
	OW_NOT_ALL_SENSORS						= 6,
	*/
	
/*
	OW_OK					= 0,
	OW_DEVICES_FOUND		= 1,
	OW_NO_DEVICES			= 2,
	OW_SEND_MISTAKE			= 3,
	OW_GET_MISTAKE			= 4,
	OW_TEMP_CALCULATED			= 5,
	OW_WAIT_TEMP			= 6,
	OWI_CRC_OK = 7,
	OWI_CRC_ERROR = 8,
	OWI_DEVICE_SEARCHING = 9,
	OWI_DEVICE_STOPPED = 10
*/
	
} OW_CONST_T;

typedef enum 
{
/*
	OW_ROM_SEARCH_FINISHED				= 0,
	OW_ROM_SEARCH_FAILED					= 1,
	OW_CRC_ERROR									= 2
*/	

	OW_DATA_GET										= 4,


	OW_OK					= 0,
	OW_DEVICE_FOUND		= 1,
	OW_NO_DEVICE			= 2,
	OW_SEND_MISTAKE			= 3,
	OW_GET_MISTAKE			= 4,
//	OW_WAIT_TEMP			= 5,
	OW_CRC_OK = 6,
	OW_CRC_ERROR = 7,
	OW_DEVICE_SEARCHING = 8
	
} OW_DeviceX_CONST_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint8_t							id[8];														//!< The 64 bit identifier
	OW_DeviceX_CONST_T	Status;
	
	uint8_t							int_tempr;
	uint8_t							fract_tempr;
	float								tempr;
	
	uint8_t							CRC_res;
	uint32_t						CRC_mistake_timer;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} OW_DeviceX_Params_T;

#if MAX_DEVICES > 1
typedef struct
{
	// Function inited (must be inited by special function ones):
	uint8_t*						newID;
	uint8_t*						currentID;
	uint8_t							lastDeviation;
	uint8_t							numFoundDevices;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t							currentBit;
	uint8_t							newDeviation;
	uint8_t							bitMask;
} OW_Searching_Params_T;
#endif

typedef struct
{
	// Function inited (must be inited by special function ones):
	OW_CONST_T								OW_Status;	
	
#if MAX_DEVICES > 1
	OW_Searching_Params_T			Search_var;
#endif	
	uint8_t										operation_func;
	uint8_t										during_readen_byte;
	uint8_t										during_written_byte;	
	uint8_t										readen_array[9];

	uint8_t										data_send_8;
	uint8_t										data_get_8;
	uint8_t										num;	

	uint32_t									Presence_mistake_calc;
	uint32_t									Presence_success_calc;
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	USARTx_Params_T*					USARTx;
	// Changing params (must be inited, may be reinited by developer):
	OW_DeviceX_Params_T*			Sensor[MAX_DEVICES];
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t*									tmp_adr;
} OW_Params_T;

void BD_OW_Tx(OW_Params_T *p);
void BD_OW_Rx(OW_Params_T *p);

void BD_OW_Init_Tx(OW_Params_T *p);
void BD_OW_Init_Rx(OW_Params_T *p);

void BD_OWI_SearchRom_Tx(OW_Params_T *p);
void BD_OWI_SearchRom_Rx(OW_Params_T *p);

void BD_OW_Detect_Presence_Tx(OW_Params_T *p);
void BD_OW_Detect_Presence_Rx(OW_Params_T *p);

void BD_OWI_Command (OW_Params_T *p, uint8_t data);

void BD_OWI_Receive_Data_Tx(OW_Params_T *p);
void BD_OWI_Receive_Data_Rx(OW_Params_T *p);

void BD_OW_Params (OW_Params_T *p);
void BD_OW_Zero_Params (OW_Params_T *p);
void BD_OW_DeviceX_Params (OW_DeviceX_Params_T *p);
void BD_OW_Searching_Params (OW_Params_T *p);
#if MAX_DEVICES > 1
void BD_OW_New_Sensor_Zero (OW_Searching_Params_T *p);
#endif
void BD_OW_WAIT_TEMP_GENERATION_Tx (OW_Params_T *p);
void BD_OW_WAIT_TEMP_GENERATION_Rx (OW_Params_T *p);

void BD_OWI_Read_Two_Bits_Tx(OW_Params_T *p);
void BD_OWI_Read_Two_Bits_Rx(OW_Params_T *p);

void BD_OW_TEMP_CALC (OW_Params_T *p);
void OWI_Check_CRC( OW_Params_T *p, uint8_t *chacking_array, uint8_t lenth);
uint8_t OWI_ComputeCRC8(uint8_t inData, uint8_t seed);

void BD_Write_Bit (OW_Params_T *p, bool bit);

extern void (*OW_Work_Tx_func)(OW_Params_T *p);
extern void (*OW_Work_Rx_func)(OW_Params_T *p);

void BD_ID_Chack(OW_Params_T *p);
void BD_OW_Zero_Func (OW_Params_T *p);

void BD_OW_MatchRom_Tx(OW_Params_T *p);
void BD_OW_MatchRom_Rx(OW_Params_T *p);

#ifdef __cplusplus
}
#endif
#endif
