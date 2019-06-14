#ifndef __USART_LOOP_MODULE_H
#define __USART_LOOP_MODULE_H
#include "sys.h"

void UsartLoop_ModuleInit(void);
void UsartLoop_ModuleProcess0(unsigned short int period);

unsigned short int UsartLoop_ModuleHandle0(Message_sut * pMessage);

#endif
