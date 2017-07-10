/* Includes ------------------------------------------------------------------*/
#include "adc.h"

//FILTR 1{
/**************************************************/
// FLOAT
void Filtr_1_FLOAT_Reg_Params_init (Filtr_1_Params_FLOAT_T *p)
{
	p->T_samp_prev = 0;
	p->T_filt_prev = 0;
}

void Filtr_1_FLOAT_init (Filtr_1_Params_FLOAT_T *p)
{
	float	tmp_add = *p->T_samp+p->T_filt;

	p->K1 = *p->T_samp/tmp_add;
	p->K2 = p->T_filt/tmp_add;
}

void Filtr_1_FLOAT_Reinit_Chack (Filtr_1_Params_FLOAT_T *p)
{
	if ( *p->T_samp != p->T_samp_prev || p->T_filt != p->T_filt_prev )
	{
		p->T_samp_prev = *p->T_samp;
		p->T_filt_prev = p->T_filt;
		Filtr_1_FLOAT_init(p);
	}
}

float Filtr_1_FLOAT (Filtr_1_Params_FLOAT_T *p, float X1)
{
	return p->Y0 = X1*p->K1 + p->Y0*p->K2;
}

// Q16
void Filtr_1_Q16_Reg_Params_init (Filtr_1_Params_Q16_T *p)
{
	p->T_samp_prev = F16(0.0);
	p->T_filt_prev = F16(0.0);	
}

void Filtr_1_Q16_init (Filtr_1_Params_Q16_T *p)
{
	fix16_t	tmp_add = Q16_ADD(*p->T_samp,p->T_filt);
	
	p->K1 = Q16_DIV(*p->T_samp, tmp_add );
	p->K2 = Q16_DIV(p->T_filt, tmp_add );
}

void Filtr_1_Q16_Reinit_Chack (Filtr_1_Params_Q16_T *p)
{
	if ( *p->T_samp != p->T_samp_prev || p->T_filt != p->T_filt_prev )
	{
		p->T_samp_prev = *p->T_samp;
		p->T_filt_prev = p->T_filt;
		Filtr_1_Q16_init(p);
	}
}

fix16_t Filtr_1_Q16 (Filtr_1_Params_Q16_T *p, fix16_t X1)
{	
	return p->Y0 = Q16_ADD(Q16_MUL(X1, p->K1),Q16_MUL(p->Y0, p->K2));
}
/**************************************************/
//FILTR 1}

//FILTR 2{
/**************************************************/
// FLOAT{

void Filtr_2_FLOAT_Reg_Params_init (Filtr_2_Params_FLOAT_T *p)
{
	p->T_samp_prev = 0;
	p->T_filt_prev = 0;
}

void Filtr_2_FLOAT_init (Filtr_2_Params_FLOAT_T *p)
{
	float	tmp_var = powf((1.0F+p->T_filt/(*p->T_samp)),2.0F);
 	
	p->K1 = 1.0F / tmp_var;
	p->K2 = ((2.0F*p->T_filt/(*p->T_samp))*(p->T_filt/(*p->T_samp)+1.0F))/tmp_var;
	p->K3 = -1.0F*powf((p->T_filt/(*p->T_samp)),2)/tmp_var;
}

void Filtr_2_FLOAT_Reinit_Chack (Filtr_2_Params_FLOAT_T *p)
{
	if ( *p->T_samp != p->T_samp_prev || p->T_filt != p->T_filt_prev )
	{
		p->T_samp_prev = *p->T_samp;
		p->T_filt_prev = p->T_filt;
		Filtr_2_FLOAT_init(p);
	}
}

float Filtr_2_FLOAT (Filtr_2_Params_FLOAT_T *p, float X2)
{
	float Y2 = X2*p->K1+p->Y1*p->K2+p->Y0*p->K3;

	p->Y0 = p->Y1;
	return p->Y1 = Y2;
}
// FLOAT}

// Q16
void Filtr_2_Q16_Reg_Params_init (Filtr_2_Params_Q16_T *p)
{
	p->T_samp_prev = F16(0.0);
	p->T_filt_prev = F16(0.0);
}

void Filtr_2_Q16_init (Filtr_2_Params_Q16_T *p)
{
	fix16_t tmp_var_1 = Q16_ADD(F16(1.0),Q16_DIV(p->T_filt,(*p->T_samp)));
	fix16_t	tmp_var_2 = Q16_MUL(tmp_var_1, tmp_var_1);
	
	p->K1 = Q16_DIV(F16(1.0),tmp_var_2);
	p->K2 = Q16_DIV(Q16_MUL(Q16_DIV(Q16_MUL(F16(2.0),p->T_filt),*p->T_samp),Q16_ADD(Q16_DIV(p->T_filt,*p->T_samp),F16(1.0))),tmp_var_2);	
	p->K3 = Q16_DIV(Q16_MUL(F16(-1.0),Q16_MUL(Q16_DIV(p->T_filt,*p->T_samp),Q16_DIV(p->T_filt,*p->T_samp))),tmp_var_2);	
}

void Filtr_2_Q16_Reinit_Chack (Filtr_2_Params_Q16_T *p)
{
	if ( *p->T_samp != p->T_samp_prev || p->T_filt != p->T_filt_prev )
	{
		p->T_samp_prev = *p->T_samp;
		p->T_filt_prev = p->T_filt;
		Filtr_2_Q16_init(p);
	}
}

fix16_t Filtr_2_Q16 (Filtr_2_Params_Q16_T *p, fix16_t X2)
{
	fix16_t Y2 = Q16_ADD(Q16_ADD(Q16_MUL(X2,p->K1),Q16_MUL(p->Y1,p->K2)),Q16_MUL(p->Y0,p->K3));

	p->Y0 = p->Y1;
	return p->Y1 = Y2;
}
/**************************************************/
//FILTR 2}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
