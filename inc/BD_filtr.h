/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __adc_H_dim
#define __adc_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "BD_general.h"

//FILTR_1{
/**********************************************************/
//FLOAT{
typedef struct
{
	// Function inited (must be inited by special function ones):
	float														K1;
	float														K2;

	float														T_samp_prev;
	float														T_filt_prev;		
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	float*													T_samp;	// Sampling period
	float														Y0;
	// Changing params (must be inited, may be reinited by developer):	
	float														T_filt;		// Filtr time		
	// Logic inited params (mustn`t be inited, reinited):
} Filtr_1_Params_FLOAT_T;

void Filtr_1_FLOAT_Reg_Params_init (Filtr_1_Params_FLOAT_T *p);
void Filtr_1_FLOAT_init (Filtr_1_Params_FLOAT_T *p);
void Filtr_1_FLOAT_Reinit_Chack (Filtr_1_Params_FLOAT_T *p);
float Filtr_1_FLOAT (Filtr_1_Params_FLOAT_T *p, float X1);
//FLOAT}
/*******/

//Q16{
typedef struct
{
	// Function inited (must be inited by special function ones):
	fix16_t													K1;
	fix16_t													K2;

	fix16_t													T_samp_prev;
	fix16_t													T_filt_prev;	
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	fix16_t*												T_samp;	// Sampling period
	fix16_t													Y0;				// Previous output value
	// Changing params (must be inited, may be reinited by developer):
	fix16_t													T_filt;		// Filtr time		
	// Logic inited params (mustn`t be inited, reinited):
} Filtr_1_Params_Q16_T;

void Filtr_1_Q16_Reg_Params_init (Filtr_1_Params_Q16_T *p);
void Filtr_1_Q16_init (Filtr_1_Params_Q16_T *p);
void Filtr_1_Q16_Reinit_Chack (Filtr_1_Params_Q16_T *p);
fix16_t Filtr_1_Q16 (Filtr_1_Params_Q16_T *p, fix16_t X1);
//Q16}
/**********************************************************/
//FILTR_1}

//FILTR_2{
/**********************************************************/

typedef struct
{
	// Function inited (must be inited by special function ones):
	float														K1;
	float														K2;
	float														K3;

	float														T_samp_prev;
	float														T_filt_prev;			
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	float*													T_samp;
	float														Y0;				// Previous output value
	float														Y1;				// Previous output value
	// Changing params (must be inited, may be reinited by developer):
	float														T_filt;	
	// Logic inited params (mustn`t be inited, reinited):	
} Filtr_2_Params_FLOAT_T;

void Filtr_2_FLOAT_Reg_Params_init (Filtr_2_Params_FLOAT_T *p);
void Filtr_2_FLOAT_init (Filtr_2_Params_FLOAT_T *p);
void Filtr_2_FLOAT_Reinit_Chack (Filtr_2_Params_FLOAT_T *p);
float Filtr_2_FLOAT (Filtr_2_Params_FLOAT_T *p, float X2);

/*******/

//FILTR_2 Q16
typedef struct
{
	// Function inited (must be inited by special function ones):
	fix16_t													K1;
	fix16_t													K2;
	fix16_t													K3;

	fix16_t													T_samp_prev;
	fix16_t													T_filt_prev;	
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	fix16_t*												T_samp;
	fix16_t													Y0;				// Previous output value
	fix16_t													Y1;				// Previous output value
	// Changing params (must be inited, may be reinited by developer):
	fix16_t													T_filt;	
} Filtr_2_Params_Q16_T;

void Filtr_2_Q16_Reg_Params_init (Filtr_2_Params_Q16_T *p);
void Filtr_2_Q16_init (Filtr_2_Params_Q16_T *p);
void Filtr_2_Q16_Reinit_Chack (Filtr_2_Params_Q16_T *p);
fix16_t Filtr_2_Q16 (Filtr_2_Params_Q16_T *p, fix16_t X2);
/**********************************************************/
//FILTR_2}

#ifdef __cplusplus
}
#endif
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
