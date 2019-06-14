#ifndef __MSG_TEST_MODULE_H
#define __MSG_TEST_MODULE_H
#include "sys.h"

void MsgTest_ModuleInit(void);
void MsgTest_ModuleProcess1(unsigned short int period);

unsigned short int MsgTest_ModuleHandle0(Message_sut * pMessage);


#endif
