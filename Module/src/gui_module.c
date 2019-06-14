#include "gui_module.h"
#include "touch.h"
#include "GUI.H"
#include "ILI93xx.h"

static unsigned short int s_period=0;

void GUI_ModuleInit(void)
{
	//TP_Init();
	GUI_Init();
	LCD_Init_ILI93();
}

void GUI_ModuleAfterInit(void)
{
	GUI_SetBkColor(GUI_RED);
	GUI_SetColor(GUI_BLUE);
	GUI_Clear();
	//LCD_Init_ILI93();
	//LCD_DrawRectangle(19,114,220,315);
}

void GUI_ModuleProcess2(unsigned short int period)
{
	GUI_Exec();
}

