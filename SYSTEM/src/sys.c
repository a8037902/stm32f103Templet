

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "DES.h"


/*******************************************************************************
* Function Name  : NVIC_SETPRIMASK.
* Descriptioan   : 屏蔽所以中断.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_SETPRIMASK(void)//提升优先级至0
{
	__set_PRIMASK(1);
}


/*******************************************************************************
* Function Name  : NVIC_RESETPRIMASK.
* Descriptioan   : 中断屏蔽移除.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_RESETPRIMASK(void)//放开优先级
{
  __set_PRIMASK(0);
}


/*******************************************************************************
* Function Name  : NVIC_SETBASEMASK.
* Descriptioan   : 设置屏蔽中断值.
* Input          : 屏蔽值.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_SETBASEMASK(uint32_t basePri)//
{
	__set_BASEPRI(basePri);
}


/*******************************************************************************
* Function Name  : NVIC_GETBASEMASK.
* Descriptioan   : 获取屏蔽中断值.
* Input          : None.
* Output         : None.
* Return         : 屏蔽值.
*******************************************************************************/
uint32_t NVIC_GETBASEMASK(void)//
{
  return __get_BASEPRI();
}


/*******************************************************************************
* Function Name  : NVIC_Configuration.
* Descriptioan   : NVIC中断配置.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_Configuration(void)
{
//	NVIC_InitTypeDef NVIC_InitStructure;

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

/*******************************************************************************
* Function Name  : Debug_Printf.
* Descriptioan   : 调试打印.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u16 DebugLevel=MY_DEBUG;
void Debug_Printf(u16 level, const char *s,...)
{
	OS_CPU_SR cpu_sr=0;
	va_list argp;  
	u8 buf[100];
	
	u16 l;
	
	OS_ENTER_CRITICAL();
	l = DebugLevel;
	OS_EXIT_CRITICAL();
	if(level<l){
		va_start( argp, s); 
		vsprintf((char*)buf,s,argp);
		va_end( argp );
		Usart_SetData(0,buf,strlen((const char*)buf));
	}
	
}

void Flash_Encode (void);

#if SYSTEM_SUPPORT_UCOS==1
OS_MEM * Mem;
OS_MEM * MemBig;

u8 _OSMEM_Mem[MEM_NUM][MEM_SIZE];
u8 _OSMEM_MemBig[MEM_BIG_NUM][MEM_BIG_SIZE];

OS_EVENT * QMessage[TASK_NUM];
static void* _OSQ_MSG[TASK_NUM][MESSAGE_NUM];
#endif
/*******************************************************************************
* Function Name  : Sys_Init.
* Descriptioan   : 系统初始化.
* Input          : 串口波特率.
* Output         : 初始化结果.
* Return         : None.
*******************************************************************************/
u32 Sys_Init(u32 baudRate0,u32 baudRate1,u32 baudRate2){
	NVIC_Configuration();
	delay_init();		
	if(baudRate0)Usart_Init(0,baudRate0); 
	if(baudRate1)Usart_Init(1,baudRate1); 
	if(baudRate2)Usart_Init(2,baudRate2); 

#if SYSTEM_SUPPORT_UCGUI==1	
	GUI_Init();
#endif
	
#if ENCRYPTION==1
	if(*(u32*)Flash_Encode!=0xFFFFFFFF){
		FLASH_Unlock();
		Flash_Encode();
		FLASH_ErasePage(ENCRYPT_FLASH_ADDRESS);
		if(PAGE_SIZE==1024)FLASH_ErasePage(ENCRYPT_FLASH_ADDRESS+PAGE_SIZE);
		FLASH_Lock();
	}
#endif
	
	return 0;
}

/*******************************************************************************
* Function Name  : Sys_Discryption.
* Descriptioan   : 系统解密.
* Input          : key.
* Output         : 解密结果.
* Return         : None.
*******************************************************************************/
BOOL Sys_Discryption(u8 *key){
	u8 buf[8];
	memcpy(buf,(void*)(ENCRYPT_FLASH_ADDRESS+0x800),4);
	memcpy((u32*)buf+1,(void*)(ENCRYPT_FLASH_ADDRESS+0x804),4);
	des(buf,key,1,buf);
	if(((u32*)buf)[0]==*(__IO u32*)(0x1FFFF7E8) && ((u32*)buf)[1]==*(__IO u32*)(0x1FFFF7EC)){
		return TRUE;
	}
	
	return FALSE;
}

#if SYSTEM_SUPPORT_UCOS==1
void Sys_Start(){
	u8 err;
	int i=0;
	for(i=0;i<TASK_NUM;i++){
		QMessage[i] = OSQCreate(_OSQ_MSG[i],MESSAGE_NUM);
	}
	
	Mem = OSMemCreate(_OSMEM_Mem,MEM_NUM,MEM_SIZE,&err);
	MemBig = OSMemCreate(_OSMEM_MemBig,MEM_BIG_NUM,MEM_BIG_SIZE,&err);
}

void * MemGet(u16 size, u8 * perr)
{
	void *p=0;
	*perr=0xff;
	if(size<=MEM_SIZE){
		p = OSMemGet(Mem, perr);
	}else if(size<=MEM_BIG_SIZE){
		p = OSMemGet(MemBig, perr);
	}
	if(*perr!=OS_ERR_NONE)Debug_Printf(15,"MemGet err=%d\r\n",*perr);
	return p;
}

u8 MemPut(void * pblk)
{
	u8 err=99;
	if((u8*)pblk>=(u8*)_OSMEM_Mem && (u8*)pblk<(u8*)_OSMEM_Mem+MEM_NUM*MEM_SIZE){
		err = OSMemPut(Mem, pblk);
	}else if((u8*)pblk>=(u8*)_OSMEM_MemBig && (u8*)pblk<(u8*)_OSMEM_MemBig+MEM_BIG_NUM*MEM_BIG_SIZE){
		err = OSMemPut(MemBig, pblk);
	}
	if(err!=OS_ERR_NONE)Debug_Printf(15,"MemPut err=%d\r\n",err);
	return err;

}

u8 send_Message(u8 task, u16 message, u16 para0, u16 para1, u8 * pData, u16 len)
{
	u8 err;
	Message_sut* pMessage=0;
	pMessage = (Message_sut*)MemGet(MEM_SIZE,&err);//(Message_sut*)OSMemGet(Mem, &err);
	if(pMessage){
		pMessage->head.message = message;
		pMessage->head.para0 = para0;
		pMessage->head.para1 = para1;
		pMessage->head.len = len;
		if(len>MEM_SIZE-sizeof(MessageHead_sut)){
			pMessage->data.pData = MemGet(len,&err);
			memcpy(pMessage->data.pData,pData,len);
		}else{
			memcpy(pMessage->data.data8,pData,len);
		}
		err = OSQPost(QMessage[task], (void *)pMessage);
		if(err==OS_ERR_NONE)Debug_Printf(15, "s_M:%d,%d\r\n",task,pMessage->head.message);
		return err;
	}
	Debug_Printf(15, "s_M err:%d\r\n",err);
	return err;
}

Message_sut * receive_Message(u8 task)
{
	u8 err;
	Message_sut* pMessage=0;
	pMessage= (Message_sut *)OSQAccept(QMessage[task],&err);
	if(pMessage)Debug_Printf(15, "r_M:%d,%d\r\n",task,pMessage->head.message);
	return pMessage;
}

u8 free_Message(Message_sut* pMessage)
{
	u8 err;
	if(pMessage->head.len>MEM_SIZE-sizeof(MessageHead_sut))err = MemPut((void*)pMessage->data.pData);
	err = MemPut((void *) pMessage);
	if(err !=OS_ERR_NONE)Debug_Printf(15, "f_M err:%d\r\n",err);
	return err;
}
#endif

/************************ (C) COPYRIGHT 黑帮老大 *****END OF FILE***************/

