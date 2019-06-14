/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "led.h"
#include "coin.h"
#include "ESP8266.h"
#include "DES.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#if SYSTEM_SUPPORT_UCOS
//设置任务堆栈大小
#define STK_SIZE      64

//设置任务优先级
#define TASK_Prio     10

//供分配的内存大小
#define MEM_SIZE_BIG		64
#define MEM_SIZE_LIT		32
#define MEM_NUM_BIG			20
#define MEM_NUM_LIT			10
#define MEM_NUM				MEM_NUM_LIT
#define MEM_SIZE			MEM_SIZE_BIG
#endif

#define COM_END				"\r\n"


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

//_TASK任务 _NUM堆栈大小 _PRIO优先级
#define CREATE_TASK(_TASK,_STKSIZE,_PRIO)	do{\
		static OS_STK _TASK##_Stk[_STKSIZE];			\
		OSTaskCreate(_TASK, (void *)0, &_TASK##_Stk[_STKSIZE-1],_PRIO);\
	}while(0)
#define	CREATE_SMSG(_OSQ,_NUM)	do{\
		static void* _OSQ##_MSG[_NUM];	\
		_OSQ =	OSQCreate(_OSQ##_MSG, _NUM); \
	}while(0)
#define CREATE_MEM(_OSMEM,_NUM,_SIZE)	do{\
		INT8U err;\
		static INT32U _OSMEM##_MEM[_NUM][_SIZE];	\
		_OSMEM =	OSMemCreate((void*)_OSMEM##_MEM, _NUM, _SIZE, &err); \
	}while(0)

#endif


/* Private functions ---------------------------------------------------------*/
typedef  void (*MSG_CALLBACK)(void *parg);

typedef struct com_struct {
    char* com;
	u8 level;//等级决定是否转发
	MSG_CALLBACK cb;//回调函数
} COM;

//串口通信协议设计
/*
* 起始标志 | 数据长度 | 命令 | 附加数据 | 校验 | 接收标志
* 1字节      1字节      1字节  未定       2字节  1字节
*/
#define COM_MIN_SIZE	6 //数据最小长度
#define COM_START_CHAR  (0xaa)
#define COM_END_CHAR	(0xab)

#define COM_LOGIN		1
#define COM_COIN		2

#define COM_USER		0
#define COM_WIFI		1

const int COIN_IO_TIME[5] ={50,100,150,200,250};

OS_EVENT * UartMutex;
OS_MEM * Mem;
OS_EVENT * ComQWifi;
OS_EVENT * ComQUser;
OS_EVENT * ComSemSend;//发送消息

u8 Level=0;

//消息分为三种，wifi消息，即从wifi发送到芯片端的消息。用户消息，即用户通过串口发送给wifi模块的消息。设备消息，设置设备参数的消息
const COM COM_MSG_WIFI[] = {
	{"OK",1},
	{"+CWLAP:",1},//当前环境下存在的无线路由
	{"+IPD",2},//远程服务器返回的数据
};

const COM COM_MSG_USER[] = {
	{"AT+RST",1},
	{"AT+CWMODE=",1},
	{"AT+CWLAP",1},//让模块列出当前环境下存在的无线路由
	{"AT+CWJAP=",0},//让模块连上路由器
	{"AT+CWJAP?",0},//检测是否连上路由
	{"AT+CIPMUX=",1},//启动多模块连接
	{"AT+CIPSTART=",1},//连接远程服务器
	{"AT+CIPSEND=",1},//发送数据
	{"AT+CIFSR",0},//查询ip地址
};

const COM COM_MSG_DEVEICE[] = {
	{"LEVEL=",0},//设置等级
	{"IOTIME=",0},//设置io时间
	{"WIFI=",0},//设置wifi账号密码
	{"SETID=",0},//设置设备id
	{"CHANGEID=",0},//改变id
	{"CHANGEPASSWORD=",0},//改变密码
};

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
	CREATE_TASK(TaskStart,STK_SIZE*4,TASK_Prio);
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
	u8 err,linkState=0;
	int cnt=0;
	char *p;
	COM * pCom;
	//u8 SendBuf[10];
	char key[8]={'p','r','o','g','r','a','m'},str[8]="123";
	
	OS_CPU_SR cpu_sr=0;
	
	int time=COIN_IO_TIME[0];//io时间
	
	pdata = pdata; 
	
	NVIC_Configuration();
	delay_init();	
	Usart_Init(COM_USER,115200);
	Usart_Init(COM_WIFI,115200); 
	
	LED_Init();
	COIN_Init();

	UartMutex = OSMutexCreate(TASK_Prio-1, &err);
	CREATE_MEM(Mem,MEM_NUM,MEM_SIZE);
	CREATE_SMSG(ComQWifi,20);
	CREATE_SMSG(ComQUser,5);
	ComSemSend = OSSemCreate(0);
	
	OSStatInit();
	OS_ENTER_CRITICAL(); 
//	OSTaskCreate( TaskOne, (void * )0, (OS_STK *)&TASK_ONE_STK[ONE_STK_SIZE-1],  ONE_TASK_Prio);
//	OSTaskCreate( TaskTwo, (void * )0, (OS_STK *)&TASK_TWO_STK[TWO_STK_SIZE-1],  TWO_TASK_Prio);
	CREATE_TASK(TaskOne,STK_SIZE,TASK_Prio+1);
	CREATE_TASK(TaskTwo,STK_SIZE*4,TASK_Prio+2);
	CREATE_TASK(TaskThree,STK_SIZE*5,TASK_Prio+3);
 	//OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
	
	ESP8266_init();
	
//	des_setkey(key);
//	des_run(str,str,encrypt);
//	Usart_SetData(COM_USER,(u8*)str,strlen(str));
//	Usart_SetData(COM_USER,(u8*)"\r\n",2);
//	des_run(str,str,decrypt);
//	Usart_SetData(COM_USER,(u8*)str,strlen(str));
//	Usart_SetData(COM_USER,(u8*)"\r\n",2);
	
	while(1)
	{	
		do{
			int l;
			p = (char*)OSQAccept(ComQUser,&err);//有用户串口输入
			if(err==OS_ERR_NONE){
				Usart_SetData(COM_WIFI,(u8*)p,strlen(p));
				OSTimeDly(2);
				l = Usart_GetData(COM_WIFI,(u8*)p,MEM_SIZE);
				if(l>0)Usart_SetData(COM_USER,(u8*)p,l);
				OSMemPut(Mem,p);
			}
		}while(err==OS_ERR_NONE);
		
		p = ESP8266_ReceiveString(ENABLE);
		if(p){
			Usart_SetData(COM_USER,(u8*)p,strlen(p));
			//ESP8266_SendString (DISABLE, p, strlen(p), Single_ID);
		}
		
		if(cnt==0){
			switch(linkState){
				case 0://未连接wifi
					if(ESP8266_Cmd ( "AT+CWJAP?", "OK", 0, 10000, true )){
						linkState=1;//连接上wifi
					}else{
						cnt = 50*10;//10秒重连
					}
					break;
				case 1://未连接服务器
					if(ESP8266_Link_Server(enumTCP,"192.168.1.100","9130",Single_ID)){
						linkState=2;//连接上服务器
						ESP8266_UnvarnishSend();//透传模式
					}else{
						cnt = 50*10;//10秒重连
					}
					break;
				case 2://登入服务器
					break;
			}
		}
		
//		pCom = (COM*)OSMboxPend(ComBox, 0, &err);
//		if(pCom->com==COM_LOGIN && pCom->len>0){
//			if(*pCom->pData>=0 && *pCom->pData<(sizeof(COIN_IO_TIME)/sizeof(COIN_IO_TIME[0]))){
//				time = COIN_IO_TIME[*pCom->pData];
//				SendMsg(COM_LOGIN,0,0);
//			}
//		}else if(pCom->com==COM_COIN){
//			u16 num=0;
//			if(pCom->len==1){
//				num = *pCom->pData;
//			}else if(pCom->len==2){
//				num = *pCom->pData;
//				num = num<<8 | *(pCom->pData+1);
//			}
//			//usart1_SetData((u8*)&num,2);
//			while(num--){
//				COIN0 = !COIN0;
//				COIN1 = !COIN1;
//				OSTimeDlyHMSM(0,0,0,time);
//				COIN0 = !COIN0;
//				COIN1 = !COIN1;
//				OSTimeDlyHMSM(0,0,0,time);
//			}
//			
//			SendMsg(COM_COIN,0,0);
//		}
		
		if(cnt)cnt--;
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
	int i,j;
	pdata = pdata; 
	

	
	while(1)
	{ 
		//串口2接收到的数据通过串口1发送，串口1接收到的数据通过串口2发送
//		int l = Usart_GetData(COM_WIFI,recviceBuf,128);
//		if(l>0){
//			OSTimeDly(1);
//			l += Usart_GetData(COM_WIFI,recviceBuf+l,128-l);
//		}
//		Usart_SetData(0,recviceBuf,l);
//		OSTimeDly(1);
//		
//		
//		l = Usart_GetData(0,recviceBuf,128);
//		if(l>0){
//			OSTimeDly(1);
//			l += Usart_GetData(0,recviceBuf+l,128-l);
//		}
//		COM_SetData(recviceBuf,l);
		
		//LED1 = !LED1;
		OSTimeDly(1);	
	}
}

/*******************************************************************************
* Function Name  : TaskTwo.
* Descriptioan   : 将接收到的数据解析为以\r\n为结尾的命令.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TaskThree(void *pdata)
{
	int i,j,lUser=0;
	u8 bufUser[64];
	u8 err;
	pdata = pdata;
	while(1)
	{
		
		//用户消息
		lUser+=Usart_GetData(COM_USER,bufUser,sizeof(bufUser));
		if(lUser>=strlen(COM_END)){
			j=0;
			for(i=0;i<lUser-strlen(COM_END)+1;i++){//解析消息
				if(strncmp((char*)bufUser+i,COM_END,strlen(COM_END))==0){//收到一个完整的消息
					u8* p = OSMemGet(Mem,&err);
					memcpy(p,bufUser+j,i+strlen(COM_END)-j);
					p[i+strlen(COM_END)-j]='\0';
					OSQPost(ComQUser,p);//发送收到的消息
					j=i+strlen(COM_END);//记录下一条消息开始的位置
				}
			}
			//前移数据
			if(j>0){
				lUser-=j;
				memcpy(bufUser,bufUser+j,lUser);
			}
		}
		
		OSTimeDly(3);	
	}
}

