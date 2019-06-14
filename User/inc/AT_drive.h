#ifndef __AT_DRIVE_H
#define __AT_DRIVE_H	 

#include "sys.h"

typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	
typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID     = 5,
} ENUM_ID_NO_TypeDef;

u8 my_strstr(const char *s0,const char *s1,u16 len);
u16 AT_SendData(u8 *pData, u16 size,BOOL debug);
u16 AT_ReceiveData(u8 *pData, u16 size,BOOL debug);

u8 AT_Cmd ( u8 *pBuf, u16 size, char * cmd, char * reply1, char * reply2, u16 waittime);
u8 AT_Test ( u16 time);
//u8 AT_Link_Server (u16 time, ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
//u8 AT_SendString (u16 time, FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );

#endif
