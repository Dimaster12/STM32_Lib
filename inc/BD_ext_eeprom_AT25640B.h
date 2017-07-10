#ifndef __BD_ext_eeprom_AT25640B_H
#define __BD_ext_eeprom_AT25640B_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"
#include "BD_general.h"
#include "BD_g_Ram_USER.h"	 

//Команды
#define _WRSR_					0x01		// Запись статусного регистра
#define _WRITE_					0x02		// Запись массива байт
#define _READ_					0x03		// Чтение массива байт
#define _WRDI_					0x04		// Запрещение записи
#define _RDSR_					0x05		// Чтение статусного регистра
#define _WREN_					0x06		// Разрешение записи

// Биты статусного регистра
#define AT25XXX_RDY			BIT_0_MASK		// 0 - Ready
#define AT25XXX_WEN			BIT_1_MASK		// 0 - Write Enable
#define AT25XXX_BP0			BIT_2_MASK
#define AT25XXX_BP1			BIT_3_MASK
#define AT25XXX_WPEN		BIT_7_MASK

#define GOOD_MEMORY_DATA	1
#define BAD_MEMORY_DATA		0

#define EEPROM_READY			1
#define EEPROM_BUSY				0

typedef enum
{
	// Commands
	CMD_WRITING					= 0,
	CMD_WRITTEN					= 1,

	// Param fixing
	PAR_WRITTING				= 2,
	PAR_WRITTEN					= 3,

	// Param reading
	PAR_READING					= 4,
	PAR_READEN					= 5,
	
	// Status reading
	STAT_READING				= 6,
	STAT_READEN					= 7,	
	// Commands allowed
	FREE								= 10,

} EEPROM_State_T;

typedef union{
	uint8_t all;
	struct {
		uint8_t RDY_inv:1;				// 0
		uint8_t WEN:1;						// 1
		uint8_t BP0:1;						// 2
		uint8_t BP1:1;						// 3
		uint8_t rsvd:3;						// 4-6
		uint8_t WPEN:1;						// 7
	} bit;
} EEPROM_Status_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	EEPROM_State_T							State;
	EEPROM_Status_T							Status;
	
	uint16_t										Writing_Array_Addr;
	uint16_t										Reading_Array_Addr;

	bool												Par_writing_flag;
	bool												Par_reading_flag;
	bool												Stat_reading_flag;
	bool												Par_searching_flag;	
	Var_T*											Var_pointer;
	uint16_t										Par_EEPROM_write_timer;
	uint16_t										Par_EEPROM_read_timer;
	uint16_t										Stat_read_timer;	
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} EEPROM_T;

void BD_Memory_Update ( EEPROM_T *p );
void BD_EEPROM_Callback (SPI_HandleTypeDef *hspi, EEPROM_T *p);
bool BD_EEPROM_Var_Search ( EEPROM_T *p );
void BD_EEPROM_Write_Param_IT ( EEPROM_T *p );
void BD_EEPROM_Read_Param_IT (EEPROM_T *p);
void BD_EEPROM_Init_Params (EEPROM_T *p);
bool BD_EEPROM_Read_Params_Callback ( EEPROM_T *p );
void BD_EEPROM_CMD_Write_en_IT ( EEPROM_T *p );
void BD_EEPROM_Read_Status_IT ( EEPROM_T *p );
bool BD_EEPROM_Read_Stat_Callback ( EEPROM_T *p );

extern EEPROM_T EEPROM_Params;

#ifdef __cplusplus
}
#endif
#endif
