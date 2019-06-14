/*
 * File	: at_baseCmd.c
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
#include <stdlib.h>

#include "at.h"
#include "at_baseCmd.h"
#include "at_version.h"
#include "config.h"

/** @defgroup AT_BASECMD_Functions
  * @{
  */

extern BOOL echoFlag;
  
/**
  * @brief  Execution commad of AT.
  * @param  id: commad id number
  * @retval None
  */
void at_exeCmdNull(uint8_t id)
{
  at_backOk;
}

/**
  * @brief  Enable or disable Echo.
  * @param  id: command id number
  * @param  pPara:
  * @retval None
  */
void at_setupCmdE(uint8_t id, char *pPara)
{
//  os_printf("%c\n",*pPara);
  if(*pPara == '0')
  {
    echoFlag = FALSE;
  }
  else if(*pPara == '1')
  {
    echoFlag = TRUE;
  }
  else
  {
    at_backError;
    return;
  }
  at_backOk;
}

/**
  * @brief  Execution commad of restart.
  * @param  id: commad id number
  * @retval None
  */
void at_exeCmdRst(uint8_t id)
{
  at_backOk;
  //system_restart();
}

/**
  * @brief  Execution commad of version.
  * @param  id: commad id number
  * @retval None
  */
void at_exeCmdGmr(uint8_t id)
{
  char temp[64];
  
  sprintf(temp,"FIRMWARE:1.1.1.1\r\n");
  at_printf(temp);


//  sprintf(temp, "GPRS:%s\r\n",APN_NAME);
//  at_printf(temp);
  at_backOk;
}

void at_setupCmdIpr(uint8_t id, char *pPara)
{
//  at_uartType tempUart;

//  pPara++;
//  tempUart.baud = atoi(pPara);
//  if((tempUart.baud>(UART_CLK_FREQ / 16))||(tempUart.baud == 0))
//  {
//    at_backError;
//    return;
//  }

  at_backOk;
}

void at_setupCmdGslp(uint8_t id, char *pPara)
{
	uint32_t n;
	pPara++;
	
	n = atoi(pPara);
	at_backOk;
}
/**
  * @}
  */

