/*
 * File	: at_port.c
 * This file is part of Espressif's AT+ command set program.
 * Copyright (C) 2013 - 2016, Espressif Systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "at.h"

/** @defgroup AT_PORT_Defines
  * @{
  */
#define at_cmdLenMax 128
#define at_dataLenMax 2048
/**
  * @}
  */

/** @defgroup AT_PORT_Extern_Variables
  * @{
  */
extern uint16_t at_sendLen;
extern uint16_t at_tranLen;
//extern UartDevice UartDev;
//extern bool IPMODE;
extern uint8_t ipDataSendFlag;
/**
  * @}
  */

/** @defgroup AT_PORT_Extern_Functions
  * @{
  */
extern void at_ipDataSending(uint8_t *pAtRcvData);
extern void at_ipDataSendNow(void);
/**
  * @}
  */


BOOL specialAtState = TRUE;
at_stateType  at_state;
uint8_t *pDataLine;
BOOL echoFlag = TRUE;

static uint8_t at_cmdLine[at_cmdLenMax];
//uint8_t at_dataLine[at_dataLenMax];/////
//uint8_t *at_dataLine;

void at_printf(const char * s)
{
	Usart_SetData(0,(u8*)s,strlen(s));
}

/** @defgroup AT_PORT_Functions
  * @{
  */
static void at_procTask();

/**
  * @brief  Uart receive task.
  * @param  events: contain the uart receive data
  * @retval None
  */
  

void at_recvTask(u8 *recvBuf,u16 recvLen)
{
	static uint8_t atHead[2];
	static uint8_t *pCmdLine;
	int i=0;
	uint8_t temp;

	//recvLen = Usart_GetData(0,recvBuf,128);
	
	while(i<recvLen){
		if(at_state != at_statIpTraning)
		{
			temp = recvBuf[i];
			if((temp != '\n') && (echoFlag))
			{
				Usart_SetData(0,&recvBuf[i],1);
			}
		}
		i++;
		switch(at_state){
			case at_statIdle: //serch "AT" head
				atHead[0] = atHead[1];
				atHead[1] = temp;
				if((memcmp(atHead, "AT", 2) == 0) || (memcmp(atHead, "at", 2) == 0)){
					at_state = at_statRecving;
					pCmdLine = at_cmdLine;
					atHead[1] = 0x00;
				}else if(temp == '\n') //only get enter
				{
					at_printf("\r\nERROR\r\n");
				}
				break;

			case at_statRecving: //push receive data to cmd line
				//Usart_SetData(0,(u8*)"1",1);
				*pCmdLine = temp;
				if(temp == '\n')
				{
					//system_os_post(at_procTaskPrio, 0, 0);
					//Usart_SetData(0,(u8*)"2",1);
					pCmdLine++;
					*pCmdLine = '\0';
					at_state = at_statProcess;
					if(echoFlag)
					{
						//Usart_SetData(0,(u8*)"3",1);
					  Usart_SetData(0,(u8*)"\r\n",strlen("\r\n")); ///////////
					  at_printf("\r\n");
					}
					
					at_procTask();
				}else if(pCmdLine >= &at_cmdLine[at_cmdLenMax - 1]){
					//Usart_SetData(0,(u8*)"4",1);
					at_state = at_statIdle;
				}
				pCmdLine++;
				break;

			case at_statProcess: //process data
				if(temp == '\n')
				{
					at_printf("\r\nbusy p...\r\n");
					//Usart_SetData(0,(u8*)"\r\nbusy p...\r\n",sizeof("\r\nbusy p...\r\n"));
				}
				break;

		    default:
			    if(temp == '\n')
			    {
			    }
			    break;
		}
	}
}

/**
  * @brief  Task of process command or txdata.
  * @param  events: no used
  * @retval None
  */
static void
at_procTask()
{
	if(at_state == at_statProcess)
	{
		//Usart_SetData(0,(u8*)"5",1);
	    at_cmdProcess(at_cmdLine);
	    if(specialAtState)
	    {
	      at_state = at_statIdle;
	    }
	}
	else if(at_state == at_statIpSended)
	{
	    //at_ipDataSending(at_dataLine);//UartDev.rcv_buff.pRcvMsgBuff);
	    if(specialAtState)
	    {
	      at_state = at_statIdle;
	    }
	}
	else if(at_state == at_statIpTraning)
	{
	    //at_ipDataSendNow();//UartDev.rcv_buff.pRcvMsgBuff);
	}
}

//static void ICACHE_FLASH_ATTR
//at_busyTask(os_event_t *events)
//{
//  switch(events->par)
//  {
//  case 1:
//    uart0_sendStr("\r\nbusy p...\r\n");
//    break;
//
//  case 2:
//    uart0_sendStr("\r\nbusy s...\r\n");
//    break;
//  }
//}

/**
  * @brief  Initializes build two tasks.
  * @param  None
  * @retval None
  */
//void ICACHE_FLASH_ATTR
//at_init(void)
//{
//  system_os_task(at_recvTask, at_recvTaskPrio, at_recvTaskQueue, at_recvTaskQueueLen);
////  system_os_task(at_busyTask, at_busyTaskPrio, at_busyTaskQueue, at_busyTaskQueueLen);
//  system_os_task(at_procTask, at_procTaskPrio, at_procTaskQueue, at_procTaskQueueLen);
//}

/**
  * @}
  */
