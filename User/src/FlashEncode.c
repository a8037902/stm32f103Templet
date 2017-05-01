#include "DES.h"
#include "sys.h"
#include "config.h"
const char DES_KEY_TO_STM[8] = { 0x86, 0xe8, 0xaa, 0xe2, 0xae, 0xe7,0x87, 0xa3 };

void Flash_Encode (){
	u16 id[4];
	 u32 temp0,temp1;
	temp0 = *(__IO u32*)(0x1FFFF7E8);    //产品唯一身份标识寄存器（96位）
    temp1 = *(__IO u32*)(0x1FFFF7EC);
	memcpy(id,&temp0,4);
	memcpy(id+2,&temp1,4);
	des((u8*)id,(u8*)DES_KEY_TO_STM,0,(u8*)id);
	FLASH_ErasePage(ENCRYPT_FLASH_ADDRESS+0x800);
	FLASH_ProgramHalfWord(ENCRYPT_FLASH_ADDRESS+0x800,id[0]);
	FLASH_ProgramHalfWord(ENCRYPT_FLASH_ADDRESS+0x802,id[1]);
	FLASH_ProgramHalfWord(ENCRYPT_FLASH_ADDRESS+0x804,id[2]);
	FLASH_ProgramHalfWord(ENCRYPT_FLASH_ADDRESS+0x806,id[3]);
}
