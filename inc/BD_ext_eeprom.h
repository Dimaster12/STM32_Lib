#ifndef __BD_ext_eeprom_H
#define __BD_ext_eeprom_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"
#include "BD_g_Ram_USER.h"

#include "spi.h"

#define MEM_PARAM_FOUND_OK			1
#define MEM_PARAM_NOT_FOUND 		0 
	 
#if defined(EXT_EEPROM_AT25640B)
	#include "BD_ext_eeprom_AT25640B.h"
#elif defined(EXT_EEPROM_AT25320B)
	//#include "BD_ext_eeprom_AT25320B.h"
#endif
	 
/*********************************************************/
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	int8_t											Readen_Value;
	int8_t											*Var_fixing_value;
	int8_t											*Min;
	int8_t											*Max;
	int8_t											*Factory;	
} Var_Memory_8_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t											Readen_Value;
	uint8_t											*Var_fixing_value;
	uint8_t											*Min;
	uint8_t											*Max;
	uint8_t											*Factory;	
} Var_Memory_U8_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	int16_t											Readen_Value;
	int16_t											*Var_fixing_value;
	int16_t											*Min;
	int16_t											*Max;
	int16_t											*Factory;	
} Var_Memory_16_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t										Readen_Value;
	uint16_t										*Var_fixing_value;
	uint16_t										*Min;
	uint16_t										*Max;
	uint16_t										*Factory;	
} Var_Memory_U16_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	int32_t											Readen_Value;
	int32_t											*Var_fixing_value;
	int32_t											*Min;
	int32_t											*Max;
	int32_t											*Factory;	
} Var_Memory_32_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	uint32_t										Readen_Value;
	uint32_t										*Var_fixing_value;
	uint32_t										*Min;
	uint32_t										*Max;
	uint32_t										*Factory;	
} Var_Memory_U32_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	float												Readen_Value;
	float												*Var_fixing_value;
	float												*Min;
	float												*Max;
	float												*Factory;	
} Var_Memory_FL_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	fix16_t											Readen_Value;
	fix16_t											*Var_fixing_value;
	fix16_t											*Min;
	fix16_t											*Max;
	fix16_t											*Factory;	
} Var_Memory_Q16_T;	 

#ifdef __cplusplus
}
#endif
#endif
