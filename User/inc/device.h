#ifndef __DEVICE_H
#define __DEVICE_H	
#include "sys.h"

typedef enum{
	DEVICE_ALL,
	DEVICE_ID,
	DEVICE_PASSWORD,
	DEVICE_SERVER,
	DEVICE_IP,
	DEVICE_PORT,
	DEVICE_RES,
	DEVICE_SET,
	DEVICE_P_W0,
	DEVICE_P_W1,
	DEVICE_P_S0,
	DEVICE_P_S1,
	DEVICE_P_IN_W_MIN0,
	DEVICE_P_IN_W_MAX0,
	DEVICE_P_IN_W_MIN1,
	DEVICE_P_IN_W_MAX1,
	DEVICE_P_IN_S0,
	DEVICE_P_IN_S1,
} DevicePara_TypeDef;

typedef struct _DevicePara {
	u16 res;
	u16 set;
	u16 pluseW0;
	u16 pluseW1;
	u16 pluseS0;
	u16 pluseS1;
	u16 pluseInWMin0;
	u16 pluseInWMax0;
	u16 pluseInWMin1;
	u16 pluseInWMax1;
	u16 pluseInS0;
	u16 pluseInS1;
} DevicePara_sut;

typedef struct _device {
	char id[10];
	char password[10];
	//uint8 s;
	u8 serverAll;
	u8 serverOff;
	u32 ip[5];
	u16 port[5];
	DevicePara_sut para;
//	u16 res;
//	u16 set;
//	u16 pluseW0;
//	u16 pluseW1;
//	u16 pluseS0;
//	u16 pluseS1;
//	u16 pluseInWMin0;
//	u16 pluseInWMax0;
//	u16 pluseInWMin1;
//	u16 pluseInWMax1;
//	u16 pluseInS0;
//	u16 pluseInS1;
//uint8 sw;
//uint8 save;

} Device_sut;

void device_Init(void);

u32 device_GetPara(DevicePara_TypeDef para,u8 off);
void device_SetPara(DevicePara_TypeDef para,u8 off, u32 data);

#endif
