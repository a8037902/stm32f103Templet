#ifndef __FLASH_IO_H
#define __FLASH_IO_H
#include"sys.h"
u16 Flash_Write(u16 * pData, u16 len, u16 off);
u16 Flash_Read(u16 * pData, u16 len, u16 off);

#endif
