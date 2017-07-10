/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BD_adc
#define __BD_adc
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"
#include "BD_general.h"
	 
#define PLUS 1
#define MINUS 0

extern volatile uint16_t		ADC_Channel_res[ADC_CHANNELS_NUM];

#if defined(FLOAT_ADC)	 
/****************************************************/
typedef struct
{
	// Function inited (must be inited by special function ones):
	uint16_t							ADC_Channel_res_saved;
	float									Zero_harm_changed;
	
	float									Recalc_value;
	float									Scaled_value;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	// Changing params (must be inited, may be reinited by developer):
	float									Zero_harm;								// Zero harmonic Max = 4095 for 12 adc
	float									Scale_coeff;							// Scale coefficient	
	// Logic inited params (mustn`t be inited, reinited):
} Scale_Params_FLOAT_T;

void BD_ADC_Scale_Params_init_FLOAT (void);
void BD_ADC_Zero_Harm_Convertion_FLOAT (void);
void BD_ADC_Scale_Convertion_FLOAT (void);
extern Scale_Params_FLOAT_T				Scale_Params_FLOAT[ADC_CHANNELS_NUM];
/****/
#elif defined(Q16_ADC)
typedef struct
{
	// Function inited (must be inited by special function ones):
	uint16_t							ADC_Channel_res_saved;
	fix16_t								Zero_harm_changed;
	
	fix16_t								Recalc_value;
	fix16_t								Scaled_value;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	// Changing params (must be inited, may be reinited by developer):
	fix16_t								Zero_harm;								// Zero harmonic Max = 4095 for 12 adc
	fix16_t								Scale_coeff;							// Scale coefficient	
	// Logic inited params (mustn`t be inited, reinited):
} Scale_Params_Q16_T;

void BD_ADC_Scale_Params_init_Q16 (void);
void BD_ADC_Zero_Harm_Convertion_arg_Q16(uint8_t buff_shift, uint8_t sign_num);
void BD_ADC_Scale_Convertion_arg_Q16 (Scale_Params_Q16_T *p, uint8_t sign_num);
extern Scale_Params_Q16_T				Scale_Params_Q16[ADC_CHANNELS_NUM];
/****************************************************/
#endif

/****************************************************/
typedef struct
{
	// Function inited (must be inited by special function ones):
	float									Oper_value;
	float									Sum_square;
	uint32_t							Points_timer;
	float									sin_Freq;
	uint32_t							timer_volt_zero;
	bool 									during_sign;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	float									*T_samp;	
	// Changing params (must be inited, may be reinited by developer):
	float									up_zero_lim;
	float									down_zero_lim;
	
	float									max_freq_value;
	float									reset_period;
	// Logic inited params (mustn`t be inited, reinited):
} Square_Params_FLOAT_T;

void BD_Square_calc_FLOAT (Square_Params_FLOAT_T *p, float during_signal);
void BD_Square_FLOAT_init (Square_Params_FLOAT_T *p);
/****************************************************/

/*
typedef struct
{
	// Function inited (must be inited by special function ones):	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	float									Fixed_level;				//
	bool 									Recalc_allowed;
	// Changing params (must be inited, may be reinited by developer):
	float									Recalc_zone;				//
	float									Oper_value;					//
	// Logic inited params (mustn`t be inited, reinited):
} Operating_Calcs_Params_T;
*/

#ifdef __cplusplus
}
#endif
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
