#include "userIO.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

//usb相关数据
extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint32_t packet_receive=1;
uint32_t packet_sent=1;
uint8_t Send_Buffer[64];

#define COM_USER	0
#define USB_MAX		63

void UserIO_Init()
{
	Usart_Init(COM_USER,115200);
	
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
}

u16 UserIO_Send(u8 *buf, u16 size)
{
	u16 l,k,r=0;
	if(bDeviceState == CONFIGURED){//usb
		l = size;
		while(l>USB_MAX){//大于64分批发送
			for(k=0;k<10;k++){
				if (packet_sent == 1){
					CDC_Send_DATA (buf+r,USB_MAX);
					l-=USB_MAX;
					r+=USB_MAX;
					break;
				}
				OSTimeDly(1);
			}
		}
		for(k=0;k<10;k++){
			if (packet_sent == 1){
				CDC_Send_DATA (buf+r,l);
				r+=l;
				break;
			}
			OSTimeDly(1);
		}
		for(k=0;k<10;k++){
			if (packet_sent == 1){
				CDC_Send_DATA (buf,0);
				break;
			}
			OSTimeDly(1);
		}
	}else{//usart0
		r = Usart_SetData(COM_USER,buf,size);
	}
	return r;
}

u16 UserIO_Receive(u8 *buf, u16 size)
{
	u16 l=0;
	if(bDeviceState == CONFIGURED){//usb
		CDC_Receive_DATA();
		/*Check to see if we have data yet */
		if (Receive_length  != 0){
			l = Receive_length;
			Receive_length=0;
			memcpy(buf,(u8*)Receive_Buffer,l);
		}
	}else{//usart0
		l = Usart_GetData(COM_USER,buf,size);
		if(l>0){
			OSTimeDly(1);
			l += Usart_GetData(COM_USER,buf+l,size-l);
		}
	}
	return l;
}
