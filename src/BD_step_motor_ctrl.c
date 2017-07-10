/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

void BD_Step_Motor_x_Phase_ctrl ( Step_Motor_x_Ctrl_T *p )
{
/*
	if ( p->mot_CMD == CMD_STOP && p->mot_State == MOT_MOVING )
	{
		p->mot_State = MOT_STOPPPED;
		BD_Step_Motor_x_Stop(p);
	}
	else if (p->mot_CMD == CMD_MOVE)
	{
		p->mot_State = MOT_MOVING;
	
		if ( p->position_calc == true )
		{
			if ( p->direction == DIRECT ) p->during_position++;
			else p->during_position--;
		}
		
		switch ( p->step_mode )
		{
			case AB_BC_MODE:
			{
				p->phase0 = p->phase1;
				switch ( p->phase1 ) 
				{
					case 1:
					{
						pAon(p->Phase_x);
						pCoff(p->Phase_x);
						if ( p->direction == p->rotating_reverse ) p->phase1 = 2;
						else p->phase1 = 4;
						break;
					}
					case 2:
					{
						pBon(p->Phase_x);
						pDoff(p->Phase_x); 
						if ( p->direction == p->rotating_reverse ) p->phase1 = 3;
						else p->phase1 = 1;
						break;
					}
					case 3:
					{
						pCon(p->Phase_x);
						pAoff(p->Phase_x);
						if ( p->direction == p->rotating_reverse ) p->phase1 = 4;
						else p->phase1 = 2;
						break;
					}
					case 4:
					{
						pDon(p->Phase_x);
						pBoff(p->Phase_x);
						if ( p->direction == p->rotating_reverse ) p->phase1 = 1;
						else p->phase1 = 3;
						break;
					}
				}
				break;
			}
			case A_AB_B_MODE:
			{
				
				break;	
			}
			case A_B_MODE:
			{
				
				break;	
			}	
		}		
	}
*/	
}

void BD_Step_Motor_x_Stop ( Step_Motor_x_Ctrl_T *p )
{
	for (uint8_t i = 0; i < 4; i++) HAL_GPIO_WritePin(p->Phase_x[i].GPIO_Port, p->Phase_x[i].GPIO_Pin, GPIO_PIN_RESET);
	p->phase1 = p->phase0;
}

void BD_Step_Motor_x_Logical_Init ( Step_Motor_x_Ctrl_T *p )
{
	p->phase0 = 1;
	p->phase1 = 2;
	p->mot_CMD = MOT_CMD_STOP;
	p->mot_State = MOT_STATE_STOPPPED;
}

void BD_Process_ctrl ( Process_Ctrl_T *p )
{
/*
	if ( p->proc_Mode == MODE_OPERATION && p->mode_operation_init)
	{
		
	}
	
	
	if ( p->proc_CMD == PROC_CMD_OPEN )
	{
		if (MOT_STATE_MOVING)
		{
			
		}
		else if ( p->Motor_x[0].mot_State == MOT_STATE_OPENING )
		{
			p->Motor_x[0].mot_CMD = MOT_CMD_STOP;
		}
	}
	else if ( p->proc_CMD == PROC_CMD_CLOSE )
	{
		
	}
	
	
	//if ( p->Motor_x[0].mot_State == MOT_MOVING &&  )
		
	
	p->proc_CMD = PROC_CMD_FREE;

	if ( p->Motor_x[0].mot_State == MOT_STATE_MOVING )
	{
		if (p->Motor_x[0].direction == DIRECT) p->proc_State = PROC_STATE_OPENING;
		else p->proc_State = PROC_STATE_CLOSING;
	}
	else if ( p->Motor_x[0].mot_State == MOT_STATE_STOPPPED )
	{
		p->proc_State = PROC_STATE_CLOSING;
	}
*/	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
