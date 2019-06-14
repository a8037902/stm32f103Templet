#include "input_module.h"
#include "input.h"
#include "tool.h"
#include "led_module.h"

#define HEART_INTERVAL	(1000/OS_TICKS_PER_SEC)

typedef enum {OPEN = 0, CLOSE = !OPEN} PLUSE_MODE;

typedef enum{
	INPUT_IDLE=0,
	INPUT_DOWN,
	INPUT_LONG_DOWN,
	INPUT_VIBRATE,
	INPUT_VIBRATE2,
	INPUT_VIBRATE3,
} Input_TypeDef;

typedef struct _Input {
	unsigned short int inputOffTime;
	unsigned short int inputLongTime;
	unsigned short int inputTime;
	PLUSE_MODE mode;
	Input_TypeDef state;
	char keyCode;
} Input_sut;

s8 Input_Handle(Input_sut * pInput,int d){
	if ((d && pInput->mode==CLOSE) || (!d && pInput->mode==OPEN)) {
		if(pInput->state==INPUT_IDLE){
			pInput->state = INPUT_VIBRATE;
			pInput->inputTime=0;
		}else if(pInput->state==INPUT_VIBRATE){
			pInput->inputTime+=HEART_INTERVAL;
			if(pInput->inputTime>=pInput->inputOffTime){
				pInput->state=INPUT_DOWN;
				return 1;
			}
		}else if(pInput->state==INPUT_DOWN){
			pInput->inputTime+=HEART_INTERVAL;
			if(pInput->inputTime>=pInput->inputLongTime){
				pInput->state=INPUT_LONG_DOWN;
				return 2;
			}
		}else if(pInput->state==INPUT_VIBRATE2){
			pInput->inputTime=0;
			pInput->state=INPUT_DOWN;
		}else if(pInput->state==INPUT_VIBRATE3){
			pInput->inputTime=0;
			pInput->state=INPUT_LONG_DOWN;
		}
	}else{
		if(pInput->state==INPUT_IDLE){
		}else if(pInput->state==INPUT_VIBRATE){
			pInput->state=INPUT_IDLE;
			pInput->inputTime=0;
		}else if(pInput->state==INPUT_DOWN){
			pInput->state=INPUT_VIBRATE2;
			pInput->inputTime=0;
		}else if(pInput->state==INPUT_VIBRATE2){
			pInput->inputTime+=HEART_INTERVAL;
			if(pInput->inputTime>=pInput->inputOffTime){
				pInput->state=INPUT_IDLE;
				pInput->inputTime=0;
				return 0;
			}
		}else if(pInput->state==INPUT_LONG_DOWN){
			pInput->state=INPUT_VIBRATE3;
			pInput->inputTime=0;
		}else if(pInput->state==INPUT_VIBRATE3){
			pInput->inputTime+=HEART_INTERVAL;
			if(pInput->inputTime>=pInput->inputOffTime){
				pInput->state=INPUT_IDLE;
				pInput->inputTime=0;
				return 0;
			}
		}
	}
	return -1;
}

const int INPUT_RANGE[2]={6,1000};
const char KEY_CODE[4]=	{0,1,2,3};//SIG_BUF_IN17-24
Input_sut Inputs[4];


void Input_ModuleInit(void)
{
	u16 i=0;
	input_Init();
	
	for(i=0;i<countof(KEY_CODE);i++){
		Inputs[i].inputOffTime=INPUT_RANGE[0];
		Inputs[i].inputLongTime=INPUT_RANGE[1];
		Inputs[i].inputTime=0;
		Inputs[i].mode=OPEN;
		Inputs[i].state=INPUT_IDLE;
		Inputs[i].keyCode=KEY_CODE[i];
	}
}

void Input_ModuleProcess0(unsigned short int period)
{
	u16 i=0;
	u32 input = input_Data();
	
	for(i=0;i<countof(KEY_CODE);i++){//È¥¶·
		u32 in;
		u8 r;
		in = Tool_GetBitFor(input,i);
		r = Input_Handle(Inputs+i/*HC245_NUM*8-1-i*/,in);
		if((r==0||r==1) && Inputs[i/*HC245_NUM*8-1-i*/].keyCode!=(char)-1){
			//keyIsChange=1;
			if(Inputs[i].keyCode==0 && r==1){//led¿ª¹Ø
				LED_ModuleOnOff();
			}
		}
	}
}
