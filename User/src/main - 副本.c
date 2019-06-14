/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "coin.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//设置任务堆栈大小
#define START_STK_SIZE      64
#define ONE_STK_SIZE        64
#define TWO_STK_SIZE        64

//设置任务优先级
#define START_TASK_Prio     10
#define ONE_TASK_Prio       4
#define TWO_TASK_Prio       5


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//任务堆栈
OS_STK  TASK_START_STK[START_STK_SIZE];
OS_STK  TASK_ONE_STK[ONE_STK_SIZE];
OS_STK  TASK_TWO_STK[TWO_STK_SIZE];


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//任务申明
void TaskStart(void *pdata);
void TaskOne(void *pdata);
void TaskTwo(void *pdata);	


/* Private functions ---------------------------------------------------------*/
int my_strcmp(char *p1,char *p2)           
{
	int i;
	i=0;
	while(*(p1+i)==*(p2+i))
		if (*(p1+i++)=='\0') return(0);     
	return(*(p1+i)-*(p2+i));              
}

int my_strncmp ( char * s1, char * s2, size_t n) 
{
  if ( !n )//n????????;??n?0,???0

  return(0);

  //?????while??? 

  //???????:--n,??????n????????

  //???????:*s1,??s1????????????

  //???????:*s1 == *s2,??????????????

  while (--n && *s1 && *s1 == *s2) 
  { 
  s1++;//S1????1,???????

  s2++;//S2????1,???????

  }
  return( *s1 - *s2 );//??????

}


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
	OSTaskCreate( TaskStart,	                            //task pointer
					(void *)0,	                                  //parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
					START_TASK_Prio );	                          //task priority
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
	OS_CPU_SR cpu_sr=0;
	
	int state=0;//状态
	int time=50;//io时间
	
	pdata = pdata; 
	
	NVIC_Configuration();
	delay_init();		
	usart1_init(19200); 
	
	LED_Init();
	COIN_Init();
	
	OSStatInit();
	OS_ENTER_CRITICAL(); 
	OSTaskCreate( TaskOne, (void * )0, (OS_STK *)&TASK_ONE_STK[ONE_STK_SIZE-1],  ONE_TASK_Prio);
	//OSTaskCreate( TaskTwo, (void * )0, (OS_STK *)&TASK_TWO_STK[TWO_STK_SIZE-1],  TWO_TASK_Prio);
 	//OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
	while(1)
	{	
		u8 buf[64];
		int l,err;
		l = usart1_GetData(buf,64);
		
		
		if(l>0){
			err=0;
			while(1){//等待命令接收结束
				int n=0;
				//delay_ms(3);
				OSTimeDly(1);
				n=usart1_GetData(buf+l,64);
				if(n>0)l=l+n;
				else break;
			}
			if(strncmp((const char*)buf,"login",strlen("login"))==0){
				int t = atoi((const char*)buf+strlen("login"));
				state = 1;//登入
				if(t>=50 && t<=250){
					time = t;
				}
			}else if(state==1){
				if(strncmp((const char*)buf,"exit",strlen("exit"))==0){
					state = 0;//退出
				}else if(strncmp((const char*)buf,"coin",strlen("coin"))==0){
					int coin = atoi((const char*)buf+strlen("coin"));
					while(coin){
						COIN0 = !COIN0;
						COIN1 = !COIN1;
						OSTimeDlyHMSM(0,0,0,time);
						COIN0 = !COIN0;
						COIN1 = !COIN1;
						OSTimeDlyHMSM(0,0,0,time);
						coin--;
					}
					
				}else{
					err=1;
				}
			}else {
				err=1;
			}
			
			if(err){
				usart1_SetData((u8*)"err",3);//处理完毕返回信息
			}else{
				usart1_SetData(buf,l);//处理完毕返回信息
			}
		}
		
		OSTimeDly(2);
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
  pdata = pdata; 
	
	while(1)
	{	
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,800);	
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
  pdata = pdata; 
	
	while(1)
	{ 
		//LED1 = !LED1;
		//OSTimeDlyHMSM(0,0,1,10);	
	}
}

