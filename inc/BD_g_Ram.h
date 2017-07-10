#ifndef __BD_g_Ram
#define __BD_g_Ram
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"
#include "BD_general.h"

typedef uint8_t U8;	 
	 
// Modbus modes
#define RO_M				BIT_0_MASK			// read only
#define WO_M				BIT_1_MASK			// write only (only unsigned variables)
#define RW_M				BIT_2_MASK			// read/write
#define MEM_FIX			BIT_3_MASK			// write with flash/eeprom fixing
#define GR_B_M			BIT_4_MASK			// graphic buffer mode

// Menu visible
#define VISIBLE			0
#define INVISIBLE		1

#define FIRST_LINE	0
#define SECOND_LINE	1

// Menu varaible mode
#define PARAM				0
#define FOLDER			1

// Var Size
#define SIZE_8			BIT_0_MASK
#define SIZE_16			BIT_1_MASK
#define SIZE_32			BIT_2_MASK
#define SIGN				BIT_3_MASK
//Float type
#define SIZE_FLOAT	BIT_4_MASK
#define POINT_0			BIT_5_MASK
#define POINT_1			BIT_6_MASK
#define POINT_2			BIT_7_MASK
#define POINT_3			BIT_8_MASK
#define POINT_4			BIT_9_MASK
//Fixed-point type
#define SIZE_Q16		BIT_10_MASK

// Param View
#define DEC					0
#define STR					1
#define COMB				2
#define UNION				3
/*********************************************************/
// VAR
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const uint8_t*							Min;
	const uint8_t*							Max;
	const uint8_t*							Value;
	const uint16_t							Size;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):	
} Var_T;

/*********************************************************/
// FLASH
typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;
	const uint8_t*							Factory;
	const uint16_t							Addr;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Memory_Var_T;

/*********************************************************/
// JOURNAL
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;
//	const uint16_t							Modbus_Addr;
//	const uint16_t							Flash_Addr;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Journal_Var_T;

/*********************************************************/
// MODBUS
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;
	const uint8_t								Mode;	
	// Changing params (must be inited, may be reinited by developer):
	uint16_t										Addr;
//	const uint8_t							Sign_Symbols;	
//	bool											Visible;
	// Logic inited params (mustn`t be inited, reinited):
} Modbus_Var_T;

/*********************************************************/
// MENU
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const uint8_t*							Pointer;
	const uint8_t								Type;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Menu_Object_T;

/**************************************/
// DEC{
typedef struct
{
	const uint8_t								Str[4];
} Units_T;

typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;	
	const uint8_t								Main_Str[16];
	const Units_T*							Units_Str;
	const uint8_t								Units_Pos;	
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_DEC_T;

typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;	
	const uint8_t								Str_Num;
	
	const Units_T*							Units_Str;
	const uint8_t								Units_Pos;
	const uint8_t								Var_Pos;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_DEC_RES_T;
// DEC}

/**************************************/
// STRING{
typedef struct
{
	uint8_t											Str[16];
} STR_T;

typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;
	const uint8_t								Main_Str[16];
	const STR_T*								Var_Str;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_STR_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;
	const uint8_t								Str_Num;

	const STR_T*								Var_Str;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_STR_RES_T;
// STRING}

/**************************************/

//COMB{
typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var_x;
	const uint8_t								Pos_x;	
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} COMB_T;

typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const COMB_T*								Var_array;
	const uint8_t								Main_Str[16];

	const uint8_t								Var_Number;
	const uint8_t								Second_Str[16];
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_COMB_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const COMB_T*								Var_array;
	const uint8_t								Str_Num;

	const uint8_t								Var_Number;
	const uint8_t								Second_Str[16];
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_COMB_RES_T;
//COMB}

/**************************************/

//UNION{
typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const Var_T*								Var;
	const uint8_t								Main_Str[16];
	const uint8_t								Bits_Num;
	const STR_T*								Var_Str;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Param_UNION_T;
//UNION}

/************************************************************************************/
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const uint8_t*							Pointer;
	const uint8_t								View;
	// Changing params (must be inited, may be reinited by developer):
	bool												Visible;
	// Logic inited params (mustn`t be inited, reinited):
} Menu_Param_T;

// Folders
typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	Menu_Object_T*							Content;
	const uint8_t								First_Str[16];
	const uint8_t								Second_Str[16];
	const uint8_t								Size;
	// Changing params (must be inited, may be reinited by developer):
	bool												Visible;
	// Logic inited params (mustn`t be inited, reinited):
} Menu_Folder_T;
/************************************************************************************/


#if defined (MODBUS_USED)
	extern Modbus_Var_T						g_Ram_modbus[];
	extern const uint8_t					g_Ram_modbus_Size;
#endif

#if defined (MEMORY_USED)
	extern Memory_Var_T						g_Ram_memory[];
	extern const uint8_t					g_Ram_memory_Size;
	#if defined (JOURNAL_USED)
		extern const Journal_Var_T		g_Ram_journal[];
		extern const uint8_t					g_Ram_journal_Size;
	#endif
#endif

#if defined (MENU_USED)
	extern const Menu_Object_T		g_Ram_menu[];	 
	extern const uint8_t					g_Ram_menu_Size;
#endif

#ifdef __cplusplus
}
#endif
#endif
