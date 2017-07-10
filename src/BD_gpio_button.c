#include "BD_gpio_button.h"

bool button_pushed (GPIO_PinState pin_state, Button_struct_T *p)
{
	if ( !p->push_allowed )
	{
		p->dur_state = (bool)BUT_NON_PUSHED;
		
		if ( p->reset_state_timer < (uint16_t)ceil(p->period_non_push*(*p->inter_freq)) ) p->reset_state_timer++;
		else
		{
			p->reset_state_timer = 0;
			p->main_timer = 0;
			p->push_allowed = true;
		}
	}
	
	if ( pin_state == BUT_PUSHED && p->push_allowed )
	{
		if ( p->main_timer < (uint16_t)ceil(p->period_push*(*p->inter_freq)) )
		{
			p->main_timer++;
			p->dur_state = (bool)BUT_NON_PUSHED;			
		}
		else if (p->result_mode == PUSH_RESULT_MODE)
		{
			p->main_timer = 0;
			p->push_allowed = false;
			p->dur_state = (bool)BUT_PUSHED;
		}
	}
	else
	{
		if ( p->push_allowed && p->main_timer >= (uint16_t)ceil(p->period_push*(*p->inter_freq)) && p->result_mode == NON_PUSH_RESULT_MODE)
		{			
			p->push_allowed = false;
			p->dur_state = (bool)BUT_PUSHED;
			p->reset_state_timer = 0;
		}
		else p->dur_state = (bool)BUT_NON_PUSHED;
		
		p->main_timer = 0;		
	}
	return p->dur_state;
}

void button_reset (Button_struct_T *p)
{
	p->push_allowed = true;
	p->dur_state = (bool)BUT_NON_PUSHED;
	p->main_timer = 0;
	p->reset_state_timer = 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
