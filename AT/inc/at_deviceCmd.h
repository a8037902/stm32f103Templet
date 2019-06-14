#ifndef __AT_DEVICECMD_H
#define __AT_DEVICECMD_H
#include "sys.h"
//#include ""

void at_testCmdId(uint8_t id);
void at_queryCmdId(uint8_t id);
void at_setupCmdId(uint8_t id, char *pPara);
//void at_exeCmdId(uint8_t id);

void at_testCmdPlu(uint8_t id);
void at_queryCmdPlu(uint8_t id);
void at_setupCmdPlu(uint8_t id, char *pPara);
//void at_exeCmdPara(uint8_t id);

void at_queryCmdBill(uint8_t id);

void at_testCmdCoin(uint8_t id);
void at_setupCmdCoin(uint8_t id, char *pPara);
//void at_exeCmdCoin(uint8_t id);

void at_queryCmdCsq(uint8_t id);

void at_testCmdDebug(uint8_t id);
void at_queryCmdDebug(uint8_t id);
void at_setupCmdDebug(uint8_t id, char *pPara);

void at_exeCmdCip(uint8_t id);

void at_testCmdGprs(uint8_t id);
void at_setupCmdGprs(uint8_t id, char *pPara);

void at_testCmdSet(uint8_t id);
void at_queryCmdSet(uint8_t id);
void at_setupCmdSet(uint8_t id, char *pPara);

void at_exeCmdGret(uint8_t id);

void at_exeCmdGdebug(uint8_t id);

void at_testCmdApn(uint8_t id);
void at_queryCmdApn(uint8_t id);
void at_setupCmdApn(uint8_t id, char *pPara);
#endif
