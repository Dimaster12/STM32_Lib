/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __tim_H_dim
#define __tim_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_main_USER.h"	 
#include "BD_general.h"	 
	 
// TIM_Calc_Params_T Mode
#define	LIMIT_TIME_CALC										0
#define	FREQ_CALC													1

// Reinit_allowed Commands
#define TIME_CALC_ACTIVE									(bool)0
#define VIEW_RESULT												(bool)1

#define WAITING_INIT_TIME									(bool)1

#define GO_UP															0
#define GO_DOWN														1

#define CALC_MODE_1												0
#define CALC_MODE_2												1

typedef enum 
{
	UNUSED_SET														= 0,
	FREQ_SET															= 1,		// Расчет частоты примерный по логарифмам
	DAC_SINUS															= 2,		// Расчет точек для ЦАП	
} TIMx_Params_Calc_Mode_T;

typedef union{
	uint8_t all;
	struct {
		uint8_t Int_Allowed:1;						// 0			Прерывание активно
		uint8_t Hand_Calc_Params:1;				// 1			Ручной расчет параметров
		uint8_t rsvd:6;										// 2-7		
	} bit;
} TIMx_Settings_T;

#define	DIRECT_PWM_MODE										1
#define	INVERSE_PWM_MODE									2
#define	BOTH_PWM_MODE											3

typedef union{
	uint8_t all;
	struct {
		uint8_t DIRECT_CH:1;							// 0			Прямой
		uint8_t INVERSE_CH:1;							// 1			Инверсный
		uint8_t rsvd:6;										// 2-7		
	} bit;
} PWM_CHx_MODE_T;

// Params for TIMx function
typedef struct 
{
	float									Freq;
	uint32_t							Prescaler;
	uint32_t							Period;
	uint16_t							Num_Points;
} TIMx_Reg_Params_T;

typedef struct 
{
	float									Freq_PWM;
	float									Freq_SIN;
	float									Ampl_SIN;							// %

	uint32_t							Prescaler;
	uint32_t							Period;
} PWMx_Reg_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool											TIMx_init_was;	
	bool											TIMx_en_was;
	bool											TIMx_dis_was;
	uint32_t									TIMx_function_break;
	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*				htim_x;																// Structer of TIMx	
	void											(*TIMx_Init_func)(void);							// TIMx init function	
	TIMx_Params_Calc_Mode_T		Calc_Mode;
	TIMx_Settings_T						Settings;
	uint32_t									APBx_Freq;
	
	// Changing params (must be inited, may be reinited by developer):
	bool											TIMx_en;															//Allowes TIMx to be enabled	
	TIMx_Reg_Params_T					TIMx_Reg_Params;

	// Logic inited params (mustn`t be inited, reinited):
	uint32_t									TIMx_dt;															// Microseconds
	float											T_samp_FLOAT;													// Period float
	fix16_t										T_samp_Q16;														// Period	Q16
	TIMx_Reg_Params_T					TIMx_Reg_Params_prev;
} TIMx_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									TIMx_init_was;	
	bool									TIMx_en_was;
	bool									TIMx_dis_was;
#ifdef PWM_SIN	
	bool									Point_timer_reset_flag;
//	bool									SIN_not_inited;		
#endif	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*		htim_x;																// Structer of TIMx	
	void									(*TIMx_Init_func)(void);							// TIMx init function
	uint32_t							APBx_Freq;
	// Changing params (must be inited, may be reinited by developer):
	bool									TIMx_en;															//Allowes TIMx to be enabled
	PWMx_Reg_Params_T			TIMx_Reg_Params;
	// Logic inited params (mustn`t be inited, reinited):
	PWMx_Reg_Params_T			TIMx_Reg_Params_prev;
	float									T_samp_FLOAT;													// Period float
	fix16_t								T_samp_Q16;														// Period	Q16	
#ifdef PWM_SIN
	uint16_t							Num_Points;
	uint16_t							SIN_array[PWM_SIN_ARRAY];
#endif	
} TIMx_PWM_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									TIMx_init_was;	
	bool									TIMx_en_was;
	bool									TIMx_dis_was;	
	uint16_t							Point_timer;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*		htim_x;																// Structer of TIMx	
	uint32_t							Channel_x;
	PWM_CHx_MODE_T				Mode;
//  __IO uint32_t					CCR_x_readen;
	// Changing params (must be inited, may be reinited by developer):
	bool									TIMx_en;															//Allowes TIMx to be enabled
  __IO uint32_t					CCR_x;
	uint16_t							Shift_deg;	
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t							Shift_Point;	
} PWM_CHx_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint32_t							Init_CNT_value;
	uint32_t							Result_time_us;				// Microseconds
	bool									State;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
} TIM_Calc_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									State;
	uint32_t							Init_CNT_value;
	float									Result_freq;

	// Ones setting params (mustn`t be changed by developer aftet first initing):	
} Freq_Calc_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									First_Calc;														// 	
	float									Calc_Speed;														//
	int32_t								Calc_Rev;															//		
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*		htim_x;																// Structer of Encoder
	void									(*TIMx_Init_func)(void);							// TIMx init function	
	float*								Freq;	
	// Changing params (must be inited, may be reinited by developer):
	uint32_t							ICFilter;	
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t							CNT1;																	// 
	uint16_t							CNT0;																	//

	bool									DIR1;																	// 
	bool									DIR0;																	//

	bool									Calc_Mode;														//
	bool									prev_State;														//
	
	uint16_t							pos_Timer;														//	
	uint32_t							Time;																	//

	uint32_t							ICFilter_prev;	
} TIMx_Encoder_Params_T;

void BD_TIM_ReInit(TIMx_Params_T *p);
void BD_TIM_Logical (TIMx_Params_T *p);
void BD_TIM_Logical_params_init (TIMx_Params_T *p);
void BD_TIMx_Params_Calc (TIMx_Reg_Params_T *p, uint32_t APBx_Freq);
void BD_TIM_Init_Callback(TIMx_Params_T *p);

// Freq Calc
void BD_TIM_Freq_Calc_Init (Freq_Calc_Params_T *p);
void BD_TIM_Freq_Calc (Freq_Calc_Params_T *p, TIMx_Params_T *v);

// Time Calc
void BD_TIM_Time_Calc_Init (TIM_Calc_Params_T *p);
void BD_TIM_Time_Calc (TIM_Calc_Params_T *p, TIMx_Params_T *v, bool Command);
void BD_TIM_Delay (TIM_Calc_Params_T *p, TIMx_Params_T *m , uint32_t delay_time);

// Encoder
void BD_TIM_Encoder_Init_params(TIMx_Encoder_Params_T *p);
void BD_TIM_Encoder_Speed_calc(TIMx_Encoder_Params_T *p);
void BD_TIM_Encoder_Logical(TIMx_Encoder_Params_T *p);

//PWM
void BD_TIMx_PWM_Params_Calc(PWMx_Reg_Params_T *p, uint32_t APBx_Freq);
void BD_PWMx_SIN_Params_Calc(TIMx_PWM_Params_T *p);

void BD_TIMx_PWM_Logical(TIMx_PWM_Params_T *p);
void BD_PWM_CHx_Logical(PWM_CHx_Params_T *p);

void BD_TIMx_PWM_Logical_params_init (TIMx_PWM_Params_T *p);
void BD_PWM_CHx_Logical_params_init (PWM_CHx_Params_T *p);

#ifdef __cplusplus
}
#endif
#endif
