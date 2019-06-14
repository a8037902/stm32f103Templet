/*
 * File	: at_cmd.h
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
#ifndef __AT_CMD_H
#define __AT_CMD_H

#include "at.h"
//#include "at_wifiCmd.h"
//#include "at_ipCmd.h"
#include "at_baseCmd.h"
#include "at_deviceCmd.h"

#define at_cmdNum   17

at_funcationType at_fun[at_cmdNum]={
	//basecmd
	{NULL, 0, NULL, NULL, NULL, at_exeCmdNull},
	{"E", 1, NULL, NULL, at_setupCmdE, NULL},
	{"+RST", 4, NULL, NULL, NULL, at_exeCmdRst},
	{"+GMR", 4, NULL, NULL, NULL, at_exeCmdGmr},
	{"+GSLP", 5, NULL, NULL, at_setupCmdGslp, NULL},
	{"+IPR", 4, NULL, NULL, at_setupCmdIpr, NULL},
	
	//devicecmd
	{"+DEBUG", 6, at_testCmdDebug, at_queryCmdDebug, at_setupCmdDebug, NULL},
};

#endif
