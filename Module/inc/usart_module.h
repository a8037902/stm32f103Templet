#ifndef __USART_MODULE_H
#define __USART_MODULE_H
#include "sys.h"

void Usart_ModuleInit(void);

u16 Usart_ModuleGetData(int usart, u8 *pData, u16 size);
u16 Usart_ModuleSetData(int usart, u8 *pData, u16 size);

#endif
