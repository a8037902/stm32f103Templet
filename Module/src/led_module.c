#include "led_module.h"
#include "led.h"

static unsigned short int s_period=0;
static unsigned char s_state=0;

void LED_ModuleInit(void)
{
	LED_Init();
}
void LED_ModuleProcess0(unsigned short int period)
{
	if(s_state==0){
		s_period+=period;
		if(s_period>=500){
			s_period=0;
			LED=!LED;
		}
	}
}

void LED_ModuleOnOff(void)
{
	if(s_state)s_state=0;
	else s_state=1;
}