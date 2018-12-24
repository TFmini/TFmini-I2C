/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_Usart.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-9-11
Description: 
				User usart Configuration
Others: 		
Function List: 
1. void fail(void)
2. void User_Usart1_Init(void)
3. void HAL_UART_IDLECallback(UART_HandleTypeDef *huart)
History: 
	V1.0 2018-9-11 wuxiuhua Create C file
*************************************************/

#include "User_Usart.h"
#include "usart.h"


extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

__IO uint8_t  g_Usart1_TxBuffer[USART_BUF_SIZE];
__IO uint8_t  g_Usart1_RxBuffer[USART_BUF_SIZE];
__IO uint32_t g_Usart1_RxBytes = 0;
uint8_t  g_Usart1_ProcBuffer[USART_BUF_SIZE];
uint32_t g_Usart1_ProcBytes = 0;

/*************************************************
Function: fail
Description: fail proceess 
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void fail(void)
{
	while(1)
	{
	}
}


/*************************************************
Function: User_Usart1_Init
Description: config parameter Flash written Verify
Input:  flashAddress - the parameter's flash address
		targetConfig - the target value
Output: None
Return: None
Others: None
*************************************************/
void User_Usart1_Init(void)
{
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)g_Usart1_RxBuffer, USART_BUF_SIZE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_ENABLE(&huart1);
}

/*************************************************
Function: UsartSaveRxDataToProcBuffer
Description: Save the data to buffer
Input:  
		rxbuf
		rxlen
		Procbuf
		Proclen
Output: None
Return: None
Others: None
*************************************************/
static void UsartSaveRxDataToProcBuffer(uint8_t *rxbuf, uint32_t rxlen, uint8_t *Procbuf, uint32_t *Proclen)
{	
	uint32_t i = 0;
	
	*Proclen = rxlen;
	
	for(i = 0; i < rxlen; i++)
	{
		Procbuf[i] = rxbuf[i];
	}
}
/*************************************************
Function: HAL_UART_IDLECallback
Description: HAL_UART_IDLECallback
Input:  huart - ptr to usart
Output: None
Return: None
Others: None
*************************************************/
 void HAL_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	uint32_t clearStatus = 0;
	// huart1 is the COM Port For PC
	if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);

		clearStatus = huart1.Instance->SR;
		clearStatus = huart1.Instance->DR;

		__HAL_DMA_DISABLE(huart->hdmarx);
		g_Usart1_RxBytes = USART_BUF_SIZE - hdma_usart1_rx.Instance->CNDTR;
		UsartSaveRxDataToProcBuffer((uint8_t *)g_Usart1_RxBuffer, g_Usart1_RxBytes, g_Usart1_ProcBuffer, &g_Usart1_ProcBytes);
		hdma_usart1_rx.Instance->CNDTR = USART_BUF_SIZE;
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
}



