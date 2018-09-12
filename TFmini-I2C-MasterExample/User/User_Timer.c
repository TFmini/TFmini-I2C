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
Function: HAL_TIM_PeriodElapsedCallback
Description: HAL_TIM_PeriodElapsedCallback
Input:  None
Output: None
Return: None
Others: None
*************************************************/
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET)
	{
		
	}
}




