#include "module.h"

#include "iwdg_module.h"
#include "usart_module.h"
#include "led_module.h"
#include "usart_loop_module.h"
#include "at_module.h"
#include "input_module.h"
#include "msg_test_module.h"
#include "gui_module.h"

static Module_sut s_modules[]={
	{IWDG_ModuleInit,0,IWDG_ModuleProcess0,0,0, 0,0,0},
	{Usart_ModuleInit,0,0,0,0, 0,0,0},
	{LED_ModuleInit,0,LED_ModuleProcess0,0,0, 0,0,0},
	//{AT_ModuleInit,0,AT_ModuleProcess0,0,0, 0,0,0},
	{UsartLoop_ModuleInit,0,UsartLoop_ModuleProcess0,0,0, UsartLoop_ModuleHandle0,0,0},
	{Input_ModuleInit,0,Input_ModuleProcess0,0,0, 0,0,0},
	{MsgTest_ModuleInit,0,0,MsgTest_ModuleProcess1,0, MsgTest_ModuleHandle0,0,0},
	//{GUI_ModuleInit,GUI_ModuleAfterInit,0,0,GUI_ModuleProcess2, 0,0,0},
};

void Module_Init()
{
	int i=0;
	for(i=0;i<countof(s_modules);i++){
		if(s_modules[i].init){
			s_modules[i].init();
		}
	}
}

void Module_AfterInit()
{
	int i=0;
	for(i=0;i<countof(s_modules);i++){
		if(s_modules[i].afterInit){
			s_modules[i].afterInit();
		}
	}
}

void Module_Process(unsigned short int processNum, unsigned short int period)
{
	int i=0;
	for(i=0;i<countof(s_modules);i++){
		if(s_modules[i].process[processNum]){
			s_modules[i].process[processNum](period);
		}
	}
}

void Module_MsgHandle(unsigned short int processNum, Message_sut * pMessage)
{
	int i=0;
	for(i=0;i<countof(s_modules);i++){
		if(s_modules[i].handle[processNum]){
			if(s_modules[i].handle[processNum](pMessage)){
				return;
			}
		}
	}
}
