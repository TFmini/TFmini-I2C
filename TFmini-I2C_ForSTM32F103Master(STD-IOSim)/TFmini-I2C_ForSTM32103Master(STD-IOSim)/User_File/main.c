/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	main.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-12-24
Description: 
				main.c
Others: 		
Function List: 

History: 
	V1.0 2018-12-24 wuxiuhua Create C file
*************************************************/
#include "stm32f10x.h"
#include "User_usart.h"
#include "User_I2C.h"
#include <stdlib.h>



int main(void)
{  
	uint8_t buf[7];
	__I2C_Status_TypeDef i2CStatus;
	int16_t Dist = 0;
	uint16_t Strength = 0;
	uint8_t Mode = 0;
	
	/* System Init */      
	SystemInit();

	/* Peripheral initialization */
	User_Usart_Init();

	User_I2C_Init();
	delay(10000);
	
	printf("Into while.\r\n");
	
	while (1)
	{ 
		i2CStatus = I2C_RecvBytes(0x10, 0x102, buf, 7, 100);
		if(i2CStatus == I2C_OK)
		{
			Mode = buf[6];
			Dist = buf[2] | (buf[3] << 8);
			Strength = buf[4] | (buf[5] << 8);
			
			printf("Slave 0x%x: Dist[%5d], Strength[%5d], Mode[%d].\r\n", \
					0x10, Dist, Strength, Mode);
		}
		else
		{
			printf("Err: I2C_RecvBytes.\r\n");
		}
		delay(10000);
	}
}

/******************* (C) COPYRIGHT 2018 chu  *****END OF FILE************/
