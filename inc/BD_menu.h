#ifndef __menu_H_dim
#define __menu_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_general.h"
#include "BD_g_Ram_USER.h"
#include "gpio.h"
#include "tim.h"
#include "BD_num_string.h"	 

#define CMD_FREE		0
#define CMD_UP			1
#define CMD_DOWN		2
#define CMD_IN			3
#define CMD_OUT			4

typedef struct 
{
	// Function inited (must be inited by special function ones):
	uint8_t										Menu_Level;
	Menu_Object_T*						during_Object;
	Menu_Object_T*						start_Object;
	uint8_t										during_Calc;
	uint8_t										during_Size;	
	uint8_t										during_Command;
	uint16_t									during_bit_num;
	bool											identify_Object;
	bool											show_Object;
	bool											LCD_update;
	uint16_t									display_timer;
	uint16_t									reset_timer;	
	uint8_t										num_symbols;
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	float*										inter_freq;
	float											reset_limit;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	bool											during_Object_Type;
	uint8_t										during_Param_View;
	Menu_Object_T*						prev_Object[MAX_LEVEL];
	uint8_t										prev_Size[MAX_LEVEL];
	uint8_t										prev_Calc[MAX_LEVEL];	
	
	Menu_Folder_T*						during_Folder;
	Menu_Param_T*							during_Param[3];
	Param_DEC_T*							during_Param_DEC;
	Param_STR_T*							during_Param_STR;
	Param_COMB_T*							during_Param_COMB;
	Param_UNION_T*						during_Param_UNION;	
	
	bool											reset_flag;
	uint8_t										objects_num;
	bool											reset_init_flag;	
	
	Param_DEC_RES_T*					during_Param_DEC_RES;	
	Param_STR_RES_T*					during_Param_STR_RES;
	Param_COMB_RES_T*					during_Param_COMB_RES;
}	Menu_T;

extern Menu_T Menu_Params;

void BD_Menu_Button_View (Menu_T *p);
void BD_Menu_Change_Object (Menu_T *p);
void BD_Menu_Init_Params (Menu_T *p);
void BD_Menu_Update_Param ( Menu_T *p );
void BD_Menu_ReInit_Params ( Menu_T *p );
void BD_Menu_Reset ( Menu_T *p );
void BD_Menu_Update_Param_Reset ( Menu_T *p );

#ifdef __cplusplus
}
#endif
#endif
