#include "Encode.h"
#include "types.h"
#include "DES.h"

extern const u8 DES_KEY[8];

void send_MessageToMaster(u8* pData){
	u8 data[8];
	u8 data2[8];
	int chk = 0;
	u8 i=0;
	memset(data,0,sizeof(data));
	data[0]=0xaa;
	memcpy(data+1,pData,5);
	
	for (i = 0; i < 6; i++)
	{
		chk += data[i];
	}
	data[6] = (u8)chk;
	data[7] = (u8)(chk>>8);
	#if DES_SUPPORT==1
	des(data,(u8*)DES_KEY,0,data2);
	#endif
	Usart_SetData(USART_COMM,data2,8);
	if(MY_DEBUG>=5){
		//Usart_SetData(USART_USER,data,8);
		//Usart_SetData(USART_USER,data2,8);
	}
}
