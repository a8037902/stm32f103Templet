#include "AT_drive.h"

#define DELAY_TIME		20

extern u16 DebugLevel;

u16 AT_SendData(u8 *pData, u16 size,BOOL debug)
{
	if(debug && DebugLevel>=5){
		Usart_SetData(0,pData,size);
	}
	return Usart_SetData(1,pData,size);
}

u16 AT_ReceiveData(u8 *pData, u16 size,BOOL debug)
{
	u16 r=0;
	r = Usart_GetData(1,pData,size);
	if(debug && DebugLevel>=5){
		Usart_SetData(0,pData,r);
	}
	return r;
}

u16 AT_Log(u8 *pData, u16 size)
{
	return 0;//Usart_SetData(0,pData,size);
}

u8 my_strstr(const char *s0,const char *s1,u16 len)
{
	int i,j;
	j=strlen(s1);
	if(len<j)return 0;
	for(i=0;i<=len-j;i++){
		if(memcmp(s0+i,s1,j)==0)
			return 1;
	}
	return 0;
}

/*
 * 函数名：AT_Cmd
 * 描述  ：发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */

u8 AT_Cmd ( u8 *pBuf, u16 size, char * cmd, char * reply1, char * reply2, u16 waittime)
{    
	u16 l0=0,l1=0,cnt=waittime/DELAY_TIME+1;
	u8 r=0;
	//u8 buf[128+1];

	if(cmd){
		AT_SendData((u8*)cmd,strlen(cmd),FALSE);
		AT_SendData((u8*)"\r\n",2,FALSE);
	}

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return r;
	
	//delay_ms ( DELAY_TIME );                 //延时
	
	while(cnt ||  l0>l1){
		cnt--;
		l1=l0;
		l0 += AT_ReceiveData(pBuf+l0,size-l0-1,TRUE);
		if(l0>l1){
			pBuf[l0]=0;
			if ( reply1 != 0 ){
				if (my_strstr ( (const char*)pBuf, reply1 ,l0) ){
					r = 1;
					break;
				}
			
			}
			
			if(reply2 != 0){
				if(my_strstr ( (const char*)pBuf, reply2 ,l0) ){
					r = 2;
					break;
				}
			}
		}
		
		delay_ms ( DELAY_TIME );                 //延时
	}
	
	return r;
}

/*
 * 函数名：AT_Test
 * 描述  ：对AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
u8 AT_Test ( u16 time )
{
	//delay_ms ( 1500 ); 
	u8 buf[64];
	return AT_Cmd ( buf,64,"AT", "OK", NULL, time);  	
}

/*
 * 函数名：AT_Enable_MultipleId
 * 描述  ：模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
//u8 AT_Enable_MultipleId (u16 time, FunctionalState enumEnUnvarnishTx )
//{
//	char cStr [20];
//	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
//	return AT_Cmd ( cStr, "OK", 0, time, TRUE );
//}

/*
 * 函数名：AT_Link_Server
 * 描述  ：模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
//u8 AT_Link_Server (u16 time, ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
//{
//	char cStr [100] = { 0 }, cCmd [120];
//	switch (  enumE )
//	{
//		case enumTCP:
//		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
//		  break;
//		case enumUDP:
//		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
//		  break;
//		default:
//			break;
//	}

//	if ( id < 5 )//多连接
//		sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);    ///AT+CIPSTART="TCP","ip","80"
//	else
//		sprintf ( cCmd, "AT+CIPSTART=%s", cStr );//单连接
//	return AT_Cmd ( cCmd, "OK", "ALREAY CONNECT", time, TRUE );
//}

/*
 * 函数名：AT_SendString
 * 描述  ：模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
 */
//u8 AT_SendString (u16 time, FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
//{
//	char cStr [20];
//	u8 bRet = 0;
//		
//	if ( enumEnUnvarnishTx ){
//		AT_SendData((u8*)pStr,ulStrLength);
//	}else{
//		if ( ucId < 5 )
//			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );
//		else
//			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
//		
//		AT_Cmd ( cStr, "> ", 0, 250, TRUE );

//		bRet = AT_Cmd ( pStr, "OK", 0, time, TRUE );
//	}
//	return bRet;
//}


/*
 * 函数名：AT_ReceiveString
 * 描述  ：WF-ESP8266模块接收字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 * 返回  : 接收到的字符串首地址
 * 调用  ：被外部调用
 */
//u8 BufRec[USART_BUF_SIZE+1];
//char * AT_ReceiveString ( FunctionalState enumEnUnvarnishTx )
//{
//	char * pRecStr = 0;
//	u8 BufRec[USART_BUF_SIZE+1];
//	u16 l=0;
//	l = AT_ReceiveData(BufRec,sizeof(BufRec)-1);
//	if(l>0){
//		delay_ms(10);
//		l+= AT_ReceiveData(BufRec+l,sizeof(BufRec)-1-l);
//		BufRec[l]='\0';
//		if ( enumEnUnvarnishTx ){
//	//		if ( strstr ( (const char*)BufRec, ">" ) )
//				pRecStr = (char*)BufRec;
//		}else {
//			if ( strstr ( (const char*)BufRec, "+IPD" ) )
//				pRecStr = (char*)BufRec;
//		}
//	}
//	return pRecStr;
//}
