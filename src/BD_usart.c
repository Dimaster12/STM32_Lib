/* Includes ------------------------------------------------------------------*/
#include "BD_usart.h"

void USART_Logical (USARTx_Params_T *p)		// Necessary calc discrete
{
	bool reset_flag = false;
	
	if (p->USARTx_init && !p->USARTx_init_was)
	{
		p->USARTx_Init_func();
		p->USARTx_init_was = true;

		p->USARTx_BaudRate = p->husart_x->Init.BaudRate;		
		p->USARTx_BaudRate_prev = p->USARTx_BaudRate;
	
		p->Num_packets_send_prev = p->Num_packets_send;
		p->Num_packets_get_prev = p->Num_packets_get;
		
		p->during_get_adr = p->data_get_8;

		MX_USART_GetData(p);
	}

	if (p->USARTx_BaudRate != p->USARTx_BaudRate_prev)
	{
		MX_USART_ReInit(p);
		p->USARTx_BaudRate_prev = p->USARTx_BaudRate;
		reset_flag = true;
	}

	if (p->Num_packets_get != p->Num_packets_get_prev )
	{
		p->Num_packets_get_prev = p->Num_packets_get;		
		reset_flag = true;
	}

	if (reset_flag) 
	{	
		reset_flag = false;
		BD_USART_Reset_Get_Interrupt(p, p->reset_Callback);
	}

	if (p->Num_packets_send != p->Num_packets_send_prev )
	{
		p->Num_packets_send_prev = p->Num_packets_send;
		//p->husart_x->TxXferSize = (uint16_t)p->Num_packets_send_prev;
		//p->husart_x->TxXferCount = (uint16_t)p->Num_packets_send_prev;
	}
}

void USART_Logical_Init_Params (USARTx_Params_T *p)
{
	p->USARTx_init_was 							= 	false;
	p->USARTx_function_break 					=	0;
	
	p->Rx_Calc = 0;
	p->Tx_Calc = 0;
	
	p->Rx_Calc_prev = 0;
	p->Tx_Calc_prev = 0;	
}

void MX_USART_ReInit(USARTx_Params_T *p)	
{
	p->husart_x->Init.BaudRate = p->USARTx_BaudRate;
	
	if (!p->One_Wire_Mode) HAL_UART_Init(p->husart_x);
	else HAL_HalfDuplex_Init(p->husart_x);
}

void MX_USART_GetData(USARTx_Params_T *p)
{
	HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
}

void MX_USART_SendData(USARTx_Params_T *p)
{
	HAL_UART_Transmit_DMA(p->husart_x, (uint8_t*)p->data_send_8, p->Num_packets_send_prev);
}

void MX_USART_ClearGettedData(USARTx_Params_T *p)
{
	for (uint8_t i = 0; i < p->Num_packets_get_prev; i++) p->data_get_8[i] = 0;
}

void BD_USART_Modbus_Send_data(USARTx_Params_T *p, uint8_t array_lenth)
{
	p->Num_packets_send_prev = array_lenth;
	MX_USART_SendData(p);
}

void BD_Zero_Func (void)
{
	return;
}

void BD_USART_Reset_Rx (DMA_HandleTypeDef *hdma, bool Callback_allowed )
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

#if defined (F1XX)
  if ( HAL_IS_BIT_CLR(hdma->Instance->CCR, DMA_CCR_CIRC) )
  {
    huart->RxXferCount = 0;
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    if(huart->State == HAL_UART_STATE_BUSY_TX_RX) huart->State = HAL_UART_STATE_BUSY_TX;
    else huart->State = HAL_UART_STATE_READY;
  }
#elif defined(F4XX)
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U)
  {
    huart->RxXferCount = 0U;
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    huart->RxState = HAL_UART_STATE_READY;
  }
#endif
	if (Callback_allowed) HAL_UART_RxCpltCallback(huart);
}

#if defined (F4XX)
typedef struct
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;
#endif

void BD_USART_Reset_Get_Interrupt (USARTx_Params_T *p, bool Callback_call)
{
#if defined (F1XX)

	__HAL_DMA_DISABLE_IT(p->husart_x->hdmarx, DMA_IT_TC);
	__HAL_DMA_CLEAR_FLAG(p->husart_x->hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(p->husart_x->hdmarx));
	SET_BIT(p->husart_x->hdmarx->ErrorCode, HAL_DMA_ERROR_NONE);
	p->husart_x->hdmarx->State = HAL_DMA_STATE_READY;
	__HAL_UNLOCK(p->husart_x->hdmarx);
	p->husart_x->RxXferSize = (uint16_t)p->Num_packets_get_prev;

#elif defined(F4XX)

	if(!(((p->husart_x->hdmarx->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET) )
	{
		if((p->husart_x->hdmarx->Instance->CR & DMA_SxCR_CIRC) == RESET)
		{
			p->husart_x->hdmarx->Instance->CR  &= ~(DMA_IT_TC);
			__HAL_DMA_DISABLE(p->husart_x->hdmarx);								//ׁבנמס פכאדא נאבמעû dma
			__HAL_UNLOCK(p->husart_x->hdmarx);
			p->husart_x->hdmarx->State = HAL_DMA_STATE_READY;
		}
	}

#endif
	p->husart_x->RxXferSize = (uint16_t)p->Num_packets_get_prev;

	if (p->husart_x->hdmarx->XferCpltCallback != NULL) BD_USART_Reset_Rx(p->husart_x->hdmarx, Callback_call);
	if (!Callback_call) HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
