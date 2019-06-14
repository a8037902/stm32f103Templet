#include "iwdg_module.h"
#include "sys.h"

void IWDG_ModuleInit()
{
	//(64/40)*625 = 1s
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(625);
	IWDG_Enable();
}

void IWDG_ModuleProcess0(unsigned short int period)
{
	IWDG_ReloadCounter();
}
