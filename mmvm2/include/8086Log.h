#ifndef _8086LOG_H_
#define _8086LOG_H_

#include "8086.h"
#include "instractionPaser.h"

extern void cpuLog(const cpu cpu);
extern void headerOfCpuLog();

extern void instraction1Log(const instraction1 inst);
extern void instraction3Log(const instraction3 inst);
extern void instraction4Log(const instraction4 inst);
extern void instraction5Log(const instraction5 inst, const int count);
extern void instraction6Log(const instraction6 inst, const int count);
extern void instraction7Log(const instraction7 inst, const int count);
extern void instraction9Log(const instraction9 inst, const int count);
extern void instraction11Log(const instraction11 inst);
extern void instraction12Log(const instraction12 inst);
extern void instraction14Log(const instraction14 inst);
extern void instraction16Log(const instraction16 inst);

#endif