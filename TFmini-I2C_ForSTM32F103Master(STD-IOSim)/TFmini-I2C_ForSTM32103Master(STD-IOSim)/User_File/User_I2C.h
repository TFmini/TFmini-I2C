#ifndef __User_I2C_H
#define __User_I2C_H
#include "stm32f10x_i2c.h"


#define USER_I2C_SCL_PIN  GPIO_Pin_6
#define USER_I2C_SDA_PIN  GPIO_Pin_7
#define USER_I2C_GPIO     GPIOB
#define USER_I2C          I2C1

#define I2C_SCL_H() 		GPIO_SetBits(USER_I2C_GPIO, USER_I2C_SCL_PIN)
#define I2C_SCL_L()			GPIO_ResetBits(USER_I2C_GPIO, USER_I2C_SCL_PIN)

#define I2C_SDA_H()			GPIO_SetBits(USER_I2C_GPIO, USER_I2C_SDA_PIN)
#define I2C_SDA_L()			GPIO_ResetBits(USER_I2C_GPIO, USER_I2C_SDA_PIN)

#define I2C_SCL_IN()		GPIO_ReadInputDataBit(USER_I2C_GPIO, USER_I2C_SCL_PIN)
#define I2C_SDA_IN()		GPIO_ReadInputDataBit(USER_I2C_GPIO, USER_I2C_SDA_PIN)

typedef enum
{
	I2C_OK = 0U,
	I2C_BUSY = 1U,
	I2C_TIMEOUT = 3U,
	I2C_ERROR = 4U,
}__I2C_Status_TypeDef;

typedef enum
{
	I2C_ACK = 0U,
	I2C_NACK = 1U,
}__I2C_ACK_TypeDef;

typedef enum
{
	I2C_Transmitter = 0U,
	I2C_Receiver = 1U,
}__I2C_Oper_TypeDef;

typedef enum
{
	I2C_SDA_IN = 0U,
	I2C_SDA_OUT = 1U,
}__I2C_SDAMode_TypeDef;
/*************************************************
Function: User_I2C_Init
Description: config I2C 
*************************************************/
void User_I2C_Init(void);

/*************************************************
Function: delay
Description: 
Input:  Delaycnt - delay cnt
*************************************************/
void delay(uint32_t Delaycnt);

/*************************************************
Function: I2C_SendBytes
Description: I2C SendBytes
*************************************************/
__I2C_Status_TypeDef I2C_SendBytes(uint8_t SlaveAddr, uint16_t RegAddr, uint8_t *TxBuf, uint8_t OperLen, uint32_t Timeout);

/*************************************************
Function: I2C_RecvBytes
Description: I2C SendBytes
*************************************************/
__I2C_Status_TypeDef I2C_RecvBytes(uint8_t SlaveAddr, uint16_t RegAddr, uint8_t *Rxbuf, uint8_t OperLen, uint32_t Timeout);
#endif /* __User_I2C_H */



