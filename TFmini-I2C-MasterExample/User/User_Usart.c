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
__IO uint32_t g_Usart1_RxBytes;


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
Return: the status of Verify
Others: None
*************************************************/
void User_Usart1_Init(void)
{
	if(HAL_UART_Receive_DMA(&huart1, (uint8_t *)g_Usart1_RxBuffer, USART_BUF_SIZE) == HAL_OK)
	{
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	}
	else
	{
		printf("Err: User_Usart1_Init.\r\n");
		fail();
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

		HAL_UART_DMAStop(&huart1);
		g_Usart1_RxBytes = USART_BUF_SIZE - hdma_usart1_rx.Instance->CNDTR;
		HAL_UART_Receive_DMA(&huart1, (uint8_t *)g_Usart1_RxBuffer, USART_BUF_SIZE);
	}
}



