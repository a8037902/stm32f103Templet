#include "ESP8266.h"
#include "userIO.h"

/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
u8 buf[USART_BUF_SIZE+1];
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime, bool log)
{    
	int l,cnt=waittime;
	Usart_SetData(COM_WIFI,(u8*)cmd,strlen(cmd));
	Usart_SetData(COM_WIFI,(u8*)"\r\n",2);

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	delay_ms ( DELAY_TIME );                 //延时
	
	l = Usart_GetData(COM_WIFI,buf,sizeof(buf)-1);
	buf[l]=0;
  
	do{
		if ( ( reply1 != 0 ) && ( reply2 != 0 ) ){
			if( ( bool ) strstr ( (const char*)buf, reply1 ) || ( bool ) strstr ( (const char*)buf, reply2 ) ){
				if(log)UserIO_Send(buf,l);//发送到电脑端
				return true;
			}
		}else if ( reply1 != 0 ){
			if ( ( bool ) strstr ( (const char*)buf, reply1 ) ){
				if(log)UserIO_Send(buf,l);//发送到电脑端
				return true;
			}
		
		}else{
			if( ( bool ) strstr ( (const char*)buf, reply2 ) ){
				if(log)UserIO_Send(buf,l);//发送到电脑端
				return true;
			}
		}
		delay_ms ( DELAY_TIME );                 //延时
		l += Usart_GetData(COM_WIFI,buf+l,sizeof(buf)-l-1);
		buf[l]=0;
	}while(cnt--);
	if(log)UserIO_Send(buf,l);//发送到电脑端
	return false;
}

/*
 * 函数名：ESP8266_Rst
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被ESP8266_AT_Test调用
 */
void ESP8266_Rst ( void )
{
	ESP8266_Cmd ( "AT+RST", "OK", "ready", DELAY_CNT_1, true);   	
}

/*
 * 函数名：ESP8266_AT_Test
 * 描述  ：对WF-ESP8266模块进行AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AT_Test ( void )
{
	delay_ms ( 1500 ); 
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, DELAY_CNT_0, true) ) ESP8266_Rst ();  	
}

/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", DELAY_CNT_1, true); 
		
		case AP:
			return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", DELAY_CNT_1, true); 
		
		case STA_AP:
			return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", DELAY_CNT_1, true); 
		
		default:
		  return false;
	}
}

/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, DELAY_CNT_1, true );
	
}

/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：WF-ESP8266模块创建WiFi热点
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 *       ：enunPsdMode，WiFi加密方式代号字符串
 * 返回  : 1，创建成功
 *         0，创建失败
 * 调用  ：被外部调用
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, DELAY_CNT_1, true );
	
}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cStr, "OK", 0, DELAY_CNT_1, true );
	
}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )//多连接
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);    ///AT+CIPSTART="TCP","ip","80"

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );//单连接

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", DELAY_CNT_1, true );
	
}

/*
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, DELAY_CNT_1, true ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, DELAY_CNT_1, true ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, DELAY_CNT_1, true );
	}
	
}


/*
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_UnvarnishSend ( void )
{
	return (
	  ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, DELAY_CNT_1, true ) &&
	  ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", DELAY_CNT_1, true ) );	
}

/*
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
		
	if ( enumEnUnvarnishTx ){
		Usart_SetData(COM_WIFI,(u8*)pStr,ulStrLength);
	}else{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );
		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, DELAY_CNT_1, true );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, DELAY_CNT_1, true );
	}
	
	return bRet;

}

/*
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块退出透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_ExitUnvarnishSend ( void )
{
	return ESP8266_SendString(ENABLE,"++",2,Single_ID);
	
}


/*
 * 函数名：ESP8266_ReceiveString
 * 描述  ：WF-ESP8266模块接收字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 * 返回  : 接收到的字符串首地址
 * 调用  ：被外部调用
 */
u8 BufRec[USART_BUF_SIZE+1];
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	int l=0;
	l = Usart_GetData(COM_WIFI,BufRec,sizeof(BufRec)-1);
	if(l>0){
		delay_ms(10);
		l+= Usart_GetData(COM_WIFI,BufRec+l,sizeof(BufRec)-1-l);
		BufRec[l]='\0';
		if ( enumEnUnvarnishTx )
		{
	//		if ( strstr ( (const char*)BufRec, ">" ) )
				pRecStr = (char*)BufRec;

		}
		
		else 
		{
			if ( strstr ( (const char*)BufRec, "+IPD" ) )
				pRecStr = (char*)BufRec;

		}
	}
	
	
	

	return pRecStr;
}


/*
 * 函数名：ESP8266_WebFetch_Test
 * 描述  ：WF-ESP8266模块进行抓取网页测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_init( void )
{
//	char cStrInput [100] = { 0 }, * pStrDelimiter [2], * pBuf, * pStr;
//	u8 uc = 0;

	//ESP8266_Choose ( ENABLE );	


	ESP8266_AT_Test ();
	

	ESP8266_Net_Mode_Choose ( STA );
	
	
	//ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 10000, true );


//  do
//	{
//		PC_Usart ( "\r\n请输入能连接到Internet的WiFi名称和密钥，输入格式为：名称字符+英文逗号+密钥字符+空格，点击发送\r\n" );

//		scanf ( "%s", cStrInput );

//		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

//		pBuf = cStrInput;
//		uc = 0;
//		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
//		{
//			pStrDelimiter [ uc ++ ] = pStr;
//			pBuf = NULL;
//		} 
//		
//  } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );
//	ESP8266_Cmd ( "AT+CIPMUX=0", "OK", 0, 500 );            //单连接 
//  do
//	{
//		PC_Usart ( "\r\n连接到WiFi\r\n" );

//  } while ( ! ESP8266_JoinAP ( "LieBao", "12345678") );

}
/*****************************************************************************************************/
/*从服务器下载数据*/
//void Web_To_ESP(void)
//{ 	
//	u16 i;
//	bool state;
////	ESP8266_Cmd ( "AT", "OK", NULL, 200 );
//	ESP8266_Cmd ( "AT+CIPMUX=0", "OK", 0, 500 );            //单连接 
//	Delay_ms(1000);
//	do
//	{
//		strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包
//		for(i=0;i<2048;i++)
//			strEsp8266_Fram_Record .Data_RX_BUF[i]=0;                //清空数组	
//		ESP8266_Usart ( "%s\r\n", "AT+CIPSTART=\"TCP\",\"192.168.191.2\",8080" );	
//		Delay_ms ( 1000);                 //延时	
//		strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
//		PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
//      state= (( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "OK" )|( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ALREAY CONNECT" ));
//		if(( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ERROR" ))
//			state=false; 		
//	}
//	while(state==false);
//    ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 );            //0,非透传；1，透传
//	ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 );
//	ESP8266_SendString ( ENABLE, "GET /LibrarySeatServer/seatInfo?room=1 HTTP/1.1\r\n",     NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "Host: 192.168.191.2:8080\r\n",                            NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "User-Agent: abc\r\n",                                     NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "Connection: close\r\n",                                   NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "\r\n",                                                    NULL, Single_ID );
//	strEsp8266_Fram_Record .InfBit .FramLength = 0;
//	Delay_ms(1200);
//	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
//	PC_Usart ("%s",strEsp8266_Fram_Record .Data_RX_BUF);
//}	
///***************************************************************************************/
////登陆或下线函数，第一次刷登陆，第二次刷下线
////参数：usr_Id             如1,2,3,4,5,.......
//void login(u8 usr_Id)
//{
//	char cStr[100]={0};
//	u16 i;
//	bool state;
//	switch(usr_Id)
//	{
//		case 1:
//			sprintf ( cStr, "GET /LibrarySeatServer/usrInfo?%s HTTP/1.1\r\n", "usr=june&password=19920211&req_type=SWIPE" );//用户june
//			break;
//		case 2:
//			sprintf ( cStr, "GET /LibrarySeatServer/usrInfo?%s HTTP/1.1\r\n", "usr=solar&password=19920211&req_type=SWIPE" );//用户solar
//			break;
//		default:
//			break;
//	}
//	
//	ESP8266_Cmd ( "AT+CIPMUX=0", "OK", 0, 500 );//单连接
//	Delay_ms(500);
//	do
//	{
//		strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包
//		for(i=0;i<1024;i++)
//			strEsp8266_Fram_Record .Data_RX_BUF[i]=0;                //清空数组
//		ESP8266_Usart ( "%s\r\n", "AT+CIPSTART=\"TCP\",\"192.168.191.3\",8080");		
//		Delay_ms ( 1000);                 //延时	
//		strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
//		PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
//      state= ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "OK" )||( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ALREAY CONNECT" );
//		if(( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ERROR" ))
//			state=false; 
//	
//	}
//	while(state==false);
//    ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 );            //0,非透传；1，透传
//	ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 );
//	ESP8266_SendString ( ENABLE, cStr,                                NULL, Single_ID );//请求行
//	ESP8266_SendString ( ENABLE, "Host: 192.168.191.3:8080\r\n",      NULL, Single_ID );//以下为请求头
//	ESP8266_SendString ( ENABLE, "User-Agent: abc\r\n",               NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "Content-Length: 38\r\n",            NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "Connection: close\r\n",             NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "\r\n",                              NULL, Single_ID );
//	strEsp8266_Fram_Record .InfBit .FramLength = 0;
//	Delay_ms(3000);
//	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
//	PC_Usart ("%s",strEsp8266_Fram_Record .Data_RX_BUF);
//	PC_Usart ("\r\n");
//	if((strstr(strEsp8266_Fram_Record .Data_RX_BUF,"OK"))!=NULL)
//		PC_Usart ("log in successfully\r\n");
//}	

///*************************************************************************************************************/
////reqType：请求类型，1直接请求某张桌子座位信息，2发送占座请求
////desk：桌子ID
////room：馆ID 
////seat：座位ID
////actionType：动作类型，直接等于occupy。
////usr：用户ID
//void occupy(char desk_ID,char room_ID,char seat_ID,char usr_ID)
//{
//	char cStr[100]={0};
//	u16 i;
//	bool state;
//	sprintf ( cStr, "GET /LibraryDeskServer/deskInfo?reqType=2&desk=%d&room=%d&seat=%d&actionType=occupy&usr=%d HTTP/1.1\r\n",desk_ID,room_ID,seat_ID,usr_ID);			
//	ESP8266_Cmd ( "AT+CIPMUX=0", "OK", 0, 500 );//单连接
//	Delay_ms(500);
//	do
//	{
//		strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包
//		for(i=0;i<1024;i++)
//			strEsp8266_Fram_Record .Data_RX_BUF[i]=0;                //清空数组
//		sprintf(array,"AT+CIPSTART=\"TCP\",\"%s\",8080",IP);
//		ESP8266_Usart ( "%s\r\n", array );		
//		Delay_ms ( 1000);                 //延时	
//		strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
//		PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
//      state= ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "OK" )||( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ALREAY CONNECT" );
//		if(( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ERROR" ))
//			state=false; 
//	
//	}
//	while(state==false);
//	sprintf(array,"Host: %s:8080\r\n",IP);
//    ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 );            //0,非透传；1，透传
//	ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 );
//	ESP8266_SendString ( ENABLE, cStr,                                NULL, Single_ID );//请求行
//  ESP8266_SendString ( ENABLE, array,                               NULL, Single_ID );//以下为请求头
//	ESP8266_SendString ( ENABLE, "User-Agent: abc\r\n",               NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "Content-Length: 38\r\n",            NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "Connection: close\r\n",             NULL, Single_ID );
//	ESP8266_SendString ( ENABLE, "\r\n",                              NULL, Single_ID );
//	strEsp8266_Fram_Record .InfBit .FramLength = 0;
//	Delay_ms(1500);
//	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
//	if((strstr(strEsp8266_Fram_Record .Data_RX_BUF,"[{"))!=NULL)
//		PC_Usart ("occupy successfully\r\n");
//	else 
//		PC_Usart ("failed\r\n");


//}
