#ifndef __UPPER_COMPUTER_H
#define __UPPER_COMPUTER_H
#include "sys.h"
#include "types.h"

#define CMD_HEAD							0xFA
#define CMD_MIN_LEN						6
#define CMD_DATA_LEN_INDEX		3

#define CMD_PC_KEY_MSG     		0x0A
#define CMD_PC_BACK_COIN   		0x0C
#define CMD_CTR_MSG        		0x0D
#define CMD_SOFTDOG_INIT   		0x7A
#define CMD_SOFTDOG_CHECK  		0x7B
#define CMD_IO_WATCH       		0x7C
#define CMD_IO_LED       			0x7D
#define CMD_COIN_OUT_MODE     0x7E
#define CMD_EXFLASH_WRITE     	0x80
#define CMD_EXFLASH_READ      	0x81
#define CMD_GET_SWITCH      	0x90
#define CMD_LINE_DATA	      	0x91
#define CMD_SET_LINE_ID	      	0x92
#define CMD_GET_STMID					0x96
#define CMD_NOTE_IN				0xA0
#define CMD_HEART_BEAT     		0x08 /* ???? */

typedef struct _Upper_Computer {
	u8 cmd;
	u8 *pData;
	u16 len;
} UpperComputer_sut;

Res_TypeDef upper_computer_message_handle(u8* pBuf, u16 len);
void upper_computer_send_message(u8 cmd, u8* pData,u8 len);

#endif
