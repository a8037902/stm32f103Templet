#include "tool.h"

u32 get_bitFor(u32 data, u8 index) {
	return (data >> index) & 1;
}

u32 set_bitFor(u32 data, u8 index, BOOL d) {
	if (d) {
		data |= 1 << index;
	} else {
		data &= ~(1 << index);
	}
	return data;
}

/*
// C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
//	[IN] pbSrc - 字符串
//	[IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value: 
// remarks : 将字符串转化为16进制数
*/
void StrToHex(u8 *pbDest, u8 *pbSrc, int nLen)
{
	char h1,h2;
	u8 s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数
// return value: 
// remarks : 将16进制数转化为字符串
*/
void HexToStr(u8 *pbDest, u8 *pbSrc, int nLen)
{
	char	ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	pbDest[nLen*2] = '\0';
}


void bubble_Sort(u16 L[],u16 n) { 
	int i,j;  
	u16 temp;
	BOOL ischanged;//设计跳出条件 
	for(j=n;j<0;j--){  
		ischanged =FALSE; 
		for(i=0;i<j;i++) {  
			if(L[i]>L[i+1]){//如果发现较重元素就向后移动 
  				temp=L[i];
 				L[i]=L[i+1]; 
				L[i+1]=temp;
 				ischanged =TRUE; 
			}
 		}
		if(!ischanged)//若没有移动则说明序列已经有序，直接跳出  
		break;  
	} 
}


