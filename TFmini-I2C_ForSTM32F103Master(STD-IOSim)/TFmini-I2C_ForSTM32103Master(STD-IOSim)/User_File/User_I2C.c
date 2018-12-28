/*************************************************
Copyright (C), 2017 Benewake(Beijing) Co, Ltd. All Rright Reserved.
File name: 	User_I2C.c
Author: 	wuxiuhua
Version: 	V1.0
Date: 		2018-12-24
Description: 
				User usart Configuration
Others: 		
Function List: 
1. static void User_Usart_IO_Init(void)
2. void User_Usart_Init(void)
3. int fputc(int ch, FILE *f)
History: 
	V1.0 2018-12-24 wuxiuhua Create C file
*************************************************/
#include "User_I2C.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include <stdarg.h>

#define I2C_TEST	


/*************************************************
Function: User_I2C_Init
Description: config I2C 
*************************************************/
void User_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* config GPIOB clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure I2C1 SCL(PB.06) as GPIO_Mode_AF_OD */
	/* Configure I2C1 SDA (PB.07) as GPIO_Mode_AF_OD */
	GPIO_InitStructure.GPIO_Pin = USER_I2C_SCL_PIN | USER_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(USER_I2C_GPIO, &GPIO_InitStructure);
		
	I2C_SCL_H();
	I2C_SDA_H();
}

/*************************************************
Function: I2C_SET_SDA_Mode
Description: 
Input:  SDAMode - SDA Pin Mode - input or output
*************************************************/
static void I2C_SET_SDA_Mode(__I2C_SDAMode_TypeDef SDAMode)    
{    
	GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_StructInit(&GPIO_InitStructure);    
    GPIO_InitStructure.GPIO_Pin = USER_I2C_SDA_PIN;    
    GPIO_InitStructure.GPIO_Mode = (SDAMode == I2C_SDA_IN) ? GPIO_Mode_IPU : GPIO_Mode_Out_OD;    
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
}


/*************************************************
Function: delay
Description: 
Input:  Delaycnt - delay cnt
*************************************************/
void delay(uint32_t Delaycnt)
{
	uint32_t i;
	
	for(i = 0; i<Delaycnt; i++)
	{
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	}
}

/*************************************************
Function: I2C_GenerateStart
Description: 
*************************************************/
static void I2C_GenerateStart(void)
{
	I2C_SET_SDA_Mode(I2C_SDA_OUT);
	
    I2C_SDA_H();
    I2C_SCL_H();
    delay(5);
    I2C_SDA_L();
    delay(5);
    I2C_SCL_L();
    delay(5);
}

/*************************************************
Function: I2C_GenerateStop
Description: 
*************************************************/
static void I2C_GenerateStop(void)
{
	I2C_SCL_L();  
    I2C_SDA_L();  
    delay(5);  
	I2C_SCL_H(); 
    delay(5); 
	I2C_SDA_H();  
    delay(5); 
}


/*************************************************
Function: I2C_GenerateAck
Description: 
*************************************************/
static void I2C_GenerateAck(__I2C_ACK_TypeDef ack)
{
    if(ack == I2C_NACK) 
	{
        I2C_SDA_H();
	}
    else
	{
        I2C_SDA_L();
	}
	
    delay(5);		
    I2C_SCL_H();
    delay(5);
    I2C_SCL_L();
}

/*************************************************
Function: I2C_RecvAck
Description: 
*************************************************/
static __I2C_ACK_TypeDef I2C_RecvAck(uint32_t Timeout)
{
	uint32_t timeout = 0;
    __I2C_ACK_TypeDef ackStatus;
	
	I2C_SDA_H();
	I2C_SET_SDA_Mode(I2C_SDA_IN);
    delay(5);
	I2C_SCL_H();
    delay(5);
	
	while(1)
	{
		ackStatus = (I2C_SDA_IN() == Bit_RESET) ? I2C_ACK : I2C_NACK;
		if(ackStatus == I2C_ACK)
		{
			break;
		}
		
		if((timeout++) > Timeout)
		{
			ackStatus = I2C_NACK;
			break;
		}
	}

    I2C_SCL_L();
    delay(5);
    I2C_SET_SDA_Mode(I2C_SDA_OUT);
    return ackStatus;  
}


/*************************************************
Function: I2C_SendByte
Description: 
*************************************************/
static __I2C_ACK_TypeDef I2C_SendByte(uint8_t data, uint32_t Timeout)
{
    uint32_t i;
	
    I2C_SCL_L();
	
    for (i = 0; i < 8; i++)      
    {
        if((data << i)&0x80)
        {
            I2C_SDA_H();
        }
        else 
        {
            I2C_SDA_L();
        }
        
        I2C_SCL_H();               
        delay(5);             
        I2C_SCL_L();    
        delay(5);   
    }

    return I2C_RecvAck(Timeout);
}


/*************************************************
Function: I2C_RecvByte
Description: 
*************************************************/
static uint8_t I2C_RecvByte(void)
{
    uint32_t i;
    uint32_t data = 0, BitStatus = 0;
    
    I2C_SDA_H();
	I2C_SET_SDA_Mode(I2C_SDA_IN);
    delay(5);
    for (i = 0; i < 8; i++)
    {  
        data <<= 1;
        I2C_SCL_H(); 
        delay(5); 
		
        BitStatus = (I2C_SDA_IN() == Bit_SET) ? 1 : 0;
        data |= BitStatus;     
		
        I2C_SCL_L();
        delay(5);
    }
	
	I2C_SET_SDA_Mode(I2C_SDA_OUT);
    return data;
}





/*************************************************
Function: I2C_DummyWrite
Description: I2C DummyWrite
*************************************************/
static __I2C_Status_TypeDef I2C_DummyWrite(uint8_t SlaveAddr, uint16_t RegAddr, uint8_t OperLen, uint32_t Timeout)
{
	/* Generate Start Sign */
	I2C_GenerateStart();
	
	/* Send Slave Address */
	if(I2C_SendByte((SlaveAddr<<1) | I2C_Transmitter, Timeout) != I2C_ACK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}

	/* Send RegAddr_H */
	if(I2C_SendByte((RegAddr >> 8) & 0xFF, Timeout) != I2C_ACK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}
	
	/* Send RegAddr_L */
	if(I2C_SendByte(RegAddr & 0xFF, Timeout) != I2C_ACK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}
	
	/* Send Data length */
	if(I2C_SendByte(OperLen, Timeout) != I2C_ACK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}

	return I2C_OK;
}


/*************************************************
Function: I2C_SendBytes
Description: I2C SendBytes
*************************************************/
__I2C_Status_TypeDef I2C_SendBytes(uint8_t SlaveAddr, uint16_t RegAddr, uint8_t *TxBuf, uint8_t OperLen, uint32_t Timeout)
{
	uint8_t i = 0;
	
	/* Dummy Write */
	if(I2C_DummyWrite(SlaveAddr, RegAddr, OperLen, Timeout) != I2C_OK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}

	/* Generate Start Sign */
	I2C_GenerateStart();

	/* Send Slave Address */
	if(I2C_SendByte((SlaveAddr<<1) | I2C_Transmitter, Timeout) != I2C_ACK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}
	
	/* Send Data */
	for(i = 0; i < OperLen; i++)
	{
		if(I2C_SendByte(TxBuf[i], Timeout) != I2C_ACK)
		{
			I2C_GenerateStop();
			return I2C_ERROR;
		}
	}

	/* Generate Stop Sign */
	I2C_GenerateStop();
	
	return I2C_OK;
}


/*************************************************
Function: I2C_RecvBytes
Description: I2C SendBytes
*************************************************/
__I2C_Status_TypeDef I2C_RecvBytes(uint8_t SlaveAddr, uint16_t RegAddr, uint8_t *Rxbuf, uint8_t OperLen, uint32_t Timeout)
{
	uint8_t i = 0;
	
	/* Dummy Write */
	if(I2C_DummyWrite(SlaveAddr, RegAddr, OperLen, Timeout) != I2C_OK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}

	/* Generate Start Sign */
	I2C_GenerateStart();
	
	/* Send Slave Address */
	if(I2C_SendByte((SlaveAddr<<1) | I2C_Receiver, Timeout) != I2C_ACK)
	{
		I2C_GenerateStop();
		return I2C_ERROR;
	}
	
	/* Receive Data */
	for(i = 0; i < OperLen - 1; i++)
	{
		Rxbuf[i] = I2C_RecvByte();
		I2C_GenerateAck(I2C_ACK);
	}

	Rxbuf[OperLen - 1] = I2C_RecvByte();
	I2C_GenerateAck(I2C_NACK);
	
	/* Generate Stop Sign */
	I2C_GenerateStop();
	
	return I2C_OK;
}





