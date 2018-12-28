/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_Usart.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-12-24
Description: 
				User usart Configuration
Others: 		
Function List: 
1. static void User_Usart_IO_Init(void)
2. void User_Usart_Init(void)
3. int fputc(int ch, FILE *f)
History: 
	V1.0 2018-12-24 wuxiuhua Create C file
*************************************************/
#include "User_usart.h"
#include <stdarg.h>


/*************************************************
Function: User_Usart_IO_Init
Description: config Usart1 IO
Input:  None
Output: None
Return: None
Others: None
*************************************************/
static void User_Usart_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*************************************************
Function: User_Usart_Init
Description: config Usart1
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Usart_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* config USART1 IO */
	User_Usart_IO_Init();
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	USART_Cmd(USART1, ENABLE);
}


/*************************************************
Function: fputc
Description: 
Input:  None
Output: None
Return: None
Others: None
*************************************************/
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));
	
	return (ch);
}



