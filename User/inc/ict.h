#ifndef __ICT_H
#define __ICT_H
#include "sys.h"

#define ICT_EEPROM_OFF	80
#define ICT_EEPROM_BLOCK_NUM	8
#define ICT_EEPROM_BLOCK_SIZE	20
#define ICT_BILL_TYPE_NUM	8
#define ICT_USART_BOUND	9600
#define ICT_BILL_TYPE_START	0x40

typedef struct _ict_bill {
	u8 frame; //
	u16 bill[ICT_BILL_TYPE_NUM];
} ICT_Bill_sut;

//ict账单在rom中分多个区块轮流存放,0:count,1:fram,2-17:bill,18-19:crc

extern const u16 ICT_PLUSE_NUM[ICT_BILL_TYPE_NUM];
extern const u16 ICT_SUBMIT_NUM[ICT_BILL_TYPE_NUM];

void ict_usart_Init(u32 bound);
void ict_bill_Read(ICT_Bill_sut *pBill);
void ict_bill_Write(ICT_Bill_sut *pBill);
#endif
