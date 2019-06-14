#include "usart_loop_module.h"
#include "usart_module.h"
#include "message.h"

#define USART_LOOP		0


static u8 usartBuf[64];
static u16 usartLen=0;

void UsartLoop_ModuleInit()
{
}

void UsartLoop_ModuleProcess0(unsigned short int period)
{
	int i = Usart_ModuleGetData(USART_LOOP, usartBuf+usartLen, sizeof(usartBuf)-usartLen);
	MSG_Send(0,MESSAGE_USART_LOOP,0,0,usartBuf+usartLen,i);
}

unsigned short int UsartLoop_ModuleHandle0(Message_sut * pMessage)
{
	if(pMessage->head.message==MESSAGE_USART_LOOP){
		if(pMessage->head.len>MEM_SIZE-sizeof(MessageHead_sut)){
			Usart_ModuleSetData(USART_LOOP, pMessage->data.pData,pMessage->head.len);
		}else{
			Usart_ModuleSetData(USART_LOOP, pMessage->data.data8,pMessage->head.len);
		}
		return 1;
	}
	return 0;
}
