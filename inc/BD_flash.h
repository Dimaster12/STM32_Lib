#ifndef __flash_H_dim
#define __flash_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"	 
#include "BD_general.h"

typedef enum
{
	// Erase
	ERASING							= 0,
	ERASED							= 1,	

	// Param fixing
	PAR_WRITTING				= 2,
	PAR_WRITTEN					= 3,
	// Journal fixing
	JOURN_WRITTING			= 4,
	JOURN_WRITTEN				= 5,

	// Commands allowed
	FREE								= 10,
} Flash_State_T;

typedef struct
{
	// Function inited (must be inited by special function ones):	
	bool												Rewrite_flag;
	uint16_t										Erase_timer;
	uint16_t										Flash_write_timer;

	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	uint16_t										Modbus_End_Addr;	
	// Logic inited params (mustn`t be inited, reinited):
// Writing Journal
	uint32_t										Writing_Addr;
	uint8_t											Page_Num;
	
// Reading Journal
	uint32_t										Reading_Addr;	
} Journal_T;


typedef struct
{
	// Function inited (must be inited by special function ones):
	Flash_State_T								State;
	uint16_t										Array_Addr;	
	
	// Par var
	bool												Par_Rewrite_flag;
	uint16_t										Par_Erase_timer;
	uint16_t										Par_Flash_write_timer;
	
	Journal_T										Journal;
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Flash_T;

void BD_Menu_Flash_Update ( Flash_T *p );
void BD_Journal_Chack_Page_Limits(void);
void BD_Menu_Flash_Write_IT ( Flash_T *p );
void BD_Flash_Erase_IT (uint32_t Sector_Addr, Flash_T *p);
void BD_Menu_Journal_Write_IT ( Flash_T *p );
void BD_Flash_Init_Params (Flash_T *p);
void BD_Menu_Read_Params (Flash_T *p);

extern Flash_T Flash_Params;

#ifdef __cplusplus
}
#endif
#endif
