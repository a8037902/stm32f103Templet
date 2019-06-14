#include "upper_computer.h"


static u16 _send(u8 *pData, u16 size){
	return Usart_SetData(USART_COMM,pData,size);
}

static u16 _receive(u8 *pData, u16 size){
	return Usart_GetData(USART_COMM, pData, size);
}

typedef u16 (*Send_Fun)(u8 *pData, u16 size);
typedef u16 (*Receive_Fun)(u8 *pData, u16 size);

static Send_Fun g_send =_send;
static Receive_Fun g_receive =_receive;

Res_TypeDef upper_computer_message_handle(u8* pBuf, u16 len){
	int i=0;
	Res_TypeDef res=RES_OK;
	if (len >= CMD_MIN_LEN)
	{
		if (pBuf[0] == CMD_HEAD)
		{
			if(len>=pBuf[CMD_DATA_LEN_INDEX]+CMD_MIN_LEN){
				u8 chk=0;
				//u8 * pCmdBuf=MemGet(MEM_BIG_SIZE,&err);
				//memcpy(pCmdBuf,usartBuf[USART_COMM],usartBuf[USART_COMM][3]+6);
				
				for (i = 1; i < pBuf[CMD_DATA_LEN_INDEX]+4; i++)
				{
					chk += pBuf[i];
				}
				if(pBuf[pBuf[CMD_DATA_LEN_INDEX]+4]==chk){
					//校验成功
					res=RES_OK;
				}else
				{
					//校验失败
					res=RES_CHECK_FAIL;
				}
				//MemPut(pCmdBuf);
			}else{
				//数据没有接收完
				res=RES_LENGHT_ERR;
			}
		}
		else
		{
			//头部错误
			res=RES_HEAD_ERR;
		}
	}else{
		res=RES_LENGHT_ERR;
	}
	return res;
}

void upper_computer_send_message(u8 cmd, u8* pData,u8 len){
	u8 i;
	u8 check_code = 0;
	u8 * send_buf;
	u8 err;
	send_buf=MemGet(len+CMD_MIN_LEN,&err);

	send_buf[0] = CMD_HEAD;
	send_buf[1] = 0x00;
	send_buf[2] = cmd;
	send_buf[3] = len;
	if(len>0)memcpy(send_buf+4,pData,len);
	for(i = 1; i < len + 4; i++) {
		check_code += send_buf[i];
	}
	send_buf[len + 4] = check_code;
	send_buf[len + 5] = 0xfb;
	#if DES_SUPPORT==1
	des(data,(u8*)DES_KEY,0,data2);
	#endif
	g_send(send_buf,len+CMD_MIN_LEN);
	if(MY_DEBUG>=5){
		//Usart_SetData(USART_USER,data,8);
		//Usart_SetData(USART_USER,data2,8);
	}
	MemPut(send_buf);
}
