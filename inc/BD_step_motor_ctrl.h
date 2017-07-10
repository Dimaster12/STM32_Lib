/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __step_motor_ctrl_H_
#define __step_motor_ctrl_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"
#include "BD_general.h"
#include "tim.h"

typedef enum
{
	DIRECT							= 0,
	INVERSE							= 1,
} POS_DIRECTION_CONST_T;

typedef enum
{
	AB_BC_MODE					= 0,
	A_AB_B_MODE					= 1,
	A_B_MODE						= 2,	
} STEP_MODE_CONST_T;

typedef enum
{
//	MOT_STATE_OPENING			= 0,
//	MOT_STATE_CLOSING			= 1,
	MOT_STATE_MOVING			= 0,
	MOT_STATE_STOPPPED		= 2,
} MOT_STATE_CONST_T;

typedef enum
{
	MOT_CMD_STOP					= 1,
	MOT_CMD_MOVE					= 2,
} MOT_COMANDS_CONST_T;

typedef struct
{
	GPIO_TypeDef* GPIO_Port;
	uint16_t			GPIO_Pin;
} Phase_x_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint8_t										phase0;														// Previous phase
	uint8_t										phase1;														// During phase
	MOT_COMANDS_CONST_T				mot_CMD;	
	MOT_STATE_CONST_T					mot_State;
	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	STEP_MODE_CONST_T					step_mode;
	POS_DIRECTION_CONST_T			rotating_reverse;									// REVERSE MOVING
	Phase_x_T									Phase_x[4];
	
	// Changing params (must be inited, may be reinited by developer):
	bool											position_calc;
	POS_DIRECTION_CONST_T			direction;												// POSITION CALC DIRECTION

	// Logic inited params (mustn`t be inited, reinited):
	int32_t										during_position;
} Step_Motor_x_Ctrl_T;

void BD_Step_Motor_x_Phase_ctrl ( Step_Motor_x_Ctrl_T *p );
void BD_Step_Motor_x_Stop ( Step_Motor_x_Ctrl_T *p );
void BD_Step_Motor_x_Logical_Init ( Step_Motor_x_Ctrl_T *p );

typedef enum
{
	PROC_STATE_FREE						= 0,
	PROC_STATE_OPENING				= 1,
	PROC_STATE_CLOSING				= 2,
	PROC_STATE_OPENED					= 3,
	PROC_STATE_CLOSED					= 4,
} PROCESS_STATE_CONST_T;

typedef enum
{
	PROC_CMD_FREE			= 0,
	PROC_CMD_STOP			= 1,
	PROC_CMD_OPEN			= 2,
	PROC_CMD_CLOSE		= 3,
} PROC_COMANDS_CONST_T;

typedef enum
{
	DRIVE_1_SELECT		= 1,
	DRIVE_2_SELECT		= 2,
	DRIVE_1_2_SELECT	= 3,
} DRIVE_CHOICE_CONST_T;

typedef enum
{
	MODE_OPERATION		= 0,
	MODE_SETTING			= 1,
	MODE_WRITING			= 2,
} PROC_MODE_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	PROC_COMANDS_CONST_T			proc_CMD;
	PROCESS_STATE_CONST_T			proc_State;
	PROC_MODE_T								proc_Mode;
	DRIVE_CHOICE_CONST_T			drive_Choice;
	
	bool											mode_operation_init;
	bool											mode_setting_init;
	bool											mode_writing_init;	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	Step_Motor_x_Ctrl_T				Motor_x[1];
	
	// Changing params (must be inited, may be reinited by developer):
	
	// Logic inited params (mustn`t be inited, reinited):

} Process_Ctrl_T;

//MOTOR PHASES
#define pAon(a)						HAL_GPIO_WritePin(a[0].GPIO_Port, a[0].GPIO_Pin, GPIO_PIN_SET)
#define pAoff(a)					HAL_GPIO_WritePin(a[0].GPIO_Port, a[0].GPIO_Pin, GPIO_PIN_RESET)
	 
#define pBon(a)						HAL_GPIO_WritePin(a[1].GPIO_Port, a[1].GPIO_Pin, GPIO_PIN_SET)
#define pBoff(a)					HAL_GPIO_WritePin(a[1].GPIO_Port, a[1].GPIO_Pin, GPIO_PIN_RESET)

#define pCon(a)						HAL_GPIO_WritePin(a[2].GPIO_Port, a[2].GPIO_Pin, GPIO_PIN_SET)
#define pCoff(a)					HAL_GPIO_WritePin(a[2].GPIO_Port, a[2].GPIO_Pin, GPIO_PIN_RESET)

#define pDon(a)						HAL_GPIO_WritePin(a[3].GPIO_Port, a[3].GPIO_Pin, GPIO_PIN_SET)
#define pDoff(a)					HAL_GPIO_WritePin(a[3].GPIO_Port, a[3].GPIO_Pin, GPIO_PIN_RESET)

#ifdef __cplusplus
}
#endif
#endif
