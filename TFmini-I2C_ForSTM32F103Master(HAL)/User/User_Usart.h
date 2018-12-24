#ifndef User_Usart_H
#define User_Usart_H

#include "stm32f1xx.h"

#define USART_BUF_SIZE 		64

extern uint8_t  g_Usart1_ProcBuffer[USART_BUF_SIZE];
extern uint32_t g_Usart1_ProcBytes;
/*************************************************
Function: fail
Description: fail proceess 
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void fail(void);

/*************************************************
Function: User_Usart1_Init
Description: config parameter Flash written Verify
Input:  flashAddress - the parameter's flash address
		targetConfig - the target value
Output: None
Return: the status of Verify
Others: None
*************************************************/
void User_Usart1_Init(void);

/*************************************************
Function: HAL_UART_IDLECallback
Description: HAL_UART_IDLECallback
Input:  huart - ptr to usart
Output: None
Return: None
Others: None
*************************************************/
 void HAL_UART_IDLECallback(UART_HandleTypeDef *huart);
#endif
