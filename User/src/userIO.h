#ifndef __USERIO_H
#define __USERIO_H
#include "sys.h"

void UserIO_Init(void);
u16 UserIO_Send(u8 *buf, u16 size);
u16 UserIO_Receive(u8 *buf, u16 size);

#endif
