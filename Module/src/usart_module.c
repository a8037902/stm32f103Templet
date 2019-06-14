#include "usart_module.h"


//´®¿Ú»º³å
static u8 USART1_BUF[2][256];


void Usart_ModuleInit()
{
	Usart_Init(0,115200,USART1_BUF[0],USART1_BUF[1],countof(USART1_BUF[0]));
}

u16 Usart_ModuleGetData(int usart, u8 *pData, u16 size)
{
	return Usart_GetData(usart, pData, size);
}

u16 Usart_ModuleSetData(int usart, u8 *pData, u16 size)
{
	return Usart_SetData(usart, pData, size);
}
