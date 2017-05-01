/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "led.h"
#include "touch.h" 
#include "ILI93xx.h"

#if AT_EN
#include "at.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if SYSTEM_SUPPORT_UCOS==1
//设置任务堆栈大小
#define STK_SIZE      64

//设置任务优先级
#define TASK_Prio     10
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//任务申明
void TaskStart(void *pdata);
void TaskOne(void *pdata);
void TaskTwo(void *pdata);	


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
#if SYSTEM_SUPPORT_UCOS==1
	OSInit();
//	OSTaskCreate( TaskStart,	                            //task pointer
//					(void *)0,	                                  //parameter
//					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
//					START_TASK_Prio );	                          //task priority
	CREATE_TASK(TaskStart,STK_SIZE*12,TASK_Prio,0);
	OSStart();
#endif
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
#if SYSTEM_SUPPORT_UCOS==1
	OS_CPU_SR cpu_sr=0;
	Message_sut* pMessage=0;
#endif
	
	pdata = pdata; 
	
	Sys_Init(115200,0,0);
	
#if SYSTEM_SUPPORT_UCOS==1
	OSStatInit();
	OS_ENTER_CRITICAL(); 
	Sys_Start();
	CREATE_TASK(TaskOne,STK_SIZE*8,TASK_Prio+1,0);
	CREATE_TASK(TaskTwo,STK_SIZE*4,TASK_Prio+2,0);
// 	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
#endif
	
#if SYSTEM_SUPPORT_UCGUI==1
	GUI_SetBkColor(GUI_RED);
	GUI_SetColor(GUI_BLUE);
	GUI_Clear();
#endif
	while(1)
	{	
#if AT_EN==1
		at_recvTask();
#endif
#if SYSTEM_SUPPORT_UCOS==1
		do{
			pMessage= receive_Message(0);
			if(pMessage){
				switch(pMessage->head.message){
						
					default:
						break;
				}
				free_Message(pMessage);

			}
		}while(pMessage);
		OSTimeDly(1);
#endif
		
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
#if SYSTEM_SUPPORT_UCOS==1
	Message_sut* pMessage=0;
#endif 
	pdata = pdata; 
	
	while(1)
	{	
		LED0 = !LED0;
#if SYSTEM_SUPPORT_UCOS==1
		do{
			pMessage= receive_Message(1);
			if(pMessage){
				switch(pMessage->head.message){
						
					default:
						break;
				}
				free_Message(pMessage);

			}
		}while(pMessage);
		OSTimeDlyHMSM(0,0,0,800);	
#endif
		
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
#if SYSTEM_SUPPORT_UCOS==1
	Message_sut* pMessage=0;
#endif
	pdata = pdata; 
	
	while(1)
	{ 
		LED1 = !LED1;
#if SYSTEM_SUPPORT_UCOS==1
		do{
			pMessage= receive_Message(2);
			if(pMessage){
				switch(pMessage->head.message){
						
					default:
						break;
				}
				free_Message(pMessage);

			}
		}while(pMessage);
		OSTimeDlyHMSM(0,0,1,10);
#endif
			
	}
}

