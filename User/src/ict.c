#include "ict.h"
#include "myiic.h"
#include "types.h"

const u16 ICT_PLUSE_NUM[ICT_BILL_TYPE_NUM]={1,5,10,20,50,100,0,0};
const u16 ICT_SUBMIT_NUM[ICT_BILL_TYPE_NUM]={1,1,1,1,1,1,1,1};

static u8 Count=0;
static u8 Block=0;

u8 send_Message(u8 task, u16 message, u16 target, u8 * pData, u16 len);

void ict_usart_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	Usart_BufInit(0);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	 
     //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//??????
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//Usart1 NVIC ??

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//????3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????
	NVIC_Init(&NVIC_InitStructure);	//??????????VIC???

	//USART ?????

	USART_InitStructure.USART_BaudRate = bound;//?????9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//???8?????
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//?????
	USART_InitStructure.USART_Parity = USART_Parity_Even;//??????
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//????????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//????

	USART_Init(USART1, &USART_InitStructure); //?????
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//????
	USART_Cmd(USART1, ENABLE);                    //???? 
}

void ict_bill_clear()
{
}

void ict_bill_Read(ICT_Bill_sut *pBill)
{
	u8 i=0,j=0;
	u16 check=0;
	u8 buf[ICT_EEPROM_BLOCK_SIZE];
	memset(pBill,0,sizeof(ICT_Bill_sut));
	memset(buf,0,ICT_EEPROM_BLOCK_SIZE);
	for(i=0;i<ICT_EEPROM_BLOCK_NUM;i++){
		I2CReadByte(ICT_EEPROM_OFF+i*ICT_EEPROM_BLOCK_SIZE,buf,ICT_EEPROM_BLOCK_SIZE);
		////for(j=0;j<ICT_EEPROM_BLOCK_SIZE/5;j++)Debug_Printf(6, "rB:%d,%d,%d,%d,%d\r\n",buf[j*5],buf[j*5+1],buf[j*5+2],buf[j*5+3],buf[j*5+4]);
		Debug_Printf(6, "\r\n");
		if(buf[0]&&buf[0]>Count){//count为目前读取的最大
			//Debug_Printf(6, "1\r\n");
			check=0;
			for(j=0;j<ICT_EEPROM_BLOCK_SIZE-2;j++)check+=buf[j];
			//Debug_Printf(6, "check:%d=%d\r\n",check,*((u16*)(buf+ICT_EEPROM_BLOCK_SIZE-2)));
			if(check == *((u16*)(buf+ICT_EEPROM_BLOCK_SIZE-2))){//校验和
				Debug_Printf(6, "2\r\n");
				//pBill->count=buf[0];
				Count=buf[0];
				pBill->frame=buf[1];
				memcpy(pBill->bill,buf+2,sizeof(pBill->bill));
				//pBill->block=i;
				Block=i+1;
				if(Block>=ICT_EEPROM_BLOCK_NUM)Block=0;
			}
		}
	}
}


void ict_bill_Write(ICT_Bill_sut *pBill)
{
	u8 i=0,j=0;
	u16 check=0;
	u8 buf[ICT_EEPROM_BLOCK_SIZE+4];
	((u16*)buf)[0]=ICT_EEPROM_OFF+Block*ICT_EEPROM_BLOCK_SIZE;
	((u16*)buf)[1]=ICT_EEPROM_BLOCK_SIZE;
	memset(buf+4,0,ICT_EEPROM_BLOCK_SIZE+4);
	if(++Count==0)Count=1;
	
	buf[4]=Count;
	buf[5]=pBill->frame;
	//Debug_Printf(6, "fw%d\r\n",buf[5]);
	memcpy(buf+6,pBill->bill,sizeof(pBill->bill));
	for(j=0;j<ICT_EEPROM_BLOCK_SIZE-2;j++)check+=buf[4+j];
	buf[ICT_EEPROM_BLOCK_SIZE+2]=check&0x00ff;
	buf[ICT_EEPROM_BLOCK_SIZE+3]=(check>>8)&0x00ff;
	send_Message(TASK_EEPROM_WRITE, MESSAGE_EEPROM_WRITE, ICT_EEPROM_OFF+Block*ICT_EEPROM_BLOCK_SIZE, buf, ICT_EEPROM_BLOCK_SIZE+4);
	if(Count==1){//清除其他区块
		
		memset(buf,0,ICT_EEPROM_BLOCK_SIZE+4);
		for(i=0;i<ICT_EEPROM_BLOCK_NUM;i++){
			if(i!=Block){
				((u16*)buf)[0]=ICT_EEPROM_OFF+i*ICT_EEPROM_BLOCK_SIZE;
				((u16*)buf)[1]=ICT_EEPROM_BLOCK_SIZE;
				send_Message(TASK_EEPROM_WRITE, MESSAGE_EEPROM_WRITE, ICT_EEPROM_OFF+i*ICT_EEPROM_BLOCK_SIZE, buf, ICT_EEPROM_BLOCK_SIZE+4);
			}
		}
	}

	if(++Block>=ICT_EEPROM_BLOCK_NUM)Block=0;
}