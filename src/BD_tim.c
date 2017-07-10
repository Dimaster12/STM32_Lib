/* Includes ------------------------------------------------------------------*/

#ifdef DAC_SIN
	#include "BD_dac.h"
#else
	#include "BD_tim.h"
#endif

void BD_TIM_ReInit(TIMx_Params_T *p)
{	
	p->htim_x->Init.Prescaler = p->TIMx_Reg_Params.Prescaler - 1;
	p->htim_x->Init.Period = p->TIMx_Reg_Params.Period - 1;

	HAL_TIM_Base_Init(p->htim_x);
}

void BD_PWM_ReInit(TIMx_PWM_Params_T *p)
{	
	p->htim_x->Init.Prescaler = p->TIMx_Reg_Params.Prescaler - 1;
	p->htim_x->Init.Period = p->TIMx_Reg_Params.Period - 1;

	HAL_TIM_Base_Init(p->htim_x);
}

void BD_TIM_Logical (TIMx_Params_T *p)
{
	if (!p->TIMx_init_was)
	{
		p->TIMx_Init_func();
		if (p->TIMx_Mode == FREQ_SET_INTERRUPT || p->TIMx_Mode == DAC_SINUS)
		{			
			#ifdef DAC_SIN
			if (p->TIMx_Mode == DAC_SINUS)
			{	
				BD_DAC_Num_Points_Calc();
				BD_DAC_Init_Calc();
			}
			#endif
			BD_TIMx_Params_Calc(&p->TIMx_Reg_Params, p->APBx_Freq);
		}		
		BD_TIM_ReInit(p);
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;
		
		p->T_samp_FLOAT = 1.0F/p->TIMx_Reg_Params.Freq;
		p->T_samp_Q16 = fix16_from_float(p->T_samp_FLOAT);
		
		p->TIMx_dt = (uint32_t)((uint64_t)p->TIMx_Reg_Params.Prescaler*1000000/p->APBx_Freq);
		
		BD_TIM_Init_Callback(p);
		p->TIMx_init_was = true;
	}
	else if ( memcmp(&p->TIMx_Reg_Params, &p->TIMx_Reg_Params_prev, sizeof(p->TIMx_Reg_Params)) != 0 )
	{		
		if (p->TIMx_Mode == FREQ_SET_INTERRUPT || p->TIMx_Mode == DAC_SINUS)
		{
			#ifdef DAC_SIN
			if (p->TIMx_Mode == DAC_SINUS)
			{	
				BD_DAC_Num_Points_Calc();
				BD_DAC_Init_Calc();
			}
			#endif
			BD_TIMx_Params_Calc(&p->TIMx_Reg_Params, p->APBx_Freq);
		}
		BD_TIM_ReInit(p);		
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;

		p->T_samp_FLOAT = 1.0F/p->TIMx_Reg_Params.Freq;
		p->T_samp_Q16 = fix16_from_float(p->T_samp_FLOAT);
		
		p->TIMx_dt = (uint32_t)((uint64_t)p->TIMx_Reg_Params.Prescaler*1000000/p->APBx_Freq);	
	}

	if (p->TIMx_en && !p->TIMx_en_was)
	{
		if (p->TIMx_Mode == FREQ_SET_INTERRUPT || p->TIMx_Mode == DAC_SINUS) HAL_TIM_Base_Start_IT(p->htim_x);
		else if (p->TIMx_Mode == TIME_MEASURE_NOINTERRUPT) HAL_TIM_Base_Start(p->htim_x);	

		p->TIMx_en_was = true;
		p->TIMx_dis_was = false;
	}
	else if (!p->TIMx_en && !p->TIMx_dis_was)
	{
		if (p->TIMx_Mode == FREQ_SET_INTERRUPT || p->TIMx_Mode == DAC_SINUS) HAL_TIM_Base_Stop_IT(p->htim_x);
		else if (p->TIMx_Mode == TIME_MEASURE_NOINTERRUPT) HAL_TIM_Base_Stop(p->htim_x);
		p->htim_x->Instance->CNT = 0UL;

		p->TIMx_en_was = false;
		p->TIMx_dis_was = true;
	}
}


/******************************************************************/
// PWM

__weak void BD_TIM_PWM_EN (void){}
__weak void BD_TIM_PWM_DIS(void){}

void BD_TIMx_PWM_Logical(TIMx_PWM_Params_T *p)
{
	if (!p->TIMx_init_was)
	{
		p->TIMx_Init_func();
		BD_TIMx_PWM_Params_Calc (&p->TIMx_Reg_Params, p->APBx_Freq);
		BD_PWM_ReInit(p);
	#ifdef PWM_SIN	
		BD_PWMx_SIN_Params_Calc(p);
	#endif	
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;
		p->TIMx_init_was = true;
	}
	else if ( memcmp(&p->TIMx_Reg_Params, &p->TIMx_Reg_Params_prev, sizeof(p->TIMx_Reg_Params)) != 0)
	{
		if (p->TIMx_en_was) BD_TIM_PWM_DIS();					// Вырубаем ШИМ, чтобы в момент переделывания ничего не бахнуло		
		if (p->TIMx_Reg_Params.Freq_PWM != p->TIMx_Reg_Params_prev.Freq_PWM)
		{
			BD_TIMx_PWM_Params_Calc (&p->TIMx_Reg_Params, p->APBx_Freq);
			BD_PWM_ReInit(p);
		}
	#ifdef PWM_SIN
		BD_PWMx_SIN_Params_Calc(p);
	#endif
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;
		p->Point_timer_reset_flag = true;
		if (p->TIMx_en_was) BD_TIM_PWM_EN();					// Если до этого ехали, едем дальше
	}

	if (p->TIMx_en && !p->TIMx_en_was)
	{
		BD_TIM_PWM_EN();
		__HAL_TIM_ENABLE_IT(p->htim_x, TIM_IT_UPDATE); // Включить прерывание UPDATE
		p->TIMx_en_was = true;
		p->TIMx_dis_was = false;
	}
	else if (!p->TIMx_en && !p->TIMx_dis_was)
	{
		BD_TIM_PWM_DIS();
		__HAL_TIM_DISABLE_IT(p->htim_x, TIM_IT_UPDATE); // Включить прерывание UPDATE
		p->TIMx_en_was = false;
		p->TIMx_dis_was = true;
	}
}

void BD_PWM_CHx_Logical(PWM_CHx_Params_T *p)
{
	if (!p->TIMx_init_was)
	{
		__HAL_TIM_SetCompare(p->htim_x, p->Channel_x, p->CCR_x);
		p->TIMx_init_was = true;
	}
	
	//p->CCR_x_readen = __HAL_TIM_GetCompare(p->htim_x, p->Channel_x);
	//if ( p->CCR_x != p->CCR_x_readen )
	if ( p->CCR_x != __HAL_TIM_GetCompare(p->htim_x, p->Channel_x) )
	{
		__HAL_TIM_SetCompare(p->htim_x, p->Channel_x, p->CCR_x);
	}

	if (p->TIMx_en && !p->TIMx_en_was)
	{
		if (p->Mode.bit.DIRECT_CH) HAL_TIM_PWM_Start_IT(p->htim_x, p->Channel_x);	
		if (p->Mode.bit.INVERSE_CH) HAL_TIMEx_OCN_Start_IT(p->htim_x, p->Channel_x);
		p->TIMx_en_was = true;
		p->TIMx_dis_was = false;
	}
	else if (!p->TIMx_en && !p->TIMx_dis_was)
	{
		if (p->Mode.bit.DIRECT_CH) HAL_TIM_PWM_Stop_IT(p->htim_x, p->Channel_x);
		if (p->Mode.bit.INVERSE_CH) HAL_TIMEx_OCN_Stop_IT(p->htim_x, p->Channel_x);	
		p->TIMx_en_was = false;
		p->TIMx_dis_was = true;
	}
}

void BD_TIMx_PWM_Params_Calc(PWMx_Reg_Params_T *p, uint32_t APBx_Freq)
{
	uint16_t 	test_Prescaler;
	float			test_Freq_rez;	
	
	for (uint16_t test_Period = 65535; test_Period > 0; test_Period--)
	{
		test_Prescaler = APBx_Freq/((uint32_t)p->Freq_PWM*test_Period);
		test_Freq_rez = (float)APBx_Freq/((uint32_t)test_Prescaler*test_Period);
		
		if ( (fabs(test_Freq_rez-p->Freq_PWM)*100.0F)/p->Freq_PWM < 1.0F )
		{
			p->Prescaler = test_Prescaler;
			p->Period = test_Period;		
			return;	
		}
	}
}

void BD_PWMx_SIN_Params_Calc(TIMx_PWM_Params_T *p)
{
	float d_float;
	float zero_harm = (float)p->TIMx_Reg_Params.Period / 2.0F;
	float sin_input = 0.0F;

	p->Num_Points = p->TIMx_Reg_Params.Freq_PWM/p->TIMx_Reg_Params.Freq_SIN;
	d_float = 6.28F / p->Num_Points;

	for (uint16_t i = 0; i < p->Num_Points; i++)
	{
		sin_input += d_float;
		//p->SIN_array[0] = 20000;
		//p->SIN_array[i] = (uint16_t)(100.0F/ (sin(sin_input)*(zero_harm - ((p->TIMx_Reg_Params.Ampl_SIN*zero_harm)/100.0F) ) + zero_harm));
		p->SIN_array[i] = (uint16_t)( ((sin(sin_input)*p->TIMx_Reg_Params.Ampl_SIN)/100.0F)*zero_harm + zero_harm );
		//(uint16_t)((sin(sin_input)*(zero_harm - (( (100.0F/  p->TIMx_Reg_Params.Ampl_SIN)   *zero_harm)/100.0F) ) + zero_harm));
	}
}
/******************************************************************/

__weak void BD_TIM_Init_Callback(TIMx_Params_T *p)
{
	UNUSED(p);
}

void BD_TIM_Logical_params_init (TIMx_Params_T *p) // Init one time for each TIMx
{
	p->TIMx_init_was					=				false;	
	p->TIMx_en_was						=				false;
	p->TIMx_dis_was						=				true;
	p->TIMx_function_break		=				0;
}

void BD_TIMx_PWM_Logical_params_init (TIMx_PWM_Params_T *p)
{
	p->TIMx_init_was					=				false;	
	p->TIMx_en_was						=				false;
	p->TIMx_dis_was						=				true;
	p->Point_timer_reset_flag =				true;
}

void BD_PWM_CHx_Logical_params_init (PWM_CHx_Params_T *p)
{
	p->TIMx_init_was					=				false;	
	p->TIMx_en_was						=				false;
	p->TIMx_dis_was						=				true;
	p->Point_timer						=				0;
}

void BD_TIMx_Params_Calc (TIMx_Reg_Params_T *p, uint32_t APBx_Freq)
{
	float log_var1;
	float log_var2;
	float log_var3;
	float log_var4;
	float log_var5;
	
	log_var3 = log10(Freq2) - log10(Freq1);
	log_var1 = log10(p->Freq) - log10(Freq1);
	log_var2 = log10(Presc2) - log10(Presc1);
	log_var4 = log10(Presc1);
	log_var5 =(log_var1*log_var2)/log_var3+log_var4;
	
	log_var1 = powf(10.0F,log_var5);
	p->Prescaler = (uint16_t)(ceil(log_var1));	
	p->Period = (uint16_t)(APBx_Freq/(p->Freq*(float)(p->Num_Points)*(float)p->Prescaler));
}

void BD_TIM_Time_Calc (TIM_Calc_Params_T *p, TIMx_Params_T *v, bool Command)
{
	if (Command == VIEW_RESULT && p->State == TIME_CALC_ACTIVE)
	{
		if (v->htim_x->Instance->CNT >= p->Init_CNT_value)
		{
			p->Result_time_us = v->TIMx_dt*( v->htim_x->Instance->CNT - p->Init_CNT_value);
		}
		else
		{
			p->Result_time_us = v->TIMx_dt*(v->htim_x->Instance->CNT + v->TIMx_Reg_Params.Period - p->Init_CNT_value);
		}
	}
	else if (Command == TIME_CALC_ACTIVE)
	{
		p->Init_CNT_value = v->htim_x->Instance->CNT;
		p->State = TIME_CALC_ACTIVE;
	}
}

void BD_TIM_Freq_Calc (Freq_Calc_Params_T *p, TIMx_Params_T *v)
{
	if (p->State == TIME_CALC_ACTIVE)
	{
		if (v->htim_x->Instance->CNT >= p->Init_CNT_value)
		{
			p->Result_freq = (float)(1000000.0F/((float)v->TIMx_dt*(v->htim_x->Instance->CNT - p->Init_CNT_value)));
		}
		else
		{
			p->Result_freq = (float)(1000000.0F/((float)v->TIMx_dt*(v->htim_x->Instance->CNT + v->TIMx_Reg_Params.Period - p->Init_CNT_value)));
		}
	}
	else if (p->State == WAITING_INIT_TIME) p->State = TIME_CALC_ACTIVE;
	p->Init_CNT_value = v->htim_x->Instance->CNT;
}

void BD_TIM_Time_Calc_Init (TIM_Calc_Params_T *p)
{
	p->State = WAITING_INIT_TIME;
	p->Init_CNT_value = 0UL;
	p->Result_time_us = 0UL;
}

void BD_TIM_Freq_Calc_Init (Freq_Calc_Params_T *p)
{
	p->State = WAITING_INIT_TIME;
	p->Init_CNT_value = 0UL;
	p->Result_freq = 0.0F;
}

void BD_TIM_Delay (TIM_Calc_Params_T *p, TIMx_Params_T *m , uint32_t delay_time)
{
	BD_TIM_Time_Calc (p, m, TIME_CALC_ACTIVE);
	do
	{		
		BD_TIM_Time_Calc (p, m, VIEW_RESULT);
	}
	while ( p->Result_time_us < delay_time);
}
	
void BD_TIM_Encoder_Speed_calc(TIMx_Encoder_Params_T *p)
{
	if (!p->First_Calc)
	{		
		p->CNT1 = __HAL_TIM_GET_COUNTER(p->htim_x);
		p->DIR1 = (bool)((p->htim_x->Instance->CR1 & TIM_CR1_DIR_Msk)>>4);
			
		if ( p->Time < 1000000000L ) p->Time++;	

		if ( (float)p->Time/(*p->Freq) > 0.1F ) p->Calc_Speed = 0;

		if ( p->CNT1 != p->CNT0 && p->DIR1 == p->DIR0 )
		{
			if ( p->DIR1 == GO_UP )
			{
				if ( p->CNT1 > p->CNT0 ) p->Calc_Mode = CALC_MODE_1;
				else if ( p->CNT1 < p->CNT0 )
				{
					p->Calc_Mode = CALC_MODE_2;
					p->Calc_Rev++;
				}
			}
			else if ( p->DIR1 == GO_DOWN )
			{
				if ( p->CNT1 < p->CNT0 )
				{
					p->Calc_Mode = CALC_MODE_1;
				}
				else if ( p->CNT1 > p->CNT0 )
				{
					p->Calc_Mode = CALC_MODE_2;
					p->Calc_Rev--;
				}					
			}

			if ( p->Calc_Mode == CALC_MODE_1 )
			{
				p->Calc_Speed = ((((float)p->CNT1 - (float)p->CNT0)*(*p->Freq)) /(float)p->htim_x->Init.Period)/(float)p->Time;
			}
			else if ( p->Calc_Mode == CALC_MODE_2 )
			{
				if ( p->DIR1 == GO_UP )
				{
					p->Calc_Speed = (((((float)p->htim_x->Init.Period+(float)p->CNT1+1.0F) - (float)p->CNT0 )*(*p->Freq)) / (float)p->htim_x->Init.Period) /(float)p->Time;
				}
				else if ( p->DIR1 == GO_DOWN )
				{
					p->Calc_Speed = (-1.0F)*((((((float)p->htim_x->Init.Period+(float)p->CNT0+1.0F) - (float)p->CNT1 )*(*p->Freq)) / (float)p->htim_x->Init.Period) /(float)p->Time);
				}						
			}
			p->Time = 0;
		}
		else if (p->DIR1 != p->DIR0)
		{
			if ( p->DIR1 == GO_UP )
			{
				if ( p->CNT1 < p->CNT0 ) p->Calc_Rev++;
			}
			else if ( p->DIR1 == GO_DOWN )
			{
				if ( p->CNT1 > p->CNT0 ) p->Calc_Rev--;				
			}
		}
		p->CNT0 = p->CNT1;
		p->DIR0 = p->DIR1;
	}
	else
	{
		p->First_Calc = false;

		p->CNT0 = __HAL_TIM_GET_COUNTER(p->htim_x);
		p->DIR0 = (bool)((p->htim_x->Instance->CR1 & TIM_CR1_DIR_Msk)>>4);
		
		p->pos_Timer = 0;
		p->Time = 0;
	}
}

void BD_TIM_Encoder_Init_params(TIMx_Encoder_Params_T *p)
{
	p->First_Calc = true;
	p->Calc_Speed = 0;
	p->Calc_Rev = 0;
	p->Time	= 0;
	
	p->TIMx_Init_func();
}

void BD_TIM_Encoder_Logical(TIMx_Encoder_Params_T *p)
{
  TIM_Encoder_InitTypeDef sConfig;

	if (p->ICFilter != p->ICFilter_prev)
	{
		p->ICFilter_prev = p->ICFilter;
		
		sConfig.IC1Filter = p->ICFilter;
		sConfig.IC2Filter = p->ICFilter;

		sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
		sConfig.IC1Polarity = TIM_ICPOLARITY_BOTHEDGE;
		sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
		sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
		sConfig.IC2Polarity = TIM_ICPOLARITY_BOTHEDGE;
		sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
		sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
		
		HAL_TIM_Encoder_Init(p->htim_x, &sConfig);
	}
}

void BD_TIM_PWM_Init_params(TIMx_Encoder_Params_T *p)
{
	p->First_Calc = true;
	p->Calc_Speed = 0;
	p->Calc_Rev = 0;
	p->Time	= 0;
	
	p->TIMx_Init_func();
}

/*
void MX_TIME_SOURCE_Logical_Init(TIMx_Params_T *p)
{
	TIM_Logical_params_init(p);
	
	p->TIMx = TIM3;
	p->TIMx_htim = &htim3;
	p->TIMx_Mode = TIME_MEASURE_NOINTERRUPT;
	
	p->TIMx_en = true;
	p->TIMx_Reg_Params.TIMx_prescaler = HAL_RCC_GetHCLKFreq()/100000UL;
	p->TIMx_Reg_Params.CounterMode = TIM_COUNTERMODE_UP;
	p->TIMx_Reg_Params.TIMx_period = 50000;
	p->TIMx_Reg_Params.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	p->Freq_Set_allowed = false;
	p->TIMx_Freq = 1.0F;
	p->TIMx_points = 1;
	
	TIM_Time_Calc_Init(&TIME_SOURCE_PARAMS);
}

TIMx_Params_T TEST_INT_TIMER;

void BD_"NAME"_INTERRUPT_Logical_Init(TIMx_Params_T *p)
{
	BD_TIM_Logical_params_init(p);
	
	p->htim_x = &htim"X";
	p->TIMx_Init_func = MX_TIMX""_Init;
	p->TIMx_Mode = DAC_SINUS;
	p->APBx_Freq = HAL_RCC_GetPCLK"X"Freq()*"N";
	
	p->TIMx_en = true;
	p->TIMx_Reg_Params.Freq = 46.5F;
	p->TIMx_Reg_Params.Prescaler = NO_MATTER;
	p->TIMx_Reg_Params.Period = NO_MATTER;
	p->TIMx_Reg_Params.Num_Points = 1;
}

void BD_ENCODER_LOW_Logical_Init(TIMx_Encoder_Params_T *p)
{
	BD_TIM_Encoder_Init_params(p);
	
	p->htim_x = &htim"X";
	p->Freq = &TEST_INT_TIMER.TIMx_Reg_Params.Freq;
	MX_TIM"X"_Init();
	p->ICFilter = 15;
	BD_TIM_Encoder_Logical(p);
	HAL_TIM_Encoder_Start(p->htim_x, TIM_CHANNEL_ALL);
}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
