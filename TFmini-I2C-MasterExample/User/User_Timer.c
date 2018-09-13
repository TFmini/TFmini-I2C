/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_Timer.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-9-11
Description: 
				User Timer Configuration
Others: 		
Function List: 
1. void User_Timer1_Init(void)
2. void User_Timer1_Disable(void)
3. void User_Timer1_Enable(void)
4. void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
History: 
	V1.0 2018-9-11 wuxiuhua Create C file
*************************************************/

#include "stm32f1xx.h"
#include "User_Timer.h"
#include "tim.h"
#include "User_TFminiConfig.h"

struct TFminiControl TFminiStruct;

extern TIM_HandleTypeDef htim1;


/*************************************************
Function: User_Timer1_Init
Description: Timer1 Init 
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Timer1_Init(void)
{
	__HAL_TIM_ENABLE(&htim1);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&htim1);

}

/*************************************************
Function: User_Timer1_Disable
Description: Disable Timer1
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Timer1_Disable(void)
{
	__HAL_TIM_DISABLE(&htim1);
	__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
	HAL_TIM_Base_Stop_IT(&htim1);
}

/*************************************************
Function: User_Timer1_Enable
Description: Enable Timer1
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Timer1_Enable(void)
{
	User_Timer1_Init();
}

/*************************************************
Function: User_ConfigTrigMessage
Description: Config Timer and TFminiStruct
Input:  on_off
		SlaveNum
		slaveAddrBuf - ptr to slave Address buffer
Output: None
Return: None
Others: None
*************************************************/
void User_ConfigTrigMessage(uint8_t on_off, uint8_t SlaveNum, uint8_t *slaveAddrBuf)
{
	uint8_t i = 0;
	printf("On/OFF=%d,SlaveNum=%d\r\n", on_off, SlaveNum);
	if(on_off != 0x00)
	{
		TFminiStruct.SlaveNum = SlaveNum;
		for(i = 0; i < TFminiStruct.SlaveNum; i++)
		{
			TFminiStruct.SlaveAddr[i] = slaveAddrBuf[i];
		}
		User_Timer1_Enable();
	}
	else
	{
		User_Timer1_Disable();
	}
}

/*************************************************
Function: User_GetTFminiMeas
Description: Get TFmini Measure data
Input:  SlaveNum
		slaveAddrBuf - ptr to slave Address buffer
Output: None
Return: None
Others: None
*************************************************/
static void User_GetTFminiMeas(uint8_t SlaveNum, uint8_t *slaveAddrBuf)
{
	uint8_t i = 0;
	uint8_t rxbuf[7] = {0};
	TFmini_StatusTypeDef status;
	
	for(i = 0; i < SlaveNum; i++)
	{
		status = User_TFmini_GetDistInformation(slaveAddrBuf[i], rxbuf);
		printf("Slave 0x%2x: ", slaveAddrBuf[i]);
		if(status != TFmini_OK)
		{
			printf("Error. ");
		}
		else
		{
			printf("Dist[%5d], ", rxbuf[2] | (rxbuf[3] << 8));
			printf("Strength[%5d], ", rxbuf[4] | (rxbuf[5] << 8));
			printf("Mode[%d]. ", rxbuf[6]);
		}
	}
	printf("\r\n");
}


/*************************************************
Function: User_IsUsartCommandExist
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
void User_IsControlFlagExist(void)
{
	if(TFminiStruct.ControlFlag != 0)
	{
		User_GetTFminiMeas(TFminiStruct.SlaveNum, TFminiStruct.SlaveAddr);
		TFminiStruct.ControlFlag = 0;
	}
}
/*************************************************
Function: HAL_TIM_PeriodElapsedCallback
Description: HAL_TIM_PeriodElapsedCallback
Input:  None
Output: None
Return: None
Others: None
*************************************************/
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TFminiStruct.ControlFlag = 0x01;
}




