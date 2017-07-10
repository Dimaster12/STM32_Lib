#ifndef __modbus_H_dim
#define __modbus_H_dim
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "BD_main_USER.h"
#include "BD_g_Ram_USER.h"	 
#include "BD_general.h"
#include "BD_tim.h"
#include "BD_usart.h"	 
#include "BD_crc.h"	
#include "BD_delay.h"

#define RS485_MODBUS_MODE						0
#define BT_MODBUS_MODE							1
#define USB_MODBUS_MODE							2 

#define MAX_RX_DATA_SIZE						20			// Количество отправляемых байт
#define MAX_TX_DATA_SIZE						20			// Количество принимаемых байт

#define MAX_JOURNAL_BUFFER					20			// Соответствует количеству 16-ти битных параметров в журнале

#define CMD_READ_DATA								3
#define CMD_WRITE_DATA							16
#define CMD_NOT_SET									0

#define PARAM_FOUND_OK							0
#define PARAM_NOT_FOUND_OK					1

#define GOOD_DATA										0
#define ILLEGAL_DATA								1	

typedef enum 
{
	Data_waiting		= 0,
	Receiving				= 1,
	Transmiting			= 2,
	Data_get_OK			= 3,
	Data_get_ERR		= 4	
} Rx_Status_T;

typedef struct 
{
	uint8_t			ID;																					// Запрашиваемый номер устройства
	uint8_t			Command;																		// Команда (код функции)
	uint16_t		Data_addr;																	// Адрес данных
	uint16_t		Number_of_registers;												// Запрашиваемое/записываемое количество регистров
	uint8_t			Bytes;																			// Количество байт данных (при записи)
	uint16_t		Writing_Data[MAX_RX_DATA_SIZE];							// Записываемые данные (при записи)
	uint16_t		CRC_Res;																		// Контрольная сумма
} Modbus_Rx_T;									

typedef struct
{
	uint8_t 		ID;																					// Запрашиваемый номер устройства
	uint8_t 		Command;																		// Команда (код функции)
//	uint8_t 		Bytes_num;																	// Количество возвращаемых байт
	uint16_t 		Reading_Data[MAX_TX_DATA_SIZE];							// Возвращаемые переменные
	uint16_t 		CRC_Res;																		// Контрольная сумма
} Modbus_Tx_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint8_t								Graphic_num;
	uint16_t							Gr_Buffer[2][MAX_GRAPH_BUFFER];
	bool									Monitor_flag;
	uint16_t							Point_offset;
	uint16_t							Buffer_addr;
	uint16_t							During_Addr;
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	uint16_t*							RefreshBuffer_CMD;
	uint16_t*							SetScreen_CMD;
	uint16_t*							SetPeriod_CMD;
	uint16_t							Addr_CH1;
	uint16_t							Addr_CH2;
	uint16_t							Buffer_size;	
	// Changing params (must be inited, may be reinited by developer):	
	// Logic inited params (mustn`t be inited, reinited):	
} Graphic_Buffer_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
  __IO uint32_t*				Byte_timer;										// Регистр, считающий принятые байты

	uint32_t							Byte_timer_prev;							// Предыдущее количество принятых байт	
	uint16_t							Addr;
	uint16_t							reset_Packet_timer;
	uint32_t							mistake_Timer;
	uint32_t							usb_Timer;
	
	// Rx{
	bool									ID_Mistake;										// Несовпадение ID
	bool									Unknown_Command;							// Неизвестаная команда
	bool									CRC_Mistake;									// Несовпадение CRC
	Rx_Status_T						Status;												// Статус приема

	Modbus_Rx_T						Rx_Params;							// Принятые данные
	//}
	
	// Tx{
	Modbus_Tx_T						Tx_Params;							// Принятые данные
	//}
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	uint16_t							ID_Device;										// ID устройства

	uint8_t*							ModbusRxBuffer;
	uint8_t*							ModbusTxBuffer;
	
	uint8_t								Modbus_Mode;
	Graphic_Buffer_T			Graphic;
	bool*									Par_searching_flag;
	Var_T**								Var_pointer;
	// Changing params (must be inited, may be reinited by developer):
	uint32_t							Reset_Timeout;
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t								i;
	uint8_t								k;
} Modbus_Params_T;

/*********************************************************/

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	int8_t											*Min;
	int8_t											*Max;
	int8_t											*Value;
	uint8_t											Mode;
	int16_t											Wr_Data;	
} Var_Modbus_8_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t											*Min;
	uint8_t											*Max;
	uint8_t											*Value;
	uint8_t											Mode;
	uint16_t										Wr_Data;	
} Var_Modbus_U8_T;	

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	int16_t											*Min;
	int16_t											*Max;
	int16_t											*Value;
	uint8_t											Mode;
	int16_t											Wr_Data;
} Var_Modbus_16_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t										*Min;
	uint16_t										*Max;
	uint16_t										*Value;
	uint8_t											Mode;
	uint16_t										Wr_Data;
} Var_Modbus_U16_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	int32_t											*Min;
	int32_t											*Max;
	int32_t											*Value;
	uint8_t												Mode;
	int32_t											Wr_Data;	
} Var_Modbus_32_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint32_t										*Min;
	uint32_t										*Max;
	uint32_t										*Value;
	uint8_t											Mode;
	uint32_t										Wr_Data;
} Var_Modbus_U32_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	float												*Min;
	float												*Max;
	float												*Value;
	uint8_t											Mode;
	float												Wr_Data;
	uint16_t										Sign_Symbols;	
} Var_Modbus_FL_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	fix16_t											*Min;
	fix16_t											*Max;
	fix16_t											*Value;
	uint8_t											Mode;
	fix16_t											Wr_Data;
	uint16_t										Sign_Symbols;	
} Var_Modbus_Q16_T;

Rx_Status_T BD_Modbus_Data_Rx ( Modbus_Params_T *p );
void BD_Modbus_Data_Tx (Modbus_Params_T *p);

#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	void BD_Modbus_Init(Modbus_Params_T *p, USARTx_Params_T *USART);
#elif defined(USB_MODBUS)
	void BD_Modbus_Init(Modbus_Params_T *p);
#endif

void BD_Modbus_Timeout_Reset ( Modbus_Params_T *p, TIM_Calc_Params_T *m, TIMx_Params_T *v );
bool BD_Modbus_Var_Search(Modbus_Params_T *p);
bool BD_Modbus_Chack_Limits (Modbus_Params_T *p);
void BD_Modbus_Graphics_Send (Modbus_Params_T *p);
//void BD_Journal_Chack (Modbus_Params_T *p, Journal_T *journal);

#if defined (FLASH_USED)
void BD_Menu_Modbus ( uint8_t Rx_data, Modbus_Params_T *p, Flash_T *m );
void BD_Modbus_Size_Convert ( Modbus_Params_T *p, uint16_t Addr, Flash_T *flash );
#else
void BD_Menu_Modbus ( Modbus_Params_T *p );
void BD_Modbus_Size_Convert ( Modbus_Params_T *p, uint16_t Addr );
#endif
void BD_MODBUS_ClearGettedData(Modbus_Params_T *p, uint8_t Num_packets_get_prev);

#ifdef __cplusplus
}
#endif
#endif
