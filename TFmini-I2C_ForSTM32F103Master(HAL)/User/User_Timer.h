#ifndef User_Timer_H
#define User_Timer_H

#include "stm32f1xx.h"

struct TFminiControl
{
	uint8_t ControlFlag;
	uint8_t SlaveNum;
	uint8_t SlaveAddr[8];
};

extern struct TFminiControl TFminiStruct;
/*************************************************
Function: User_Timer1_Init
Description: Timer1 Init 
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Timer1_Init(void);

/*************************************************
Function: User_Timer1_Disable
Description: Disable Timer1
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Timer1_Disable(void);

/*************************************************
Function: User_Timer1_Enable
Description: Enable Timer1
Input:  None
Output: None
Return: None
Others: None
*************************************************/
void User_Timer1_Enable(void);

/*************************************************
Function: ConfigTrigMessage
Description: Config Timer and TFminiStruct
Input:  on_off
		SlaveNum
		slaveAddrBuf - ptr to slave Address buffer
Output: None
Return: None
Others: None
*************************************************/
void User_ConfigTrigMessage(uint8_t on_off, uint8_t SlaveNum, uint8_t *slaveAddrBuf);

/*************************************************
Function: User_IsUsartCommandExist
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
void User_IsControlFlagExist(void);

/*************************************************
Function: User_StartTFMiniMeasure
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
void User_StartTFMiniMeasure(void);
#endif
