#ifndef _8086_H_
#define _8086_H_

#include "disassemble.h"
#include "headerPaser.h"

typedef struct cpu{
	unsigned short reg[8];

	unsigned char o;
	unsigned char s;
	unsigned char z;
	unsigned char c;

	unsigned char memory[0xffff];
	unsigned char data[100];
    
    int isINT;

}cpu;

extern void initCpu(cpu* cpu, header header);
extern void initStack(cpu* cpu, header header);
extern void initData(cpu* cpu, header header, const unsigned char* binary);
extern void resetFlags(cpu* cpu);

unsigned short createAdressFromRegAndDISP(const cpu cpu, const char reg, const short disp);

extern int movRegisterMemoryToFromRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int movImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int movImmediateToRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int movMemoryToAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int orImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int orRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int xorRrmAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int rep(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int movs(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cmps(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int scas(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int loadEAtoRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int halt(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cliClearInterrupt(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cld(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int std(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int shl(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int shr(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int sar(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int rcl(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int neg(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int retWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int retWithinSegAddingImmedToSP(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int mulMultiply(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int divDivide(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int intInterrupt(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cbwConvertByteToWord(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cwdConvertWordToDoubleWord(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int pushRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int pushRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int popRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int xchgRegisterMemoryWithRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int xchgExchange(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int callDirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int callIndirectIntersegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int callIndirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int addRegisterMemoryWithRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int addImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int adcRegMemoryWithRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cmpImmediateWithRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cmpRegisterMemoryAndRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int cmpImmediateWithAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int incRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int incRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int decRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int decRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jmpIndirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jmpDirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jmpDirectWithinSegmentShort(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jnbJae(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jnlJge(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jnleJg(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jneJnz(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jlJnge(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jbJnae(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jeJz(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jbeJna(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jleJng(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int jnbeJa(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int loop(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int inFixedPort(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int inVariablePort(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int testRegisterMemoryAndRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int testImmediateDataAndRegistertoEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int testImmediateDataAndAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int andRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int andImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int subRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int subImmediateFromRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int subImmediateFromAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int ssbRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);
extern int ssbImmediateFromRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op);

#endif
