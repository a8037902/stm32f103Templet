#include "at_module.h"
#include "usart_module.h"
#include "at.h"

#define AT_USART	0

void AT_ModuleInit(void)
{
}
void AT_ModuleProcess0(unsigned short int period)
{
	u8 usartBuf[64];
	u16 usartLen=0;
	usartLen += Usart_ModuleGetData(AT_USART, usartBuf+usartLen, sizeof(usartBuf)-usartLen);
	at_recvTask(usartBuf,usartLen);
}
