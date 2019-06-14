#include "stm32f10x.h"
#include "led.h"

							  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PA8和PD2为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_RCC_GPIO, ENABLE);	 //使能PA端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_PIN;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	GPIO_SetBits(LED_GPIO,LED_PIN);						 //PA.8 输出高
	
 
// GPIO_InitTypeDef  GPIO_InitStructure;
// 	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
//	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 端口配置
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOA, &GPIO_InitStructure);
// GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 输出高


// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);   //使能PD端口时钟

// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 端口配置
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
// GPIO_Init(GPIOD, &GPIO_InitStructure);	
// GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 输出高 
}

//void LED_Init(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			 
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);
//}
 
