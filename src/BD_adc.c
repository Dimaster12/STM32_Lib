/* Includes ------------------------------------------------------------------*/
#include "BD_adc.h"

volatile uint16_t					ADC_Channel_res[ADC_CHANNELS_NUM];

#if defined(FLOAT_ADC)
/********************************************/
Scale_Params_FLOAT_T			Scale_Params_FLOAT[ADC_CHANNELS_NUM];
//Init

void BD_ADC_Scale_Params_init_FLOAT (void)
{
	for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
	{
		Scale_Params_FLOAT[i].ADC_Channel_res_saved = 0;
		Scale_Params_FLOAT[i].Scaled_value = 0.0F;
	}
}

//ZERO HARMONIC
void BD_ADC_Zero_Harm_Convertion_FLOAT (void)
{
	for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
	{
		Scale_Params_FLOAT[i].ADC_Channel_res_saved = ADC_Channel_res[i];
		Scale_Params_FLOAT[i].Zero_harm_changed = (float)Scale_Params_FLOAT[i].ADC_Channel_res_saved;
		Scale_Params_FLOAT[i].Zero_harm_changed -= (float)Scale_Params_FLOAT[i].Zero_harm;
	}
}

//SCALE
void BD_ADC_Scale_Convertion_FLOAT (void)
{
	for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
	{	
		Scale_Params_FLOAT[i].Scaled_value = Scale_Params_FLOAT[i].Recalc_value;
		Scale_Params_FLOAT[i].Scaled_value *= Scale_Params_FLOAT[i].Scale_coeff;
	}
}
/*******/
#elif defined(Q16_ADC)
Scale_Params_Q16_T				Scale_Params_Q16[ADC_CHANNELS_NUM];

//Init
void BD_ADC_Scale_Params_init_Q16 (void)
{
	for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
	{
		Scale_Params_Q16[i].ADC_Channel_res_saved = 0;
		Scale_Params_Q16[i].Zero_harm_changed = F16(0.0);
		Scale_Params_Q16[i].Recalc_value = F16(0.0);
		Scale_Params_Q16[i].Scaled_value = F16(0.0);
	}
}

//ZERO HARMONIC
void BD_ADC_Zero_Harm_Convertion_arg_Q16(uint8_t buff_shift, uint8_t sign_num)
{
	for (uint8_t i=buff_shift; i < sign_num+buff_shift; i++)
	{
		Scale_Params_Q16[i].ADC_Channel_res_saved = ADC_Channel_res[i];
		Scale_Params_Q16[i].Zero_harm_changed = INTtoQ16(Scale_Params_Q16[i].ADC_Channel_res_saved);
		Scale_Params_Q16[i].Zero_harm_changed = Q16_SUB(Scale_Params_Q16[i].Zero_harm_changed,Scale_Params_Q16[i].Zero_harm);
	}
}

//SCALE
void BD_ADC_Scale_Convertion_arg_Q16 (Scale_Params_Q16_T *p, uint8_t sign_num)
{
	for (uint8_t i = 0; i < sign_num; i++)
	{	
		p[i].Scaled_value = p[i].Recalc_value;
		p[i].Scaled_value = Q16_MUL(p[i].Scaled_value,p[i].Scale_coeff);
	}
}
#endif

/********************************************/

void BD_Square_calc_FLOAT (Square_Params_FLOAT_T *p, float during_signal)
{
	if (during_signal > 0.01F && p->during_sign == MINUS)
	{
		//if ( 1.0F/((float)p->Points_timer*(*p->T_samp)*2.0F) < p->max_freq_value )
		//{
			p->Oper_value = p->Sum_square/p->Points_timer;
			//p->sin_Freq = 1.0F/((float)p->Points_timer*(*p->T_samp)*2.0F);
		//}
		p->Sum_square = 0;
		p->Points_timer = 0;
		p->during_sign = PLUS;
	}
	
	if (during_signal < -0.01F && p->during_sign != MINUS)
	{
		//if ( 1.0F/((float)p->Points_timer*(*p->T_samp)*2.0F) < p->max_freq_value )
		//{
			p->Oper_value = p->Sum_square/p->Points_timer;
			//p->sin_Freq = 1.0F/((float)p->Points_timer*(*p->T_samp)*2.0F);
		//}
		p->Sum_square = 0;
		p->Points_timer = 0;	
		p->during_sign = MINUS;
	}

	p->Sum_square += fabs(during_signal);
	p->Points_timer++;
	
	if (during_signal < p->up_zero_lim && during_signal > p->down_zero_lim) p->timer_volt_zero++;
	else p->timer_volt_zero = 0;
		
	if ( p->timer_volt_zero*(*p->T_samp) >= p->reset_period )
	{
		p->Oper_value = 0.0F;
		p->sin_Freq = 0;
	}
}

void BD_Square_FLOAT_init (Square_Params_FLOAT_T *p)
{
	p->Oper_value = 0.0F;
	p->Sum_square = 0.0F;
	p->Points_timer = 0;
	p->sin_Freq = 0.0F;
	p->timer_volt_zero = 0;
	p->during_sign = MINUS;
}
/*******/

/*
void Ampl_calc(Operating_Calcs_Params_T *p, float during_signal)
{
	float tmp_during_signal = fabs(during_signal);
	
	if ( p->Fixed_level > tmp_during_signal )
	{
		if ( (p->Fixed_level - tmp_during_signal > p->Recalc_zone) && p->Recalc_allowed )
		{
			p->Oper_value = p->Fixed_level/sqrt(2.0F);
			p->Recalc_allowed = false;
			//p->Fixed_level = 0;
		}
		//else p->Fixed_level = tmp_during_signal;
	}
	else
	{
		p->Fixed_level = tmp_during_signal;
		p->Recalc_allowed = true;
	}
	
	if (tmp_during_signal < 10.0F) p->Fixed_level = 0;
}
*/

// Calculation of maximum delta time and configuration of timer for digitization signals
void BD_ADC_PWM_Set_timer(PWM_CHx_Params_T *pwm_ch_x, TIMx_PWM_Params_T *pwm_timer, TIMx_Params_T *int_timer)
{
	uint32_t tmp_CCR_0,
					 tmp_CCR_1,
					 tmp_CCR_2,

					 delta_0, 
					 delta_1,
					 delta_2,
					 delta_3,

					 Period;

	/*
	if (pwm_ch_x[0].CCR_x < pwm_ch_x[1].CCR_x && pwm_ch_x[0].CCR_x < pwm_ch_x[2].CCR_x)
	{
		tmp_CCR_0 = pwm_ch_x[0].CCR_x;
		if (pwm_ch_x[1].CCR_x < pwm_ch_x[2].CCR_x)
		{
			tmp_CCR_1 = pwm_ch_x[1].CCR_x;
			tmp_CCR_2 = pwm_ch_x[2].CCR_x;
		}
		else
		{
			tmp_CCR_1 = pwm_ch_x[2].CCR_x;
			tmp_CCR_2 = pwm_ch_x[1].CCR_x;
		}
	}
	else if (pwm_ch_x[1].CCR_x < pwm_ch_x[0].CCR_x && pwm_ch_x[1].CCR_x < pwm_ch_x[2].CCR_x)
	{
		tmp_CCR_0 = pwm_ch_x[1].CCR_x;
		if (pwm_ch_x[0].CCR_x < pwm_ch_x[2].CCR_x)
		{
			tmp_CCR_1 = pwm_ch_x[0].CCR_x;
			tmp_CCR_2 = pwm_ch_x[2].CCR_x;
		}
		else
		{
			tmp_CCR_1 = pwm_ch_x[2].CCR_x;
			tmp_CCR_2 = pwm_ch_x[0].CCR_x;
		}
	}
	else if (pwm_ch_x[2].CCR_x < pwm_ch_x[0].CCR_x && pwm_ch_x[2].CCR_x < pwm_ch_x[1].CCR_x)
	{
		tmp_CCR_0 = pwm_ch_x[2].CCR_x;
		if (pwm_ch_x[0].CCR_x < pwm_ch_x[1].CCR_x)
		{
			tmp_CCR_1 = pwm_ch_x[0].CCR_x;
			tmp_CCR_2 = pwm_ch_x[1].CCR_x;
		}
		else
		{
			tmp_CCR_1 = pwm_ch_x[1].CCR_x;
			tmp_CCR_2 = pwm_ch_x[0].CCR_x;			
		}
	}
	*/
	
	if (pwm_ch_x[0].CCR_x >= pwm_ch_x[1].CCR_x)
	{
		if (pwm_ch_x[0].CCR_x >= pwm_ch_x[2].CCR_x)
		{
			if (pwm_ch_x[1].CCR_x >= pwm_ch_x[2].CCR_x)
			{
				tmp_CCR_0 = pwm_ch_x[0].CCR_x;
				tmp_CCR_1 = pwm_ch_x[1].CCR_x;
				tmp_CCR_2 = pwm_ch_x[2].CCR_x;
			}
			else
			{
				tmp_CCR_0 = pwm_ch_x[0].CCR_x;
				tmp_CCR_1 = pwm_ch_x[2].CCR_x;
				tmp_CCR_2 = pwm_ch_x[1].CCR_x;		
			}
		}
		else
		{
			tmp_CCR_0 = pwm_ch_x[2].CCR_x;
			tmp_CCR_1 = pwm_ch_x[0].CCR_x;
			tmp_CCR_2 = pwm_ch_x[1].CCR_x;
		}
	}
	else
	{
		if (pwm_ch_x[1].CCR_x >= pwm_ch_x[2].CCR_x)
		{
			if (pwm_ch_x[0].CCR_x >= pwm_ch_x[2].CCR_x)
			{
				tmp_CCR_0 = pwm_ch_x[1].CCR_x;
				tmp_CCR_1 = pwm_ch_x[0].CCR_x;
				tmp_CCR_2 = pwm_ch_x[2].CCR_x;
			}
			else
			{
				tmp_CCR_0 = pwm_ch_x[1].CCR_x;
				tmp_CCR_1 = pwm_ch_x[2].CCR_x;
				tmp_CCR_2 = pwm_ch_x[0].CCR_x;				
			}
		}
		else
		{
			tmp_CCR_0 = pwm_ch_x[2].CCR_x;
			tmp_CCR_1 = pwm_ch_x[1].CCR_x;
			tmp_CCR_2 = pwm_ch_x[0].CCR_x;
		}		
	}
	
	
	delta_0 = tmp_CCR_0;
	delta_1 = tmp_CCR_1 - tmp_CCR_0;
	delta_2 = tmp_CCR_2 - tmp_CCR_1;
	delta_3 = (pwm_timer->TIMx_Reg_Params_prev.Period-1) - tmp_CCR_2;
	
	if (delta_0 > delta_1)
	{
		if (delta_0 > delta_2)
		{
			if (delta_0 > delta_3)
			{
				//Period = delta_0;
			}
			else
			{
				//3
			}
		}
		else
		{
			if (delta_2 > delta_3)
			{
				//2
			}
			else
			{
				//3
			}
		}
	}
	else
	{
		if (delta_1 > delta_2)
		{
			if (delta_1 > delta_3)
			{
				//1
			}
			else
			{
				//3
			}
		}
		else
		{
			if (delta_2 > delta_3)
			{
				//2
			}
			else
			{
				//3
			}
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
