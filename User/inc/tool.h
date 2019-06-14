#ifndef __TOOL_H
#define __TOOL_H
#include "sys.h"

u32 Tool_GetBitFor(u32 data, u8 index);
u32 Tool_SetBitFor(u32 data, u8 index, BOOL d);
void Tool_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);
void Tool_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);


void Tool_BubbleSort(u16 L[],u16 n);
u16 Tool_GetByteIndex(u8 *pBuf, u16 start, u16 len, u8 byte);
u32 Tool_LoopSub(u32 last, u32 next);

#endif
