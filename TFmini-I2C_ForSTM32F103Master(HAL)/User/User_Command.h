
#ifndef User_Command_H
#define User_Command_H

#include "stm32f1xx.h"

#define COM_FH_0	0x42
#define COM_FH_1	0x57

#define COM_TrigDataControl			0x00
#define COM_ConfigSlaveAddress		0x01
#define COM_ConfigInttimeAndMode	0x02
#define COM_ConfigRangeModeAndValue	0x03
#define COM_ConfigStrengthLowThreshold 0x04
#define COM_ConfigUnit				0x05
#define COM_ConfigDefault			0x06
#define COM_ConfigOutput			0x07

typedef enum
{
  COM_OK       = 0x01U,
  COM_PARAS_ERROR = 0x02U,
  COM_FORMAT_ERROR = 0x03U,
  COM_ERROR    = 0x04U,
} COM_StatusTypeDef;

/*************************************************
Function: User_IsUsartCommandExist
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
void User_IsUsartCommandExist(void);
#endif
