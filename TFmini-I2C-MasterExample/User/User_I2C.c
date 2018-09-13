/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_I2C.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-9-12
Description: 
				User I2C Configuration
Others: 		
Function List: 
1. HAL_StatusTypeDef User_I2C_ResetBus(void)
2. HAL_StatusTypeDef User_I2C_WriteToSlave(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len, uint8_t *TxBuf)
3. User_I2C_ReadFromSlave(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len, uint8_t *RxBuf)
History: 
	V1.0 2018-9-12 wuxiuhua Create C file
*************************************************/
#include "User_I2C.h"
#include "i2c.h"

#define USER_I2C_Modify_Flag			1

#define USER_I2C						hi2c1
#define USER_I2C_SCL_Pin				GPIO_PIN_6
#define USER_I2C_SDA_Pin				GPIO_PIN_7
#define USER_I2C_GPIO					GPIOB
#define USER_I2C_WriteBuf_Bytes			255
#define USER_I2C_ReadBuf_Bytes			255
#define USER_I2C_DummyWrite_Bytes		3
#define USER_I2C_DummyWrite_Timeout		5
#define USER_I2C_Transmit_Timeout		10
#define USER_I2C_Receive_Timeout		10

#define USER_I2C_SlaveAddrToSTM32(addr)		(addr<<1)
#define USER_I2C_Set_IO()			HAL_GPIO_WritePin(GPIOB, USER_I2C_SCL_Pin | USER_I2C_SDA_Pin, GPIO_PIN_SET)
#define USER_I2C_Reset_IO()			HAL_GPIO_WritePin(GPIOB, USER_I2C_SCL_Pin | USER_I2C_SDA_Pin, GPIO_PIN_RESET)
__IO uint8_t g_I2C_TxBuf[USER_I2C_WriteBuf_Bytes] = {0};
__IO uint8_t g_I2C_RxBuf[USER_I2C_ReadBuf_Bytes] = {0};



/**************************************************************
 The following code is copied from the HAL Library and
 makes some changes
***************************************************************/ 
#define I2C_TIMEOUT_FLAG          35U         /*!< Timeout 35 ms             */
#define I2C_TIMEOUT_BUSY_FLAG     25U         /*!< Timeout 25 ms             */
#define I2C_NO_OPTION_FRAME       0xFFFF0000U /*!< XferOptions default value */

/* Private define for @ref PreviousState usage */
#define I2C_STATE_MSK             ((uint32_t)((HAL_I2C_STATE_BUSY_TX | HAL_I2C_STATE_BUSY_RX) & (~(uint32_t)HAL_I2C_STATE_READY))) /*!< Mask State define, keep only RX and TX bits            */
#define I2C_STATE_NONE            ((uint32_t)(HAL_I2C_MODE_NONE))                                                        /*!< Default Value                                          */
#define I2C_STATE_MASTER_BUSY_TX  ((uint32_t)((HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | HAL_I2C_MODE_MASTER))            /*!< Master Busy TX, combinaison of State LSB and Mode enum */
#define I2C_STATE_MASTER_BUSY_RX  ((uint32_t)((HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | HAL_I2C_MODE_MASTER))            /*!< Master Busy RX, combinaison of State LSB and Mode enum */
#define I2C_STATE_SLAVE_BUSY_TX   ((uint32_t)((HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | HAL_I2C_MODE_SLAVE))             /*!< Slave Busy TX, combinaison of State LSB and Mode enum  */
#define I2C_STATE_SLAVE_BUSY_RX   ((uint32_t)((HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | HAL_I2C_MODE_SLAVE))             /*!< Slave Busy RX, combinaison of State LSB and Mode enum  */

/**
  * @brief  This function handles Acknowledge failed detection during an I2C Communication.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c)
{
  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Clear NACKF Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    hi2c->ErrorCode = HAL_I2C_ERROR_AF;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->State= HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for Master addressing phase.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart)
{
  while(__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
    {
      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;

      /* Clear AF Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      hi2c->ErrorCode = HAL_I2C_ERROR_AF;
      hi2c->PreviousState = I2C_STATE_NONE;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}



/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Status The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait until flag is set */
  while((__HAL_I2C_GET_FLAG(hi2c, Flag) ? SET : RESET) == Status) 
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  return HAL_OK;
}


/**
  * @brief  This function handles I2C Communication Timeout for specific usage of TXE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{    
  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c) != HAL_OK)
    {
      return HAL_ERROR;
    }
		
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U) || ((HAL_GetTick()-Tickstart) > Timeout))
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;      
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of BTF flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnBTFFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{  
  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U) || ((HAL_GetTick()-Tickstart) > Timeout))
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}


/**
  * @brief  This function handles I2C Communication Timeout for specific usage of RXNE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{  

  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET)
  {
    /* Check if a STOPF is detected */
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
    {
      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
      hi2c->PreviousState = I2C_STATE_NONE;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if((Timeout == 0U) || ((HAL_GetTick()-Tickstart) > Timeout))
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_TIMEOUT;
    }
  }
  return HAL_OK;
}


/**
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
  uint32_t CurrentXferOptions = hi2c->XferOptions;

  /* Generate Start condition if first transfer */
  if((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME) || (CurrentXferOptions == I2C_NO_OPTION_FRAME))
  {
    /* Generate Start */
    hi2c->Instance->CR1 |= I2C_CR1_START;
  }
  else if(hi2c->PreviousState == I2C_STATE_MASTER_BUSY_RX)
  {
    /* Generate ReStart */
    hi2c->Instance->CR1 |= I2C_CR1_START;
  }

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout, Tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send slave address */
    hi2c->Instance->DR = I2C_10BIT_ADDRESS(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef User_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0x00U;

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
#ifndef USER_I2C_Modify_Flag
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
    {
      return HAL_BUSY;
    }
#endif

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Check if the I2C is already enabled */
    if((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      __HAL_I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    hi2c->State     = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode      = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferSize    = hi2c->XferCount;

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    while(hi2c->XferSize > 0U)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Write data to DR */
      hi2c->Instance->DR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
      hi2c->XferSize--;

      if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (hi2c->XferSize != 0U))
      {
        /* Write data to DR */
        hi2c->Instance->DR = (*hi2c->pBuffPtr++);
        hi2c->XferCount--;
        hi2c->XferSize--;
      }
      
      /* Wait until BTF flag is set */
      if(I2C_WaitOnBTFFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }
    }

    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    
    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Master sends target device address for read request.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
  uint32_t CurrentXferOptions = hi2c->XferOptions;

  /* Enable Acknowledge */
  hi2c->Instance->CR1 |= I2C_CR1_ACK;

  /* Generate Start condition if first transfer */
  if((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME)  || (CurrentXferOptions == I2C_NO_OPTION_FRAME))
  {
    /* Generate Start */
    hi2c->Instance->CR1 |= I2C_CR1_START;
  }
  else if(hi2c->PreviousState == I2C_STATE_MASTER_BUSY_TX)
  {
    /* Generate ReStart */
    hi2c->Instance->CR1 |= I2C_CR1_START;
  }

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = I2C_7BIT_ADD_READ(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout, Tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send slave address */
    hi2c->Instance->DR = I2C_10BIT_ADDRESS(DevAddress);

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Generate Restart */
    hi2c->Instance->CR1 |= I2C_CR1_START;

    /* Wait until SB flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_READ(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Receives in master mode an amount of data in blocking mode. 
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef User_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0x00U;

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
  #ifndef USER_I2C_Modify_Flag
    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
    {
      return HAL_BUSY;
    }
 #endif
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Check if the I2C is already enabled */
    if((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      __HAL_I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferSize    = hi2c->XferCount;

    /* Send Slave Address */
    if(I2C_MasterRequestRead(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(hi2c->XferSize == 0U)
    {
      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
    }
    else if(hi2c->XferSize == 1U)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
      software sequence must complete before the current byte end of transfer */
      __disable_irq();

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;

      /* Re-enable IRQs */
      __enable_irq(); 
    }
    else if(hi2c->XferSize == 2U)
    {
      /* Enable Pos */
      hi2c->Instance->CR1 |= I2C_CR1_POS;

      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
      software sequence must complete before the current byte end of transfer */
      __disable_irq();

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Re-enable IRQs */
      __enable_irq(); 
    }
    else
    {
      /* Enable Acknowledge */
      hi2c->Instance->CR1 |= I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while(hi2c->XferSize > 0U)
    {
      if(hi2c->XferSize <= 3U)
      {
        /* One byte */
        if(hi2c->XferSize == 1U)
        {
          /* Wait until RXNE flag is set */
          if(I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)      
          {
            if(hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
            {
              return HAL_TIMEOUT;
            }
            else
            {
              return HAL_ERROR;
            }
          }

          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
        /* Two bytes */
        else if(hi2c->XferSize == 2U)
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
             software sequence must complete before the current byte end of transfer */
           __disable_irq();

          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;

          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Re-enable IRQs */
          __enable_irq();

          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Disable Acknowledge */
          hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
             software sequence must complete before the current byte end of transfer */
          __disable_irq();

          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;

          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Re-enable IRQs */
          __enable_irq(); 

          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if(I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)      
        {
          if(hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
          {
            return HAL_TIMEOUT;
          }
          else
          {
            return HAL_ERROR;
          }
        }

        /* Read data from DR */
        (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
        hi2c->XferSize--;
        hi2c->XferCount--;

        if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
      }
    }

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef User_I2C_Master_DummyWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0x00U;

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Check if the I2C is already enabled */
    if((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      __HAL_I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    hi2c->State     = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode      = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;
    hi2c->XferSize    = hi2c->XferCount;

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    while(hi2c->XferSize > 0U)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Write data to DR */
      hi2c->Instance->DR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
      hi2c->XferSize--;

      if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (hi2c->XferSize != 0U))
      {
        /* Write data to DR */
        hi2c->Instance->DR = (*hi2c->pBuffPtr++);
        hi2c->XferCount--;
        hi2c->XferSize--;
      }
      
      /* Wait until BTF flag is set */
      if(I2C_WaitOnBTFFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }
    }
#ifndef USER_I2C_Modify_Flag
    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;
#endif
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    
    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**************************************************************
 The upper code is copied from the HAL Library and
 makes some changes
***************************************************************/ 


/*************************************************
Function: User_I2C_GeneralPurposeOutput_Init
Description: Set I2C1 IO as General Purpose Output
Input:  None
Output: None
Return: None
Others: None
*************************************************/
static void User_I2C_GeneralPurposeOutput_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = USER_I2C_SCL_Pin|USER_I2C_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USER_I2C_GPIO, &GPIO_InitStruct);
}


/*************************************************
Function: User_I2C_AlternateFunction_Init
Description: Set I2C IO as Alternate Function Output
Input:  None
Output: None
Return: None
Others: None
************************************************/
static void User_I2C_AlternateFunction_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = USER_I2C_SCL_Pin|USER_I2C_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USER_I2C_GPIO, &GPIO_InitStruct);
}


/*************************************************
Function: User_I2C_ResetBus
Description: Reset I2C Bus
Input:  i2cHandle - ptr to I2C
Output: None
Return: None
Others: None
************************************************/
HAL_StatusTypeDef User_I2C_ResetBus(void)
{
	/* 1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register */
	__HAL_I2C_DISABLE(&USER_I2C);
	HAL_GPIO_DeInit(USER_I2C_GPIO, USER_I2C_SCL_Pin|USER_I2C_SDA_Pin);
	
	/* 2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR) */
	User_I2C_GeneralPurposeOutput_Init();
	HAL_Delay(1);
	USER_I2C_Set_IO();
	HAL_Delay(1);
	
	/* 3. Check SCL and SDA High level in GPIOx_IDR */
	if ((HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SCL_Pin) != GPIO_PIN_SET)||(HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SDA_Pin) != GPIO_PIN_SET))
	{
#ifdef I2C_TEST	
		printf("3.SCL=%d, SDA=%d\r\n", HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SCL_Pin), HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SDA_Pin));
#endif
		return HAL_ERROR;
	}
	
	/* 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
	 * 5. Check SDA Low level in GPIOx_IDR.
	 * 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR)
	 * 7. Check SCL Low level in GPIOx_IDR.
	 * */
	USER_I2C_Reset_IO();
	HAL_Delay(1);
	if ((HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SCL_Pin) != GPIO_PIN_RESET)||(HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SDA_Pin) != GPIO_PIN_RESET))
	{
#ifdef I2C_TEST	
		printf("4-7.SCL=%d, SDA=%d\r\n", HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SCL_Pin), HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SDA_Pin));
#endif
		return HAL_ERROR;
	}
	
	/*
	 * 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	 * 9. Check SCL High level in GPIOx_IDR.
	 * 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
	 * 11. Check SDA High level in GPIOx_IDR.
	 */
	USER_I2C_Set_IO();
	HAL_Delay(1);
	if ((HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SCL_Pin) != GPIO_PIN_SET)||(HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SDA_Pin) != GPIO_PIN_SET))
	{
#ifdef I2C_TEST	
		printf("8-11.SCL=%d, SDA=%d\r\n", HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SCL_Pin), HAL_GPIO_ReadPin(USER_I2C_GPIO, USER_I2C_SDA_Pin));
#endif
		return HAL_ERROR;
	}
	
	/* 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain. */
	HAL_GPIO_DeInit(USER_I2C_GPIO, USER_I2C_SCL_Pin|USER_I2C_SDA_Pin);
	User_I2C_AlternateFunction_Init();
	
	/* 13. Set SWRST bit in I2Cx_CR1 register. */
	USER_I2C.Instance->CR1 |=  I2C_CR1_SWRST;
	HAL_Delay(2);
	/* 14. Clear SWRST bit in I2Cx_CR1 register. */
	USER_I2C.Instance->CR1 &=  ~I2C_CR1_SWRST;
	HAL_Delay(2);
	/* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register */
	MX_I2C1_Init();
	__HAL_I2C_ENABLE(&USER_I2C);
	HAL_Delay(2);
#ifdef I2C_TEST	
	printf("I2C_ResetBus\r\n");
#endif
	USER_I2C.ErrorCode = HAL_I2C_ERROR_NONE;
	USER_I2C.State = HAL_I2C_STATE_READY;
	USER_I2C.PreviousState = I2C_STATE_NONE;
	USER_I2C.Mode = HAL_I2C_MODE_NONE;
	
	return HAL_OK;
}


/*************************************************
Function: User_I2C_DummyWrite
Description: I2C Dummy Write 
Input:  
		slaveAddr - the target slave address
		RegAddr - register's address
		len - the operation length 
Output: None
Return: None
Others: None
************************************************/
static HAL_StatusTypeDef User_I2C_DummyWrite(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len)
{
	uint8_t dummyBUf[USER_I2C_DummyWrite_Bytes];
	
	dummyBUf[0] = RegAddr >> 8;
	dummyBUf[1] = RegAddr;
	dummyBUf[2] = len;
	
	return User_I2C_Master_DummyWrite(&USER_I2C, USER_I2C_SlaveAddrToSTM32(slaveAddr), dummyBUf, USER_I2C_DummyWrite_Bytes, USER_I2C_DummyWrite_Timeout);
}



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
HAL_StatusTypeDef User_I2C_WriteToSlave(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len, uint8_t *TxBuf)
{
	HAL_StatusTypeDef status;
	
	status = User_I2C_DummyWrite(slaveAddr, RegAddr, len);
	if(status == HAL_OK)
	{
		status = User_I2C_Master_Transmit(&USER_I2C, USER_I2C_SlaveAddrToSTM32(slaveAddr), TxBuf, len, USER_I2C_Transmit_Timeout);
		if(status != HAL_OK)
		{
			User_I2C_ResetBus();
#ifdef I2C_TEST
			printf("Err: User_I2C_WriteToSlave in I2CWriteToSlave, status =%d\r\n", status);
			printf("BUSY=%d, BERR=%d, AF=%d, ARLO=%d, OVR=%d\r\n" ,\
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BUSY), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BERR), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_AF), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_ARLO), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_OVR));
#endif
			return HAL_ERROR;
		}
	}
	else
	{
        User_I2C_ResetBus();
#ifdef I2C_TEST
			printf("Err: I2C_DummyWrite in I2CWriteToSlave, status =%d\r\n", status);
			printf("BUSY=%d, BERR=%d, AF=%d, ARLO=%d, OVR=%d\r\n" ,\
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BUSY), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BERR), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_AF), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_ARLO), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_OVR));
#endif
		return HAL_ERROR;
	}
	
	return HAL_OK;
}


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
HAL_StatusTypeDef User_I2C_ReadFromSlave(uint8_t slaveAddr, uint16_t RegAddr, uint8_t len, uint8_t *RxBuf)
{
	HAL_StatusTypeDef status;

	status = User_I2C_DummyWrite(slaveAddr, RegAddr, len);
	if(status == HAL_OK)
	{
		status = User_I2C_Master_Receive(&USER_I2C, USER_I2C_SlaveAddrToSTM32(slaveAddr), RxBuf, len, USER_I2C_Receive_Timeout);

		if(status != HAL_OK)
		{
			User_I2C_ResetBus();
#ifdef I2C_TEST
			printf("Err: HAL_I2C_Master_Receive in I2CReadFromSlave, status =%d\r\n", status);
			printf("BUSY=%d, BERR=%d, AF=%d, ARLO=%d, OVR=%d\r\n" ,\
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BUSY), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BERR), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_AF), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_ARLO), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_OVR));
#endif
			return HAL_ERROR;
		}
	}
	else
	{
        User_I2C_ResetBus();
#ifdef I2C_TEST
			printf("Err: I2C_DummyWrite in I2CReadFromSlave, status =%d\r\n", status);
			printf("BUSY=%d, BERR=%d, AF=%d, ARLO=%d, OVR=%d\r\n" ,\
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BUSY), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BERR), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_AF), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_ARLO), \
				__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_OVR));
#endif
		return HAL_ERROR;
	}
	
	return HAL_OK;
}


/*************************************************
Function: User_I2C_GeneralCall
Description: I2C_GeneralCall
Input:  
		None
Output: None
Return: status of User_I2C_GeneralCall
Others: None
************************************************/
HAL_StatusTypeDef User_I2C_GeneralCall(void)
{
	uint8_t txbuf[1] = {0x06};
	HAL_StatusTypeDef status;
	
	status = HAL_I2C_Master_Transmit(&USER_I2C, 0x00, txbuf, 1, USER_I2C_Transmit_Timeout);

	if(status != HAL_OK)
	{
		User_I2C_ResetBus();
#ifdef I2C_TEST
		printf("Err: HAL_I2C_Master_Receive in I2CReadFromSlave, status =%d\r\n", status);
		printf("BUSY=%d, BERR=%d, AF=%d, ARLO=%d, OVR=%d\r\n" ,\
			__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BUSY), \
			__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_BERR), \
			__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_AF), \
			__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_ARLO), \
			__HAL_I2C_GET_FLAG(&USER_I2C, I2C_FLAG_OVR));
#endif
		return HAL_ERROR;
	}
	
	return HAL_OK;
}



