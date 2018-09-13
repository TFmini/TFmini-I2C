/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_Command.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-9-12
Description: 
				User Command Process
Others: 		
Function List: 
1. void fail(void)
2. void User_Usart1_Init(void)
3. void HAL_UART_IDLECallback(UART_HandleTypeDef *huart)
History: 
	V1.0 2018-9-11 wuxiuhua Create C file
*************************************************/
#include "User_Command.h"
#include "User_Usart.h"
#include "usart.h"
#include "stm32f1xx_hal_uart.h"
#include "User_Timer.h"
#include "User_TFminiConfig.h"
extern __IO uint8_t  g_Usart1_TxBuffer[USART_BUF_SIZE];
extern __IO uint8_t  g_Usart1_RxBuffer[USART_BUF_SIZE];
extern __IO uint32_t g_Usart1_RxBytes;


/*************************************************
Function: User_CommandFHVerify
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
static COM_StatusTypeDef User_CommandFormatVerify(__IO uint8_t *rxbuf)
{
	uint8_t chk = 0, i = 0;
	
	// Check Frame Head
	if((COM_FH_0 != rxbuf[0])&&(COM_FH_1 != rxbuf[1]))
	{
		return COM_FORMAT_ERROR;
	}
	
	switch(rxbuf[2])
	{
		case COM_TrigDataControl:
		{
			// I2C bus can't exist more than 8 TFmini
			if(rxbuf[4] > 8)
			{
				return COM_PARAS_ERROR;
			}
			
			// Check chk
			for(i = 0,chk = 0; i < (5 + rxbuf[4]); i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[rxbuf[4]+5] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Addr range [0x78, 0x07]
			for(i = 0; i < rxbuf[4]; i++)
			{
				if((rxbuf[5+i] > 0x78) || (rxbuf[5+i] < 0x07))
				{
					return COM_PARAS_ERROR;
				}
			}
		}
		break;
		case COM_ConfigSlaveAddress:
		{
			// Check chk
			for(i = 0,chk = 0; i < 5; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[5] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Original Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
			// Check Target Address Range
			if((rxbuf[4] > 0x78) || (rxbuf[4] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
		}
		break;
		case COM_ConfigInttimeAndMode:
		{
			// Check chk
			for(i = 0,chk = 0; i < 6; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[6] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
			
			// Check Inttime Range
			if((rxbuf[4]!=0x00) || (rxbuf[4]!=0x03)||(rxbuf[4]!=0x07))
			{
				return COM_PARAS_ERROR;
			}
		}
		break;
		case COM_ConfigRangeModeAndValue:
		{
			// Check chk
			for(i = 0,chk = 0; i < 7; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[7] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
		}
		break;
		case COM_ConfigStrengthLowThreshold:
		{
			// Check chk
			for(i = 0,chk = 0; i < 6; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[6] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
		}
		break;
		case COM_ConfigUnit:
		{
			// Check chk
			for(i = 0,chk = 0; i < 5; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[5] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
		}
		break;
		case COM_ConfigDefault:
		{
			// Check chk
			for(i = 0,chk = 0; i < 5; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[5] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
			
			// Check Config Range
			if(rxbuf[3] != 0x02)
			{
				return COM_PARAS_ERROR;
			}
		}
		break;
		case COM_ConfigOutput:
		{
			// Check chk
			for(i = 0,chk = 0; i < 4; i++)
			{
				chk += rxbuf[i];
			}
			
			if(rxbuf[5] != chk)
			{
				return COM_FORMAT_ERROR;
			}
			
			// Check Slave Address Range
			if((rxbuf[3] > 0x78) || (rxbuf[3] < 0x07))
			{
				return COM_PARAS_ERROR;
			}
		}
		default:
		{
			return COM_FORMAT_ERROR;
		}
	}
	return COM_OK;
}

/*************************************************
Function: User_CommandSendBack
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
static void User_CommandSendBack(COM_StatusTypeDef procStatus, uint8_t com_code)
{
	static uint8_t buf[5] = {0x42, 0x57};
	int i = 0;

	buf[2] = com_code;
	buf[3] = (uint8_t)procStatus;
	
	for(i = 0, buf[4] = 0; i < 4; i++)
	{
		buf[4]+= buf[i];
	}
	
	HAL_UART_Transmit(&huart1, buf, 5, 10);
	
}
/*************************************************
Function: User_CommandProc
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
static void User_CommandProc(__IO uint8_t *rxbuf)
{
	COM_StatusTypeDef status = User_CommandFormatVerify(rxbuf);
		
	if(status == COM_OK)
	{
		switch(rxbuf[2])
		{
			case COM_TrigDataControl:
			{
				User_ConfigTrigMessage(rxbuf[3], rxbuf[4], (uint8_t *)&rxbuf[5]);
				status = COM_OK;
			}
			break;
			case COM_ConfigSlaveAddress:
			{
				status = (User_TFmini_ConfigSlaveAddress(rxbuf[3], rxbuf[4]) != TFmini_OK) ? COM_ERROR : COM_OK;
			}
			break;
			case COM_ConfigInttimeAndMode:
			{
				status = (User_TFmini_ConfigInttimeAndMode(rxbuf[3], rxbuf[4], rxbuf[5]) != TFmini_OK) ? COM_ERROR : COM_OK;
			}
			break;
			case COM_ConfigRangeModeAndValue:
			{
				status = (User_TFmini_ConfigRangeModeAndValue(rxbuf[3], rxbuf[4], rxbuf[5]|(rxbuf[6]<<8)) != TFmini_OK) ? COM_ERROR : COM_OK;
			}
			break;
			case COM_ConfigStrengthLowThreshold:
			{
				status = (User_TFmini_ConfigStrengthLowThres(rxbuf[3], rxbuf[4]|(rxbuf[5]<<8)) != TFmini_OK) ? COM_ERROR : COM_OK;
			}
			break;
			case COM_ConfigUnit:
			{
				status = (User_TFmini_ConfigUnit(rxbuf[3], rxbuf[4]) != TFmini_OK) ? COM_ERROR : COM_OK;
			}
			break;
			case COM_ConfigDefault:
			{
				status = (User_TFmini_ConfigDefault(rxbuf[3]) != TFmini_OK) ? COM_ERROR : COM_OK;
			}
			break;
			case COM_ConfigOutput:
			{
				User_TFmini_ReadAllConfig(rxbuf[3]);
				status = COM_OK;
			}
			default:
				status = COM_ERROR;
			break;
		}
	}
	User_CommandSendBack(status, rxbuf[2]);
}

/*************************************************
Function: User_IsUsartCommandExist
Description: 
Input:  
Output: None
Return: None
Others: None
*************************************************/
void User_IsUsartCommandExist(void)
{
	if(g_Usart1_RxBytes != 0)
	{
		User_CommandProc(g_Usart1_RxBuffer);
		g_Usart1_RxBytes = 0;
	}
}
