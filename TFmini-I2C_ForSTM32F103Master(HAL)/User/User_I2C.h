#ifndef User_I2C_H
#define User_I2C_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal_i2c.h"

/*************************************************
Function: User_I2C_ResetBus
Description: Reset I2C Bus
Input:  i2cHandle - ptr to I2C
Output: None
Return: None
Others: None
************************************************/
HAL_StatusTypeDef User_I2C_ResetBus(void);

/*************************************************
Function: User_I2C_WriteToSlave
Description: I2C write to slave
Input:  
		slaveAddr - the target slave address
		RegAddr - register's address
		len - the operation length 
		TxBuf - prt to tx buffer
Output: None
Return: status of written
Others: None
************************************************/
HAL_StatusTypeDef User_I2C_WriteToSlave(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len, uint8_t *TxBuf);

/*************************************************
Function: User_I2C_ReadFromSlave
Description: I2C read from slave
Input:  
		slaveAddr - the target slave address
		RegAddr - register's address
		len - the operation length 
		RxBuf - prt to rx buffer
Output: None
Return: status of read
Others: None
************************************************/
HAL_StatusTypeDef User_I2C_ReadFromSlave(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len, uint8_t *RxBuf);

/*************************************************
Function: User_I2C_GeneralCall
Description: I2C_GeneralCall
Input:  
		None
Output: None
Return: status of User_I2C_GeneralCall
Others: None
************************************************/
HAL_StatusTypeDef User_I2C_GeneralCall(void);

#endif
