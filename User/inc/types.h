#ifndef __TYPES_H
#define __TYPES_H

#include "config.h"


typedef enum{
	TASK_IO,
	TASK_COMM,
	TASK_EEPROM_WRITE
} Task_TypeDef;

typedef enum{
	//USART_BILL,
	USART_COMM,
	USART_USER,
	USART_3,
	USART_BILL,
	USART_NULL
} Usart_TypeDef;

typedef enum{
	INPUT_IDLE=0,
	INPUT_DOWN,
	INPUT_LONG_DOWN,
	INPUT_VIBRATE,
	INPUT_VIBRATE2,
	INPUT_VIBRATE3,
} Input_TypeDef;


typedef enum{
	MESSAGE_NULL,
	MESSAGE_EEPROM_WRITE,
	MESSAGE_EEPROM_READ,
	MESSAGE_CAN_WRITE,
	MESSAGE_CAN_READ,
} Message_TypeDef;

typedef enum{
	RES_OK,
	RES_CHECK_FAIL,//校验失败
	RES_LENGHT_ERR,//长度不足
	RES_HEAD_ERR,//头部错误
} Res_TypeDef;


typedef struct _Input {
	unsigned short int inputOffTime;
	unsigned short int inputLongTime;
	unsigned short int inputTime;
	PLUSE_MODE mode;
	Input_TypeDef state;
	char keyCode;
} Input_sut;

typedef struct _Output {
	unsigned short int time;
	unsigned int pluseNum;
	PLUSE_MODE mode;
	//char out;
} Output_sut;
		 				    
#endif
