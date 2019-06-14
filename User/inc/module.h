#ifndef __MODULE_H
#define __MODULE_H
#include "sys.h"

typedef void (*Init)(void);
typedef void (*Process)(unsigned short int);
typedef unsigned short int (*Handle)(Message_sut *);

typedef struct _Module {
	Init init;
	Init afterInit;
	Process process[3];
	Handle handle[3];
} Module_sut;

void Module_Init(void);
void Module_AfterInit(void);

/*******************************************************************************
* Function Name  : Module_Process.
* Descriptioan   : 模块处理过程.
* Input          : 处理的线程，周期.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Module_Process(unsigned short int processNum, unsigned short int period);

/*******************************************************************************
* Function Name  : Module_MsgHandle.
* Descriptioan   : 模块消息处理.
* Input          : 处理的线程，消息.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Module_MsgHandle(unsigned short int processNum, Message_sut * pMessage);

#endif
