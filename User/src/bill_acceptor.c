#include "bill_acceptor.h"
#include "CRC16.h"
#include "tool.h"

#define CODE_HEAD	0xFC

#define LONG_TIME			1000
#define NEXT_TIME			100

//status
#define ACCEPTOR_ENABLE						0x11
#define	ACCEPTOR_ACCEPTING				0x12
#define	ACCEPTOR_ESCROW						0x13
#define	ACCEPTOR_STACKING					0x14
#define	ACCEPTOR_VEND_VALID				0x15
#define	ACCEPTOR_STACKED					0x16
#define	ACCEPTOR_REJECTING				0x17
#define	ACCEPTOR_RETURNING				0x18
#define	ACCEPTOR_HOLDING					0x19
#define	ACCEPTOR_DISABLE					0x1A
#define	ACCEPTOR_INTIALIZE				0x1B
//power up status
#define	POWER_UP									0x40
#define POWER_UP_WITH_BILL_IN_ACCEPTOR	0x41
#define POWER_UP_WITH_BILL_IN_STACKER	0x42
//error status
#define ERROR_STACKER_FULL				0x43
#define ERROR_STACKER_OPEN				0x44
#define ERROR_JAM_IN_ACCEPTOR			0x45
#define ERROR_JAM_IN_STACKER			0x46
#define ERROR_PAUSE								0x47
#define ERROR_CHEATED							0x48
#define ERROR_FAILURE							0x49
#define ERROR_COMMUNICATION				0x4A
//poll request
#define ENQ												0x05
//reqponse to operation command
#define ACK												0x50
#define INVALID_COMMAND						0x4B

//controller->acceptor
#define STATUS_REQUEST						0x11
//operation command
#define CMD_RESET											0x40
#define CMD_STACK_1										0x41
//setting command
#define SET_CMD_EN								0xC0
#define SET_CMD_SECURITY					0xC1
#define SET_CMD_COM_MODE								0xC2
#define SET_CMD_INHIBIT								0xC3
#define SET_CMD_DIRECTION								0xC4
#define SET_CMD_OPT_FUN								0xC5
//setting status request
#define VERSION_REQUEST						0x88


u8 USART4_BUF[2][64];

//73h-79h
const u16 BILL_EURO_TABLE[]={5,10,20,50,100,200,500};
//61h-69h 
const u16 BILL_TABLE[][9]={
	{1,2,5,10,20,50,100,0,0},
	{0,20,50,100,200,500,1000,0,0},
};

const u8 COUNTRY_CODES_TABLE[][3]={
	{"CHN"},
	{"THA"}
};


typedef enum{
	STATE_UNKNOWN,
	STATE_REQUEST,
	STATE_ENABLE,
	STATE_ACCEPTING,
	STATE_ESCROW,
	STATE_STACKING,
	STATE_VEND_VALID,
	STATE_STACKED,
	STATE_REJECTING,
	STATE_RETURNING,
	STATE_HOLDING,
	STATE_DISABLE,
	STATE_INTIALIZE,
	STATE_ACK,
	STATE_OTHER,
	STATE_RESET,
	STATE_POWER_UP,
} Controller_StateDef;

static u16 _send(u8 *pData, u16 size){
	return Usart_SetData(USART_BILL,pData,size);
}

static u16 _receive(u8 *pData, u16 size){
	//u16 len = Usart_GetData(USART_BILL, pData, size);
	//Usart_SetData(USART_COMM,pData,len);
	return Usart_GetData(USART_BILL, pData, size);
}

typedef u16 (*Send_Fun)(u8 *pData, u16 size);
typedef u16 (*Receive_Fun)(u8 *pData, u16 size);

static Send_Fun g_send =_send;
static Receive_Fun g_receive =_receive;

static u8 g_buf[64];
static u8 g_len=0;

static Controller_StateDef controllerState=STATE_UNKNOWN;
static u8 acceptorState=0;
static u16 cnt=LONG_TIME;

static u8 countryCodes[3];
static u8 billTableIndex=0;

void bill_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	
	//Usart_Init(USART_BILL,9600,USART4_BUF[0],USART4_BUF[1],countof(USART4_BUF[0]));
	
	Usart_BufInit(USART_BILL,USART4_BUF[0],USART4_BUF[1],countof(USART4_BUF[0]));
	
	#if 1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = 9600;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART4, &USART_InitStructure); //初始化串口
	
//	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
//  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
//  USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
//  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

//  USART_ClockInit(UART4, &USART_ClockInitStructure);
			 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(UART4, ENABLE);                    //使能串口 
	#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = 9600;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 
	#endif
	
	
	
	
}

Res_TypeDef bill_acceptor_message_handle(u8* pBuf, u16 len){
	int i=0;
	Res_TypeDef res=RES_OK;
	if (len >= 5)
	{
		if (pBuf[0] == CODE_HEAD)
		{
			if(len>=pBuf[1]){
				u16 chk=0;
				//u8 * pCmdBuf=MemGet(MEM_BIG_SIZE,&err);
				//memcpy(pCmdBuf,usartBuf[USART_COMM],usartBuf[USART_COMM][3]+6);
				chk = CRC16_CCITT(pBuf,pBuf[1]-2);
				if((pBuf[pBuf[1]-2]|((u16)pBuf[pBuf[1]-1]<<8))==chk){
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
u16 bill_acceptor_clear_data(u16 len){
	u16 i=0;
	if(g_len<len)len=g_len;
	g_len -= len;
	for (i = 0; i < g_len; i++)
	{
		g_buf[i] = g_buf[i + len];
	}
	return len;
}

void bill_acceptor_send_message(u8 cmd, u8* pData,u8 len){
	//u8 i;
	u16 chk = 0;
	u8 * send_buf;
	u8 err;
	send_buf=MemGet(len+5,&err);

	send_buf[0] = CODE_HEAD;
	send_buf[1] = len+5;
	send_buf[2] = cmd;
	if(len>0)memcpy(send_buf+3,pData,len);
	chk = CRC16_CCITT(send_buf,len+3);
	send_buf[len + 3] = chk;
	send_buf[len + 4] = chk>>8;
	g_send(send_buf,len+5);
	if(MY_DEBUG>=5){
		//Usart_SetData(USART_USER,data,8);
		//Usart_SetData(USART_USER,data2,8);
	}
	MemPut(send_buf);
	
	cnt=LONG_TIME;
}

u16 bill_acceptor_process(){
	Res_TypeDef res;
	u16 i=0,j=0;
	u16 bill=0;
	u8 buf[2];
	g_len+=g_receive(g_buf+g_len,sizeof(g_buf)-g_len);

	while(1){
		res = bill_acceptor_message_handle(g_buf,g_len);
		if(res==RES_OK){
			acceptorState=g_buf[2];
			cnt=NEXT_TIME;
			//校验成功
			switch(acceptorState){
				
				case ACK://ack
					if(STATE_RESET==controllerState){
						//bill_acceptor_send_message(STATUS_REQUEST,0,0);
					}else{
						//controllerState=STATE_ACK;
					}
					break;
				case POWER_UP:
					//memset(buf,0,sizeof(buf));
					//controllerState=STATE_RESET;
					bill_acceptor_send_message(VERSION_REQUEST,0,0);
					controllerState=STATE_POWER_UP;
					break;
				case VERSION_REQUEST:
					memcpy(countryCodes,g_buf+5,sizeof(countryCodes));
					for(i=0;i<countof(COUNTRY_CODES_TABLE);i++){
						if(memcmp(COUNTRY_CODES_TABLE[i],countryCodes,sizeof(countryCodes))==0){
							billTableIndex=i;
							break;
						}
					}
					
					if(STATE_POWER_UP==controllerState){
						bill_acceptor_send_message(CMD_RESET,0,0);
						controllerState=STATE_RESET;
					}else{
						controllerState = STATE_INTIALIZE;
					}
					break;
				case ACCEPTOR_INTIALIZE:
					if(STATE_RESET==controllerState){
						memset(buf,0,sizeof(buf));
						bill_acceptor_send_message(SET_CMD_EN,buf,2);
					}
					break;
				case SET_CMD_EN:
					if(STATE_RESET==controllerState){
						memset(buf,0,sizeof(buf));
						bill_acceptor_send_message(SET_CMD_SECURITY,buf,2);
					}
					break;
				case SET_CMD_SECURITY:
					if(STATE_RESET==controllerState){
						memset(buf,0,sizeof(buf));
						bill_acceptor_send_message(SET_CMD_OPT_FUN,buf,2);
					}
					break;
				case SET_CMD_OPT_FUN:
					if(STATE_RESET==controllerState){
						memset(buf,0,sizeof(buf));
						bill_acceptor_send_message(SET_CMD_INHIBIT,buf,1);
					}
					break;
				case SET_CMD_INHIBIT:
					if(STATE_RESET==controllerState){
						controllerState = STATE_INTIALIZE;
					}
					break;
				case ACCEPTOR_DISABLE:
					memset(buf,0,sizeof(buf));
					bill_acceptor_send_message(SET_CMD_INHIBIT,buf,1);
					break;
				case INVALID_COMMAND://invalid command
					break;
				case ACCEPTOR_ENABLE:
					//state=STATE_ENABLE;
					if(STATE_UNKNOWN==controllerState){
						bill_acceptor_send_message(VERSION_REQUEST,0,0);
					}
					break;
				case ACCEPTOR_ACCEPTING:
					//state=STATE_ACCEPTING;
					break;
				case ACCEPTOR_ESCROW:
					//state=STATE_ESCROW;
					if(g_buf[3]>=0x73&&g_buf[3]<=0x79){
						bill = BILL_EURO_TABLE[g_buf[3]-0x73];
					}else if(g_buf[3]>=0x61&&g_buf[3]<=0x69){
						bill = BILL_TABLE[billTableIndex][g_buf[3]-0x61];
					}
					bill_acceptor_send_message(CMD_STACK_1,0,0);
					//cnt=LONG_TIME;
					break;
				case ACCEPTOR_STACKING:
					//state=STATE_STACKING;
					break;
				case ACCEPTOR_STACKED:
					//state=STATE_STACKED;
					break;
				case ACCEPTOR_VEND_VALID:
					//state=STATE_VEND_VALID;
					bill_acceptor_send_message(ACK,0,0);
					break;
				default:
					//state=STATE_OTHER;
					break;
			}
			
			j=g_buf[1];
			bill_acceptor_clear_data(j);
		}else if(res==RES_CHECK_FAIL){
			//校验失败
			j = get_byte_index(g_buf,1,g_len,CODE_HEAD);
			bill_acceptor_clear_data(j);
		}else if(res==RES_LENGHT_ERR){
			//数据没有接收完
			
			break;
		}else if(res==RES_HEAD_ERR){
			//头部错误
			j = get_byte_index(g_buf,1,g_len,CODE_HEAD);
			bill_acceptor_clear_data(j);
		}
	}
	
	switch(controllerState){
		case STATE_UNKNOWN:
			if(cnt<=0){
				//3s
				//cnt=LONG_TIME;
				bill_acceptor_send_message(STATUS_REQUEST,0,0);
			}
			break;
		default:
			if(cnt<=0){
				//3s
				//cnt=LONG_TIME;
				//controllerState=STATE_REQUEST;
				bill_acceptor_send_message(STATUS_REQUEST,0,0);
			}
			break;
	}
	
	if(cnt)cnt--;
	return bill;
}
