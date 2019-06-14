#include <stdlib.h>

#include "at.h"
#include "at_deviceCmd.h"
#include "at_version.h"

extern at_funcationType at_fun[];
extern u16 DebugLevel;




void at_testCmdDebug(uint8_t id)
{
	char temp[32];
	sprintf(temp, "%s:(0-100)\r\n", at_fun[id].at_cmdName);
	at_printf(temp);
	at_backOk;
}
void at_queryCmdDebug(uint8_t id)
{
	char temp[32];
	sprintf(temp, "%s:%d\r\n", at_fun[id].at_cmdName,DebugLevel);
	at_printf(temp);
	at_backOk;
}

void at_setupCmdDebug(uint8_t id, char *pPara)
{
	int data;
	sscanf(pPara,"=%d",&data);
	if(data<=100){
		DebugLevel=data;
		at_backOk;
	}else{
		at_backError;
	}
}


