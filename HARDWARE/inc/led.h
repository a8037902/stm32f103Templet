#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//LED端口定义


#define LED_RCC_GPIO RCC_APB2Periph_GPIOB
#define LED_GPIO GPIOB
#define LED_PIN GPIO_Pin_5
#define LED PBout(5)// PA8

void LED_Init(void);//初始化

		 				    
#endif
