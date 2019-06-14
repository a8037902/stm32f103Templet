#include "device.h"
#include "eeprom.h"
#include "config.h"

static const char* ID = "cs01";
static const char* PASSWARD = "1234";

static Device_sut Device;

uint16_t VirtAddVarTab[NumbOfVar] = {0x5500, 0x5501, 0x5502, 0x5503, 0x5504, 0x5505, 0x5506, 0x5507, 0x5508, 0x5509, 0x550a, 0x550b, 0x550c, 0x550d, 0x550e, 0x550f,
									 0x5510, 0x5511, 0x5512, 0x5513, 0x5514, 0x5515, 0x5516, 0x5517, 0x5518, 0x5519, 0x551a, 0x551b, 0x551c, 0x551d, 0x551e, 0x551f,
									 0x5520, 0x5521, 0x5522, 0x5523, 0x5524, 0x5525,};//38

void _para_Init(void)
{
	Device.para.pluseW0 = PLUSE_WIDTH_DEF;
	Device.para.pluseW1 = PLUSE_WIDTH_DEF;
	Device.para.pluseS0 = PLUSE_SPACE_DEF;
	Device.para.pluseS1 = PLUSE_SPACE_DEF;
	Device.para.set = 0;
	Device.para.res = 0;

	Device.serverAll = 0;
	Device.serverOff = 0;

	Device.para.pluseInWMin0 = PLUSE_IN_WIDTH_MIN;
	Device.para.pluseInWMax0 = PLUSE_IN_WIDTH_MAX;
	Device.para.pluseInS0 = PLUSE_IN_SPACE_DEF;
	Device.para.pluseInWMin1 = PLUSE_IN_WIDTH_MIN;
	Device.para.pluseInWMax1 = PLUSE_IN_WIDTH_MAX;
	Device.para.pluseInS1 = PLUSE_IN_SPACE_DEF;

	memset(Device.ip, 0, sizeof(Device.ip));
	memset(Device.port, 0, sizeof(Device.port));
}

void _para_Wirte(DevicePara_TypeDef para)
{
	
}


void device_Init(void)
{
	u16 data=0;
	u8 i=0;
	EE_Init();
	
	_para_Init();
	
	//∂¡»°id
	do{
		if(EE_ReadVariable(VirtAddVarTab[i],&data)){//∂¡»°id ß∞‹
			Debug_Printf("read id fail\r\n");
			memcpy(Device.id,ID,strlen(ID));
			memcpy(Device.password,PASSWARD,strlen(PASSWARD));
			return;
		}
		memcpy(Device.id+i*2,&data,2);
	}while(++i<5);
	
	//∂¡»°√‹¬Î
	do{
		if(EE_ReadVariable(VirtAddVarTab[i],&data)){//∂¡»° ß∞‹
			Debug_Printf("read password fail\r\n");
			memcpy(Device.password,PASSWARD,strlen(PASSWARD));
			i=10;
			break;
		}else{
			memcpy(Device.id+i*2,&data,2);
		}
	}while(++i<10);
	
	do{
		if(EE_ReadVariable(VirtAddVarTab[i],&data)){//∂¡»° ß∞‹
			Debug_Printf("read fail:%d\r\n",i);
		}else{
			memcpy(Device.id+i*2,&data,2);
		}
	}while(++i<38);
}

u32 device_GetPara(DevicePara_TypeDef para,u8 off)
{
	u32 r=0;
	switch(para){
		case DEVICE_ALL:
			r = (u32)&Device;
			break;
		case DEVICE_ID:
			r = (u32)Device.id;
			break;
		case DEVICE_PASSWORD:
			r = (u32)Device.password;
			break;
		case DEVICE_SERVER:
			r = ((u16)Device.serverAll<<8) || (u16)Device.serverOff;
			break;
		case DEVICE_IP:
			r = Device.ip[off];
			break;
		case DEVICE_PORT:
			r = Device.port[off];
			break;
		case DEVICE_RES:
			r = Device.para.res;
			break;
		case DEVICE_SET:
			r = Device.para.set;
			break;
		case DEVICE_P_W0:
			r = Device.para.pluseW0;
			break;
		case DEVICE_P_W1:
			r = Device.para.pluseW1;
			break;
		case DEVICE_P_S0:
			r = Device.para.pluseS0;
			break;
		case DEVICE_P_S1:
			r = Device.para.pluseS1;
			break;
		case DEVICE_P_IN_W_MIN0:
			r = Device.para.pluseInWMin0;
			break;
		case DEVICE_P_IN_W_MAX0:
			r = Device.para.pluseInWMax0;
			break;
		case DEVICE_P_IN_W_MIN1:
			r = Device.para.pluseInWMin1;
			break;
		case DEVICE_P_IN_W_MAX1:
			r = Device.para.pluseInWMax1;
			break;
		case DEVICE_P_IN_S0:
			r = Device.para.pluseInS0;
			break;
		case DEVICE_P_IN_S1:
			r = Device.para.pluseInS1;
			break;
	}
	return r;
}

void device_SetPara(DevicePara_TypeDef para,u8 off, u32 data)
{
	int i=0;
	u16 tmp=0;
	switch(para){
		case DEVICE_ALL:
			memcpy(&Device,(u8*)data,sizeof(Device_sut));
			do{
				tmp = (u16)*(Device.id+i*2);
				EE_WriteVariable(VirtAddVarTab[i],tmp);
			}while(++i<38);
			break;
		case DEVICE_ID:
			memcpy(Device.id,(u8*)data,8);
			break;
		case DEVICE_PASSWORD:
			memcpy(Device.password,(u8*)data,8);
			break;
		case DEVICE_SERVER:
			Device.serverAll=(data>>8);
			Device.serverOff=(u8)(data);
			break;
		case DEVICE_IP:
			Device.ip[off]=data;
			break;
		case DEVICE_PORT:
			Device.port[off]=(u16)data;
			break;
		case DEVICE_RES:
			Device.para.res=(u16)data;
			break;
		case DEVICE_SET:
			Device.para.set=(u16)data;
			break;
		case DEVICE_P_W0:
			Device.para.pluseW0=(u16)data;
			break;
		case DEVICE_P_W1:
			Device.para.pluseW1=(u16)data;
			break;
		case DEVICE_P_S0:
			Device.para.pluseS0=(u16)data;
			break;
		case DEVICE_P_S1:
			Device.para.pluseS1=(u16)data;
			break;
		case DEVICE_P_IN_W_MIN0:
			Device.para.pluseInWMin0=(u16)data;
			break;
		case DEVICE_P_IN_W_MAX0:
			Device.para.pluseInWMax0=(u16)data;
			break;
		case DEVICE_P_IN_W_MIN1:
			Device.para.pluseInWMin1=(u16)data;
			break;
		case DEVICE_P_IN_W_MAX1:
			Device.para.pluseInWMax1=(u16)data;
			break;
		case DEVICE_P_IN_S0:
			Device.para.pluseInS0=(u16)data;
			break;
		case DEVICE_P_IN_S1:
			Device.para.pluseInS1=(u16)data;
			break;
	}
}

