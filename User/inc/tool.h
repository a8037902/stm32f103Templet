#ifndef __TOOL_H
#define __TOOL_H
#include "sys.h"

u32 get_bitFor(u32 data, u8 index);
u32 set_bitFor(u32 data, u8 index, BOOL d);
void StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);
void HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);


void bubble_Sort(u16 L[],u16 n);


#endif
