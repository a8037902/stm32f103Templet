# stm32f103Templet

## 功能模块介绍

### 模块
业务代码在放在Module文件夹下，模块之间可以根据业务需要将业务分离在不同的线程中。通过消息传递数据。
module.c文件根据需要填写模块的处理函数或注销。
```c
static Module_sut s_modules[]={
    {IWDG_ModuleInit,0,IWDG_ModuleProcess0,0,0, 0,0,0},
	{Usart_ModuleInit,0,0,0,0, 0,0,0},
	{LED_ModuleInit,0,LED_ModuleProcess0,0,0, 0,0,0},
	//{AT_ModuleInit,0,AT_ModuleProcess0,0,0, 0,0,0},
	{UsartLoop_ModuleInit,0,UsartLoop_ModuleProcess0,0,0, UsartLoop_ModuleHandle0,0,0},
	{Input_ModuleInit,0,Input_ModuleProcess0,0,0, 0,0,0},
	{MsgTest_ModuleInit,0,0,MsgTest_ModuleProcess1,0, MsgTest_ModuleHandle0,0,0},
	//{GUI_ModuleInit,GUI_ModuleAfterInit,0,0,GUI_ModuleProcess2, 0,0,0},
};
```


### 串口
串口操作完全由中断完成，读写数据会缓存到一个循环缓冲区，无需占用线程时间。
```c
typedef struct usart_struct {
    
    u8 *pRecBufH;
	u8 *pRecBufE;
	u8 *pSendBufH;
	u8 *pSendBufE;
//	u8 receiveBuf[DATA_BUF_SIZE];
//	u8 sendBuf[DATA_BUF_SIZE];
	u8 *pReceiveBuf;
	u8 *pSendBuf;
	u16 len;
} sutUSART;

USART_TypeDef * USARTS[5]={USART1,USART2,USART3,UART4,UART5};

static sutUSART g_Usarts[5];

void _USART_IRQHandler(int index){
    if((USARTS[index]->SR & (uint32_t)(1<<5)) && (USARTS[index]->CR1 & (uint32_t)(1<<5)))
	{	
		//LED=!LED;
		*g_Usarts[index].pRecBufH = USARTS[index]->DR & (uint16_t)0x01FF;
		++g_Usarts[index].pRecBufH;
		if((g_Usarts[index].pRecBufH-g_Usarts[index].pReceiveBuf) >= g_Usarts[index].len)
		{
			g_Usarts[index].pRecBufH = g_Usarts[index].pReceiveBuf;
		}
	}	
	
	if((USARTS[index]->SR & (uint32_t)(1<<7)) && (USARTS[index]->CR1 & (uint32_t)(1<<7)))
	{
		//delay_us(10);
		//USART_ClearITPendingBit(USART, USART_IT_TXE);
		USARTS[index]->DR = *g_Usarts[index].pSendBufE;
		g_Usarts[index].pSendBufE++;
		if(g_Usarts[index].pSendBufE - g_Usarts[index].pSendBuf >= g_Usarts[index].len)g_Usarts[index].pSendBufE = g_Usarts[index].pSendBuf;
		if(g_Usarts[index].pSendBufE == g_Usarts[index].pSendBufH)		//·￠?ííê3é
		{
			USARTS[index]->CR1 &= ~((uint32_t)(1<<7));
		}
	}	
}

void UART4_IRQHandler(void)                    //串口1中断服务程序
{
//	u8 Res;
	OSIntEnter();
//LED=!LED;
	_USART_IRQHandler(3);
	
	OSIntExit();			
}
```

### 内存管理
内存分为大内存块和小内存块，数量和大小都可以在config.h里配置。
在获取内存或者释放内存时，会根据你需要的内存大小和地址自动分配和释放。
```c
void * MEM_Get(u16 size, u8 * perr)
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

u8 MEM_Put(void * pblk)
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
```

### 消息
每一个线程都有他自己的消息队列，消息会根据你需要发送的数据自动分配和释放内存。
```c
u8 MSG_Send(u8 task, u16 message, u16 para0, u16 para1, u8 * pData, u16 len)
{
    u8 err;
	Message_sut* pMessage=0;
	pMessage = (Message_sut*)MEM_Get(MEM_SIZE,&err);//(Message_sut*)OSMemGet(Mem, &err);
	if(pMessage){
		pMessage->head.message = message;
		pMessage->head.para0 = para0;
		pMessage->head.para1 = para1;
		pMessage->head.len = len;
		if(len>MEM_SIZE-sizeof(MessageHead_sut)){
			pMessage->data.pData = MEM_Get(len,&err);
			if(pMessage->data.pData){
				Debug_Printf(15, "MemGet:%d\r\n",err);
				memcpy(pMessage->data.pData,pData,len);
			}else{
				MEM_Put((void *) pMessage);
				return err;
			}
		}else{
			memcpy(pMessage->data.data8,pData,len);
		}
		err = OSQPost(QMessage[task], (void *)pMessage);
		if(err==OS_ERR_NONE){
			Debug_Printf(15, "s_M:%d,%d\r\n",task,pMessage->head.message);
		}else{
			MSG_Free(pMessage);
		}
		return err;
	}
	Debug_Printf(15, "s_M err:%d\r\n",err);
	return err;
}

Message_sut * MSG_Receive(u8 task)
{
	u8 err;
	Message_sut* pMessage=0;
	pMessage= (Message_sut *)OSQAccept(QMessage[task],&err);
	if(pMessage)Debug_Printf(15, "r_M:%d,%d\r\n",task,pMessage->head.message);
	return pMessage;
}

u8 MSG_Free(Message_sut* pMessage)
{
	u8 err;
	if(pMessage->head.len>MEM_SIZE-sizeof(MessageHead_sut))err = MEM_Put((void*)pMessage->data.pData);
	err = MEM_Put((void *) pMessage);
	if(err !=OS_ERR_NONE)Debug_Printf(15, "f_M err:%d\r\n",err);
	return err;
}
```
