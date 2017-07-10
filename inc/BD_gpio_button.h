/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_button_H_dim
#define __gpio_button_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_general.h"
#include "math.h"
#include "tim.h"

// Defines:
#define BUT_PUSHED					(GPIO_PinState)0
#define BUT_NON_PUSHED				(GPIO_PinState)1

#define PUSH_RESULT_MODE					(bool)0
#define NON_PUSH_RESULT_MODE			(bool)1

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint16_t								main_timer;	
	uint16_t								state_timer;	
	uint16_t								reset_state_timer;
	bool										dur_state;
	bool										prev_state;
	bool										push_allowed;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	float										period_push;
	float										period_non_push;	
	uint16_t								but_pushed_perc;
	uint16_t		    				but_non_pushed_perc;
	bool										result_mode;
	float*									inter_freq;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} 
Button_struct_T;

bool button_pushed (GPIO_PinState pin_state, Button_struct_T *p);
void button_reset (Button_struct_T *p);

#ifdef __cplusplus
}
#endif
#endif
