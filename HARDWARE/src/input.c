#include "input.h"
#include "sys.h"

void input_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//PE2~4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
}
//32-25 null 24-17 pb5:pc0-pc8 16-9 pb13:pc0-pc8 8-1 pb15:pc0-pc8

GPIO_TypeDef* INPUT_GPIO[]={GPIOA,GPIOE,GPIOE,GPIOE};
const uint16_t INPUT_PIN[]={GPIO_Pin_0,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4};

u32 input_Data()
{
	u16 i=0;
	u32 in=0;
	//unsigned int temp=0;
	
	for(i=0;i<countof(INPUT_PIN);i++){
		in <<= 1;
		if(GPIO_ReadInputDataBit(INPUT_GPIO[i],INPUT_PIN[i])){
			in |= 0x01;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)){
		in |= 0x01;
	}
	
	return in;
}

