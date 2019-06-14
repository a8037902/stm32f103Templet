#include "msg_test_module.h"
#include "usart_module.h"
#include "message.h"

#if SYSTEM_SUPPORT_UCOS==1
extern OS_MEM * Mem;
extern OS_MEM * MemBig;

extern OS_EVENT * QMessage[TASK_NUM];
#endif

static unsigned short int s_period=0;

void MsgTest_ModuleInit(void)
{
	
}
void MsgTest_ModuleProcess1(unsigned short int period)
{
	s_period+=period;
	if(s_period>=1000){
		s_period-=1000;
		MSG_Send(0,MESSAGE_MSG_TEST,OSTime,OSCPUUsage,(u8*)&OSTime,sizeof(OSTime));
	}
}

unsigned short int MsgTest_ModuleHandle0(Message_sut * pMessage)
{
	if(pMessage->head.message==MESSAGE_MSG_TEST){
		u8 i=0;
		u8 err;
		void *pData=0;
		char* pBuf=MEM_Get(MEM_SIZE,&err);
		
		if(!pBuf)return 1;
		
		sprintf(pBuf,"OSTime:%d\r\n",pMessage->data.data32[0]);
		Usart_ModuleSetData(0, (u8*)pBuf,strlen(pBuf));
		sprintf(pBuf,"OSCPUUsage:%d\r\n",pMessage->head.para1);
		Usart_ModuleSetData(0, (u8*)pBuf,strlen(pBuf));
		
		pData=MEM_Get(sizeof(OS_Q_DATA),&err);
		if(pData){
			for(i=0;i<TASK_NUM;i++){
				if(OS_ERR_NONE==OSQQuery(QMessage[i],pData)){
					sprintf(pBuf,"Message:Task=%d,Num=%d\r\n",i,((OS_Q_DATA*)pData)->OSQSize-((OS_Q_DATA*)pData)->OSNMsgs);
					Usart_ModuleSetData(0, (u8*)pBuf,strlen(pBuf));
				}
			}
			MEM_Put(pData);
		}
		
		pData=MEM_Get(sizeof(OS_MEM_DATA),&err);
		if(pData){
			if(OS_ERR_NONE==OSMemQuery(Mem,pData)){
				sprintf(pBuf,"Mem:Num=%d\r\n",((OS_MEM_DATA*)pData)->OSNFree);
				Usart_ModuleSetData(0, (u8*)pBuf,strlen(pBuf));
			}
			if(OS_ERR_NONE==OSMemQuery(MemBig,pData)){
				sprintf(pBuf,"MemBig:Num=%d\r\n",((OS_MEM_DATA*)pData)->OSNFree);
				Usart_ModuleSetData(0, (u8*)pBuf,strlen(pBuf));
			}
			MEM_Put(pData);
		}
		MEM_Put(pBuf);
		
		return 1;
	}
	return 0;
}
