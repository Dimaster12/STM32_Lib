/* Includes ------------------------------------------------------------------*/
#include "dac.h"

DAC_Params_T DAC_Params;

void BD_DAC_sin_array_calc (DAC_Channel_Params_T *p)
{
	float log_var1;
	float log_var2;
	float log_var3;
	float delta;
	float Zero_level_DAC;
	float Amplitude_DAC;
	float sin_input = 0.0F;
	float sin_output;

	log_var1 = log10(DAC_Params.TIMx->TIMx_Reg_Params.Freq)*(Points2-Points1);
	log_var2 = Points1*log10(Freq2)-Points2*log10(Freq1);
	log_var3 = log10(Freq2) - log10(Freq1);

	if (p->Reg_Params.Zero_angle != 0)
	{
		p->Zero_angle_points = (uint16_t)(ceil((log_var2 + log_var1)/((log_var3*360.0F)/p->Reg_Params.Zero_angle)));
	}
	else p->Zero_angle_points = 0;
	
	delta = 6.283185F/((float)DAC_Params.TIMx->TIMx_Reg_Params.Num_Points);	
	Zero_level_DAC = (p->Reg_Params.Zero_level*4095.0F)/MAX_VOLT;
	Amplitude_DAC = (p->Reg_Params.Ampl_level*4095.0F)/MAX_VOLT;

	for (uint16_t i = 0; i< DAC_Params.TIMx->TIMx_Reg_Params.Num_Points; i++)	
	{
		sin_output = sin(sin_input);
		sin_input += delta;		
		p->DAC_array[i] = (uint16_t) (ceil((sin_output*Amplitude_DAC) + Zero_level_DAC));
		/*
		if ((i == DAC_Params.TIMx->TIMx_Reg_Params.Num_Points) && (p->DAC_array[i] >= p->DAC_array[0]))
		{
			//p->Array_size--;
			i++;
		}
		*/
	}
}

void BD_DAC_Logical (void)
{
	bool recalc_flag = false;
	DAC_Channel_Params_T *tmp = DAC_Params.DAC_Channels;
	
	for (uint8_t i = 0; i < DAC_CHANNELS; i++)
	{
		if ( memcmp(&tmp->Reg_Params, &tmp->Reg_Params_prev, sizeof(tmp->Reg_Params)) != 0 )
		{
			DAC_Params.TIMx->TIMx_en = false;
			BD_TIM_Logical(DAC_Params.TIMx);	
			
			BD_DAC_sin_array_calc(tmp);
			
			recalc_flag = true;
		}
		tmp++;
	}
	if (recalc_flag)
	{
		BD_DAC_angle_reset();
		DAC_Params.TIMx->TIMx_en = true;
		BD_DAC_Init_Logical();
		BD_TIM_Logical(DAC_Params.TIMx);
	}
}

void BD_DAC_Init_Calc (void)
{
	DAC_Channel_Params_T *tmp = DAC_Params.DAC_Channels;
	
	for (uint8_t i = 0; i < DAC_CHANNELS; i++)
	{
		BD_DAC_sin_array_calc(tmp);
		tmp++;
	}
	BD_DAC_angle_reset();
	BD_DAC_Init_Logical();
}

void BD_DAC_angle_reset (void)
{
	DAC_Channel_Params_T *tmp = DAC_Params.DAC_Channels;
	
	for  (uint8_t i = 0; i < DAC_CHANNELS; i++)
	{
		if (tmp->Reg_Params.Zero_angle > 0)
		{
			tmp->during_Array_element = DAC_Params.TIMx->TIMx_Reg_Params.Num_Points - tmp->Zero_angle_points;
		}
		else if (tmp->Reg_Params.Zero_angle < 0)
		{
			tmp->during_Array_element = tmp->Zero_angle_points;
		}
		else tmp->during_Array_element = 0;
		tmp++;
	}
}

void BD_DAC_signals_generate (void)
{
	DAC_Channel_Params_T *tmp = DAC_Params.DAC_Channels;

	for  (uint8_t i = 0; i < DAC_CHANNELS; i++)
	{
		/*				// Переход через 0
		if (tmp->during_Array_element == DAC_Params.TIMx->TIMx_Reg_Params.Num_Points-1)
		{
			HAL_DAC_SetValue(&hdac, tmp->Channel, DAC_ALIGN_12B_R, 0);
		}
		else
		{
		*/
			HAL_DAC_SetValue(&hdac, tmp->Channel, DAC_ALIGN_12B_R, tmp->DAC_array[tmp->during_Array_element]);			
		//}		
		tmp->during_Array_element++;
		if (tmp->during_Array_element >= DAC_Params.TIMx->TIMx_Reg_Params.Num_Points) tmp->during_Array_element = 0;
		tmp++;
	}
}

void BD_DAC_Init_Logical (void)
{
	DAC_Channel_Params_T *tmp = DAC_Params.DAC_Channels;
	
	for  (uint8_t i = 0; i < DAC_CHANNELS; i++)
	{
		tmp->Reg_Params_prev.Zero_level = tmp->Reg_Params.Zero_level;
		tmp->Reg_Params_prev.Ampl_level = tmp->Reg_Params.Ampl_level;
		tmp->Reg_Params_prev.Zero_angle = tmp->Reg_Params.Zero_angle;
		tmp++;
	}
}

void BD_DAC_Num_Points_Calc(void)
{
	float log_var1;
	float log_var2;
	float log_var3;

	log_var1 = log10(DAC_Params.TIMx->TIMx_Reg_Params.Freq)*(Points2-Points1);
	log_var2 = Points1*log10(Freq2)-Points2*log10(Freq1);
	log_var3 = log10(Freq2) - log10(Freq1);
	//DAC_Params.Array_size = (uint16_t)(ceil((log_var2 + log_var1)/log_var3));
	DAC_Params.TIMx->TIMx_Reg_Params.Num_Points = (uint16_t)(ceil((log_var2 + log_var1)/log_var3));
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
