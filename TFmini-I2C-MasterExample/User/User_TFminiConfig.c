/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_TFminiControl.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-9-12
Description: 
				User TFmini Configuration
Others: 		
Function List: 
1. TFmini_StatusTypeDef User_TFmini_GetDistInformation(uint8_t slaveAddr, uint8_t *DistBuf)
2. TFmini_StatusTypeDef User_TFmini_ConfigSlaveAddress(uint8_t slaveAddr, uint8_t targetslaveAddr)
3. TFmini_StatusTypeDef User_TFmini_ConfigInttimeAndMode(uint8_t slaveAddr, uint8_t inttime, uint8_t mode)
4. TFmini_StatusTypeDef User_TFmini_ConfigStrengthLowThres(uint8_t slaveAddr, uint16_t strengthLowThreshold)
History: 
	V1.0 2018-9-12 wuxiuhua Create C file
*************************************************/

#include "User_TFminiConfig.h"
#include "User_I2C.h"


/*************************************************
Function: User_TFmini_GetDistInformation
Description: Get TFmini Dist Information
Input:  slaveAddr - tfmini I2C slave address
		DistBuf - Buf
Output: None
Return: the status of User_TFmini_GetDistInformation
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_GetDistInformation(uint8_t slaveAddr, uint8_t *DistBuf)
{
	HAL_StatusTypeDef i2c_status = HAL_ERROR;
	
	i2c_status = User_I2C_ReadFromSlave(slaveAddr, TFmini_DistInformation_RegAddr, TFmini_DistInformation_Bytes, DistBuf);

	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}


/*************************************************
Function: User_TFmini_ConfigSlaveAddress
Description: Configure TFmini Slave Address
Input:  slaveAddr - tfmini I2C slave address
		targetslaveAddr - the target I2C slave Address
Output: None
Return: the status of User_TFmini_ConfigSlaveAddress
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigSlaveAddress(uint8_t slaveAddr, uint8_t targetslaveAddr)
{
	uint8_t txbuf[1] = {0};
	HAL_StatusTypeDef i2c_status = HAL_ERROR;

	txbuf[0] = targetslaveAddr;
	i2c_status = User_I2C_WriteToSlave(slaveAddr, TFmini_SlaveAddr_RegAddr, TFmini_SlaveAddr_Bytes, txbuf);

	if(i2c_status != HAL_OK)
	{
		return TFmini_ERROR;
	}
	else
	{
		HAL_Delay(500);
		i2c_status = User_I2C_GeneralCall();
		if(i2c_status == HAL_OK)
		{
			HAL_Delay(1000);
		}
	}
	
	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}


/*************************************************
Function: User_TFmini_ConfigInttimeAndMode
Description: Config TFmini inttime and mode
Input:  slaveAddr - tfmini I2C slave address
		inttime - inttime
		mode 
Output: None
Return: the status of User_TFmini_ConfigInttimeAndMode
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigInttimeAndMode(uint8_t slaveAddr, uint8_t inttime, uint8_t mode)
{
	uint8_t txbuf[TFmini_InttimeAndMode_Bytes] = {0};
	HAL_StatusTypeDef i2c_status = HAL_ERROR;

	txbuf[0] = inttime;
	txbuf[1] = mode;
	i2c_status = User_I2C_WriteToSlave(slaveAddr, TFmini_InttimeAndMode_RegAddr, TFmini_InttimeAndMode_Bytes, txbuf);

	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}


/*************************************************
Function: User_TFmini_ConfigRangeModeAndValue
Description: Config TFmini range mode and value
Input:  slaveAddr - tfmini I2C slave address
		rangeMode
		rangeValue 
Output: None
Return: the status of User_TFmini_ConfigRangeModeAndValue
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigRangeModeAndValue(uint8_t slaveAddr, uint8_t rangeMode, uint16_t rangeValue)
{
	uint8_t txbuf[TFmini_RangeModeAndValue_Bytes] = {0};
	HAL_StatusTypeDef i2c_status = HAL_ERROR;

	txbuf[0] = rangeMode;
	txbuf[1] = rangeValue & 0xFF;
	txbuf[2] = (rangeValue >> 8)& 0xFF;
	i2c_status = User_I2C_WriteToSlave(slaveAddr, TFmini_RangeModeAndValue_RegAddr, TFmini_RangeModeAndValue_Bytes, txbuf);

	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}


/*************************************************
Function: User_TFmini_ConfigStrengthLowThres
Description: Config TFmini strength Low Threshold
Input:  slaveAddr - tfmini I2C slave address
		strengthLowThreshold 
Output: None
Return: the status of User_TFmini_ConfigStrengthLowThres
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigStrengthLowThres(uint8_t slaveAddr, uint16_t strengthLowThreshold)
{
	uint8_t txbuf[TFmini_StrengthLowThreshold_Bytes] = {0};
	HAL_StatusTypeDef i2c_status = HAL_ERROR;

	txbuf[0] = strengthLowThreshold & 0xFF;
	txbuf[1] = (strengthLowThreshold >> 8)& 0xFF;
	i2c_status = User_I2C_WriteToSlave(slaveAddr, TFmini_StrengthLowThreshold_RegAddr, TFmini_StrengthLowThreshold_Bytes, txbuf);

	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}


/*************************************************
Function: User_TFmini_ConfigUnit
Description: Config TFmini Unit
Input:  slaveAddr - tfmini I2C slave address
		Unit 
Output: None
Return: the status of User_TFmini_ConfigUnit
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigUnit(uint8_t slaveAddr, uint8_t Unit)
{
	uint8_t txbuf[TFmini_Unit_Bytes] = {0};
	HAL_StatusTypeDef i2c_status = HAL_ERROR;

	txbuf[0] = Unit;
	i2c_status = User_I2C_WriteToSlave(slaveAddr, TFmini_Unit_RegAddr, TFmini_Unit_Bytes, txbuf);

	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}


/*************************************************
Function: User_TFmini_ConfigDefault
Description: Config TFmini Unit
Input:  slaveAddr - tfmini I2C slave address
Output: None
Return: the status of User_TFmini_ConfigUnit
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigDefault(uint8_t slaveAddr)
{
	uint8_t txbuf[TFmini_Default_Bytes] = {0};
	HAL_StatusTypeDef i2c_status = HAL_ERROR;

	txbuf[0] = TFmini_Default_Value;
	i2c_status = User_I2C_WriteToSlave(slaveAddr, TFmini_Default_RegAddr, TFmini_Default_Bytes, txbuf);

	return (i2c_status != HAL_OK) ? TFmini_ERROR : TFmini_OK;
}

/*************************************************
Function: User_TFmini_ReadAllConfig
Description: Read All Config
Input:  slaveAddr - tfmini I2C slave address
Output: None
Return: None
Others: Config message
*************************************************/
void User_TFmini_ReadAllConfig(uint8_t slaveAddr)
{
	HAL_StatusTypeDef status;
	uint8_t rxbuf[5] = {0};
	
	status = User_I2C_ReadFromSlave(slaveAddr, TFmini_InttimeAndMode_RegAddr, TFmini_InttimeAndMode_Bytes, rxbuf);
	if(status != HAL_OK)
	{
		printf("Err: Read Inttime and mode, status=%d\r\n", status);
		return;
	}
	printf("\r\nInttime = %d, Mode=%d\r\n", rxbuf[0], rxbuf[1]);
	
	status = User_I2C_ReadFromSlave(slaveAddr, TFmini_RangeModeAndValue_RegAddr, TFmini_RangeModeAndValue_Bytes, rxbuf);
	if(status != HAL_OK)
	{
		printf("Err: Read Range Mode and Value, status = %d\r\n", status);
		return;
	}
	printf("RangeMode = %d, RangeValue = %d\r\n", rxbuf[0], rxbuf[1]|(rxbuf[2] << 8));
	
	status = User_I2C_ReadFromSlave(slaveAddr, TFmini_StrengthLowThreshold_RegAddr, TFmini_StrengthLowThreshold_Bytes, rxbuf);
	if(status != HAL_OK)
	{
		printf("Err: Read Strength Low Threshold, status = %d\r\n", status);
		return;
	}
	printf("Strength Low Threshold = %d\r\n", rxbuf[0]|(rxbuf[1] << 8));
	
	status = User_I2C_ReadFromSlave(slaveAddr, TFmini_Unit_RegAddr, TFmini_Unit_Bytes, rxbuf);
	if(status != HAL_OK)
	{
		printf("Err: Read Unit, status = %d\r\n", status);
		return;
	}
	printf("Unit = %d\r\n", rxbuf[0]|(rxbuf[1] << 8));
}
