#ifndef __BD_dac
#define __BD_dac
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "BD_tim.h"

#define DAC_CHANNELS					1
#define DAC_ARRAY_SIZE				1000

typedef struct 
{
	float							Zero_level;									// [Volt]
	float							Ampl_level;									// [Volt]
	int16_t						Zero_angle;									// [deg]
} Reg_Params_T;	 

typedef struct 
{
	// Function inited (must be inited by special function ones):
	Reg_Params_T			Reg_Params_prev;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	uint32_t					Channel;
	// Changing params (must be inited, may be reinited by developer):
	Reg_Params_T			Reg_Params;
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t					DAC_array[DAC_ARRAY_SIZE];
	uint16_t					during_Array_element;
	uint16_t					Zero_angle_points;
} DAC_Channel_Params_T;

typedef struct 
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	TIMx_Params_T								*TIMx;
	DAC_Channel_Params_T				DAC_Channels[DAC_CHANNELS];
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} DAC_Params_T;

#define 	MAX_VOLT			3.29F

extern DAC_Params_T DAC_Params;

void BD_DAC_sin_array_calc (DAC_Channel_Params_T *p);
void BD_DAC_Logical (void);
void BD_DAC_angle_reset (void);
void BD_DAC_signals_generate (void);
void BD_DAC_Init_Logical (void);
void BD_DAC_Num_Points_Calc(void);
void BD_DAC_Init_Calc (void);

#ifdef __cplusplus
}
#endif
#endif
