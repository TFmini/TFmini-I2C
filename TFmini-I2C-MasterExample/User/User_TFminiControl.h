#ifndef User_TFminiControl_H
#define User_TFminiControl_H

#include "stm32f1xx.h"

typedef enum
{
  TFmini_OK       = 0x00U,
  TFmini_ERROR    = 0x01U,
} TFmini_StatusTypeDef;

#define TFmini_DistInformation_RegAddr	0x0102
#define TFmini_DistInformation_Bytes	0x07
#define TFmini_SlaveAddr_RegAddr		0x0026
#define TFmini_SlaveAddr_Bytes			0x01
#define TFmini_InttimeAndMode_RegAddr	0x0050
#define TFmini_InttimeAndMode_Bytes		0x02
#define TFmini_RangeModeAndValue_RegAddr	0x0055
#define TFmini_RangeModeAndValue_Bytes	0x03
#define TFmini_StrengthLowThreshold_RegAddr	0x0058
#define TFmini_StrengthLowThreshold_Bytes	0x02
#define TFmini_Unit_RegAddr	0x0066
#define TFmini_Unit_Bytes	0x01
#define TFmini_Default_RegAddr	0x0070
#define TFmini_Default_Value	0x02
#define TFmini_Default_Bytes	0x01

/*************************************************
Function: User_TFmini_GetDistInformation
Description: Get TFmini Dist Information
Input:  slaveAddr - tfmini I2C slave address
		DistBuf - Buf
Output: None
Return: the status of User_TFmini_GetDistInformation
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_GetDistInformation(uint8_t slaveAddr, uint8_t *DistBuf);

/*************************************************
Function: User_TFmini_ConfigSlaveAddress
Description: Configure TFmini Slave Address
Input:  slaveAddr - tfmini I2C slave address
		targetslaveAddr - the target I2C slave Address
Output: None
Return: the status of User_TFmini_ConfigSlaveAddress
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigSlaveAddress(uint8_t slaveAddr, uint8_t targetslaveAddr);

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
TFmini_StatusTypeDef User_TFmini_ConfigInttimeAndMode(uint8_t slaveAddr, uint8_t inttime, uint8_t mode);

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
TFmini_StatusTypeDef User_TFmini_ConfigRangeModeAndValue(uint8_t slaveAddr, uint8_t rangeMode, uint16_t rangeValue);


/*************************************************
Function: User_TFmini_ConfigStrengthLowThres
Description: Config TFmini strength Low Threshold
Input:  slaveAddr - tfmini I2C slave address
		strengthLowThreshold 
Output: None
Return: the status of User_TFmini_ConfigStrengthLowThres
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigStrengthLowThres(uint8_t slaveAddr, uint16_t strengthLowThreshold);


/*************************************************
Function: User_TFmini_ConfigUnit
Description: Config TFmini Unit
Input:  slaveAddr - tfmini I2C slave address
		Unit 
Output: None
Return: the status of User_TFmini_ConfigUnit
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigUnit(uint8_t slaveAddr, uint8_t Unit);

/*************************************************
Function: User_TFmini_ConfigDefault
Description: Config TFmini Unit
Input:  slaveAddr - tfmini I2C slave address
		Unit 
Output: None
Return: the status of User_TFmini_ConfigUnit
Others: None
*************************************************/
TFmini_StatusTypeDef User_TFmini_ConfigDefault(uint8_t slaveAddr);





#endif
