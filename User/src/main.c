/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "config.h"

#include "tool.h"
#include "module.h"


#include <stdlib.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#if SYSTEM_SUPPORT_UCOS
//设置任务堆栈大小
#define STK_SIZE      64

//设置任务优先级
#define TASK_Prio     10

#endif


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if SYSTEM_SUPPORT_UCOS
//任务申明
void TaskStart(void *pdata);
void TaskOne(void *pdata);
void TaskTwo(void *pdata);	
void TaskThree(void *pdata);

#endif


/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : main.
* Descriptioan   : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{	
	
	OSInit();
//	OSTaskCreate( TaskStart,	                            //task pointer
//					(void *)0,	                                  //parameter
//					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
//					START_TASK_Prio );	                          //task priority
	CREATE_TASK(TaskStart,STK_SIZE*20,TASK_Prio,0);
	OSStart();
	return 0;	
}


/*******************************************************************************
* Function Name  : TaskStart.
* Descriptioan   : 起始任务.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TaskStart(void * pdata)
{
	
	Message_sut * pMessage;
	void * pTemp;
	u32 lastOSTime=0;
	u32 nextOSTime=0;

	OS_CPU_SR cpu_sr=0;
	
	pdata = pdata; 
	
	SYS_Init();

	Module_Init();
	
	OSStatInit();
	OS_ENTER_CRITICAL(); 
	SYS_Start();
	CREATE_TASK(TaskOne,STK_SIZE*20,TASK_Prio+1,pTemp);//can通信线程
	CREATE_TASK(TaskTwo,STK_SIZE*20,TASK_Prio+2,pTemp);//eeprom写线程
	OS_EXIT_CRITICAL();
	
	Module_AfterInit();
	while(1)
	{	
		nextOSTime=OSTime;
		Module_Process(0,Tool_LoopSub(lastOSTime,OSTime));
		lastOSTime=nextOSTime;

		do{
			pMessage= MSG_Receive(0);
			if(pMessage){
				Module_MsgHandle(0,pMessage);
				switch(pMessage->head.message){
					
						
					default:
						break;
				}

				//OSMemPut(Mem, (void *) pMessage);
				MSG_Free(pMessage);

			}
		}while(pMessage);
		OSTimeDly(1);
		
	}
}

/*******************************************************************************
* Function Name  : TaskOne.
* Descriptioan   : 任务0.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void TaskOne(void *pdata)
{
	Message_sut * pMessage=0;
	u32 lastOSTime=0;
	u32 nextOSTime=0;
	while(1)
	{	
		nextOSTime=OSTime;
		Module_Process(1,Tool_LoopSub(lastOSTime,OSTime));
		lastOSTime=nextOSTime;
		do{
			pMessage= MSG_Receive(1);
			if(pMessage){
				Module_MsgHandle(1,pMessage);
				switch(pMessage->head.message){
					default:
						break;
				}

				//OSMemPut(Mem, (void *) pMessage);
				MSG_Free(pMessage);

			}
		}while(pMessage);
		
		OSTimeDly(10);	
		//LED = !LED;
		//OSTimeDlyHMSM(0,0,0,800);	
	}
}

/*******************************************************************************
* Function Name  : TaskTwo.
* Descriptioan   : 任务1.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void TaskTwo(void *pdata)
{ 
	Message_sut * pMessage;
	u32 lastOSTime=0;
	u32 nextOSTime=0;
	pdata = pdata; 
	
	while(1)
	{
		nextOSTime=OSTime;
		Module_Process(2,Tool_LoopSub(lastOSTime,OSTime));
		lastOSTime=nextOSTime;
		do{
			pMessage= MSG_Receive(2);
			if(pMessage){
				Module_MsgHandle(2,pMessage);
				switch(pMessage->head.message){
						
					default:
						break;
				}
				MSG_Free(pMessage);

			}
		}while(pMessage);
		OSTimeDly(100);	
	}
}

/*******************************************************************************
* Function Name  : TaskTwo.
* Descriptioan   : 将接收到的数据解析为以\r\n为结尾的命令.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//void TaskThree(void *pdata)
//{
////	int i,j,lUser=0;
////	u8 bufUser[64];
////	u8 err;
//	pdata = pdata;
//	while(1)
//	{
//		
//		OSTimeDly(3);	
//	}
//}


