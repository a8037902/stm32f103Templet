/*
 * config.h
 *
 *  Created on: 2015?12?18?
 *      Author: Administrator
 */

#ifndef APP_INCLUDE_CONFIG_H_
#define APP_INCLUDE_CONFIG_H_

#define SYSTEM_SUPPORT_UCOS		1		//定义系统文件夹是否支持UCOS
#define SYSTEM_SUPPORT_UCGUI	0		//定义系统文件夹是否支持UCOS
#define USART_SUPPORT_MUX_TASK	0		//串口是否支持多线程


#if SYSTEM_SUPPORT_UCOS==1
#define TASK_NUM						3
#define MESSAGE_NUM					30
#define MEM_SIZE						32
#define MEM_NUM							30
#define MEM_BIG_SIZE				256
#define MEM_BIG_NUM					16
#endif

#define DEBUG_USART					1

#define MY_DEBUG						0



#if defined STM32F10X_LD
#define ENCRYPT_FLASH_ADDRESS 0x8008000
#elif defined STM32F10X_MD
#define ENCRYPT_FLASH_ADDRESS 0x8008000
#elif defined STM32F10X_HD
#define ENCRYPT_FLASH_ADDRESS 0x803F000
#elif defined STM32F10X_CL
#define ENCRYPT_FLASH_ADDRESS 0x8008000
#endif

#if defined STM32F10X_LD
#define PAGE_SIZE		1024
#elif defined STM32F10X_MD
#define PAGE_SIZE		1024
#elif defined STM32F10X_HD
#define PAGE_SIZE		2048
#elif defined STM32F10X_CL
#define PAGE_SIZE		2048
#endif

#define HARDWARE_VERSOIN_NUM	(0x01)
#define INDUSTR_VERSOIN_NUM		(0x01)
#define MAIN_SOFTWARE_VERSOIN_NUM	(0x01)
#define SOFTWARE_VERSOIN_NUM		(0x03)

#define DEVICE_VERSOIN ((HARDWARE_VERSOIN_NUM<<24)|(INDUSTR_VERSOIN_NUM<<16)|(MAIN_SOFTWARE_VERSOIN_NUM<<8)|SOFTWARE_VERSOIN_NUM)

#define DATE_YEAR		2018
#define DATE_MONTH	12
#define DATE_DAY		20


//user config
#endif /* APP_INCLUDE_CONFIG_H_ */
