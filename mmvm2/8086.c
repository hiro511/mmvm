#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "common.h"
#include "instractionPaser.h"
#include "textPaser.h"
#include "headerPaser.h"
#include "8086Log.h"
#include "8086.h"


void initCpu(cpu* cpu, header header){
	for(int i = 0; i<8; i++){
		cpu->reg[i] =  0x0;
	}

	cpu->reg[4] = 0xffdc;

	cpu->o = 0x0;
	cpu->s = 0x0;
	cpu->z = 0x0;
	cpu->c = 0x0;
    
    for (int i = 0; i < 0xffff; i++) {
        cpu->memory[i] = 0x0;
    }
}

void initStack(cpu* cpu, header header){
	unsigned char stack[36] = {0};
	stack[0] = 2;
	stack[2] = 0xe6;
	stack[3] = 0xff;
	stack[6] = 0xec;
	stack[7] = 0xff;
	stack[8] = -1;
	memcpy(&stack[10], "a.out", 6);
	memcpy(&stack[16], "PATH=:/bin:/usr/bin", 20);

	memcpy(&cpu->memory[0xffdc], stack, 36);
}

void initData(cpu* cpu, header header, const unsigned char* binary){
	memcpy(cpu->data, &binary[header.length + header.text], header.text);

	for(int i=0; i<50; i++){
		printf("%02x ", cpu->data[i]);
		if(i % 16 == 15){
			printf("\n");
		}
	}
}

void resetFlags(cpu* cpu){
	cpu->o = 0x0;
	cpu->s = 0x0;
	cpu->c = 0x0;
	cpu->z = 0x0;
}

void resetFlagsWithoutC(cpu*cpu){
    cpu->o = 0x0;
    cpu->s = 0x0;
    cpu->z = 0x0;
}

int addRegisterMemoryWithRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction1 inst;
	parseInstraction1(&inst, binary);

    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);

        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("add ");
            instraction1Log(inst);
            
            if(op == DISASSEMBLE){
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            unsigned short mem, adr, result = 0;
            
            switch(inst.mod){
                case 0x0 :
                {
                    if (inst.rm == 0x6) {
                        memcpy(&mem, &cpu->memory[inst.disp], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x", inst.disp, mem);
                        }
                        
                        if (inst.d) {
                            result = cpu->reg[inst.reg] += mem;
                        }else{
                            result = mem += cpu->reg[inst.reg];
                            memcpy(&cpu->memory[inst.disp], &mem, 2);
                        }
                        
                    }else{
                        adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        memcpy(&mem, &cpu->memory[inst.rm], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x", adr, mem);
                        }
                        
                        if (inst.d) {
                            result = cpu->reg[inst.reg] += mem;
                        }else{
                            result = mem += cpu->reg[inst.reg];
                            memcpy(&cpu->memory[adr], &mem, 2);
                        }
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                case 0x1 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x", adr, mem);
                    }
                    
                    if (inst.d) {
                        result = cpu->reg[inst.reg] += mem;
                    }else{
                        result = mem += cpu->reg[inst.reg];
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                case 0x2 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    result = cpu->reg[inst.reg] += mem;
                    
                    break;
                }
                case 0x3 :
                {
                    if(inst.d){
                        result = cpu->reg[(int)inst.reg] += cpu->reg[(int)inst.rm];
                    }else{
                        result = cpu->reg[(int)inst.rm] += cpu->reg[(int)inst.reg];
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                    
            }
            cpu->s = result >> 15 ? 1 : 0;
            
            break;
        }
    }
    
	return inst.length;
}

int adcRegMemoryWithRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction1 inst;
    parseInstraction1(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("adc ");
            instraction1Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
            
        }
        case NONE :
        {
            break;
        }
    }
    
    return inst.length;
}


int addImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction4 inst;
	parseInstraction4(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("add ");
            instraction4Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            unsigned short mem, adr, data, result = 0;
            
            switch (inst.mod) {
                case 0x0:
                {
                    if (inst.s == 0 && inst.w == 1) {
                        adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        memcpy(&mem, &cpu->memory[adr], 2);
                        memcpy(&data, inst.data, 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem += data;
                        
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }else{
                        adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        memcpy(&mem, &cpu->memory[adr], 2);
                        data = (char)inst.data[0];
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem += data;
                        
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }
                    
                    break;
                }
                case 0x1 :
                case 0x2 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    memcpy(&data, inst.data, 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    if (inst.s == 0 && inst.w == 1) {
                        result = mem += data;
                    }else{
                        result = mem += inst.data[0];
                    }
                    
                    memcpy(&cpu->memory[adr], &mem, 2);
                    
                    break;
                }
                case 0x3 :
                {
                    if (inst.s == 0 && inst.w == 1) {
                        memcpy(&data, inst.data, 2);
                        
                        result = cpu->reg[inst.rm] += data;
                    }else if(inst.s == 1 && inst.w == 1){
                        result = cpu->reg[inst.rm] += inst.data[0];
                    }else{
                        result = cpu->reg[inst.rm] += inst.data[0];
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    break;
                }
            }
            
            cpu->s = result >> 15 ? 1 : 0;
            break;
        }
    }

	return inst.length;
}

int subRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction1 inst;
    parseInstraction1(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("sub ");
            instraction1Log(inst);
            
            if(op == DISASSEMBLE){
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            unsigned short mem, adr, result = 0;
            
            switch(inst.mod){
                case 0x0 :
                {
                    if (inst.rm == 0x6) {
                        memcpy(&mem, &cpu->memory[inst.disp], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x", inst.disp, mem);
                        }
                        
                        if (inst.d) {
                            cpu->c = cpu->reg[inst.reg] < mem ? 1 : 0;
                            
                            result = cpu->reg[inst.reg] -= mem;
                        }else{
                            cpu->c = mem < cpu->reg[inst.reg] ? 1 : 0;
                            
                            result = mem -= cpu->reg[inst.reg];
                            memcpy(&cpu->memory[inst.disp], &mem, 2);
                        }
                        
                    }else{
                        adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        memcpy(&mem, &cpu->memory[inst.rm], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x", adr, mem);
                        }
                        
                        if (inst.d) {
                            cpu->c = cpu->reg[inst.reg] < mem ? 1 : 0;
                            
                            result = cpu->reg[inst.reg] -= mem;
                        }else{
                            cpu->c = mem < cpu->reg[inst.reg] ? 1 : 0;
                            
                            result = mem -= cpu->reg[inst.reg];
                            memcpy(&cpu->memory[adr], &mem, 2);
                        }
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                case 0x1 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x", adr, mem);
                    }
                    
                    if (inst.d) {
                        cpu->c = cpu->reg[inst.reg] < mem ? 1 : 0;
                        
                        result = cpu->reg[inst.reg] -= mem;
                    }else{
                        cpu->c = mem < cpu->reg[inst.reg] ? 1 : 0;
                        
                        result = mem -= cpu->reg[inst.reg];
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                case 0x2 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    result = cpu->reg[inst.reg] -= mem;
                    
                    cpu->c = cpu->reg[inst.reg] < mem ? 1 : 0;
                    
                    break;
                }
                case 0x3 :
                {
                    if(inst.d){
                        result = cpu->reg[(int)inst.reg] -= cpu->reg[(int)inst.rm];
                        
                        cpu->c = cpu->reg[inst.reg] < cpu->reg[inst.rm] ? 1 : 0;
                    }else{
                        result = cpu->reg[(int)inst.rm] -= cpu->reg[(int)inst.reg];
                        
                        cpu->c = cpu->reg[inst.rm] < cpu->reg[inst.reg] ? 1 : 0;
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                    
            }
            cpu->s = result >> 15 ? 1 : 0;
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int subImmediateFromRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction4 inst;
	parseInstraction4(&inst, binary);
    
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("sub ");
            instraction4Log(inst);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            switch(inst.mod){
                case 0x3 :
                {
                    if (inst.s == 0 && inst.w == 1) {
                        short data;
                        memcpy(&data, inst.data, 2);
                        
                        cpu->reg[(int)inst.rm] -= data;
                        
                        cpu->z = cpu->reg[(int)inst.rm] == 0 ? 1 : 0;
                        cpu->s = (cpu->reg[(int)inst.rm]>>7) > 0 ? 1 : 0;
                        
                    }else if(inst.s == 1 && inst.w == 1){
                        cpu->reg[(int)inst.rm] -= inst.data[0];
                        
                        cpu->z = cpu->reg[(int)inst.rm] == 0 ? 1 : 0;
                        cpu->s = (cpu->reg[(int)inst.rm]>>7) > 0 ? 1 : 0;
                    }
                    break;
                }
            }
            break;
        }
    }

	return inst.length;
}

int subImmediateFromAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction14 inst;
    parseInstraction14(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("sub ");
            instraction14Log(inst);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            if (inst.w) {
                short data;
                memcpy(&data, inst.data, 2);
                cpu->reg[0] -= data;
                if (cpu->reg[0] == 0) {
                    cpu->z = 1;
                }
            }else{
                
            }
            break;
        }
    }
    
    return inst.length;
}

int ssbRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction1 inst;
	parseInstraction1(&inst, binary);

    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("ssb ");
            instraction1Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            break;
        }

    }

	return inst.length;
}

int ssbImmediateFromRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction4 inst;
    parseInstraction4(&inst, binary);
    
    printf("%04x: ", count);
    hexDumpWithWhiteSpace(binary, inst.length);
    printf("ssb ");
    instraction4Log(inst);
    printf("\n");
    
    
    return inst.length;
}

int incRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction6 inst;
    parseInstraction6(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("inc ");
            instraction6Log(inst, count);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                
                break;
            }
        }
        case NONE :
        {
            resetFlagsWithoutC(cpu);
            
            unsigned short result = 0;
            switch (inst.mod) {
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    
                    if (inst.w) {
                        unsigned short mem;
                        memcpy(&mem, &cpu->memory[adr], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem += 1;
                        memcpy(&cpu->memory[adr], &mem, 2);
                        
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, cpu->memory[adr]);
                        }
                        
                        cpu->memory[adr] += 1;
                        result = (char)cpu->memory[adr];
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    cpu->memory[(int)inst.rm] += 1;
                    result = (char)cpu->memory[inst.rm];
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                    
                default:
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            
            cpu->z = result == 0 ? 1 : 0;
            cpu->s = result >> 15 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int incRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction10 inst;
    parseInstraction10(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("inc %s\n", registerLog[1][(int)inst.reg]);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlagsWithoutC(cpu);
            cpu->reg[(int)inst.reg]++;
            break;
        }
    }
    
    return inst.length;
}

int decRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction6 inst;
    parseInstraction6(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("dec ");
            instraction6Log(inst, count);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlagsWithoutC(cpu);
            
            unsigned short result = 0;
            switch (inst.mod) {
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    
                    if (inst.w) {
                        unsigned short mem;
                        memcpy(&mem, &cpu->memory[adr], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem -= 1;
                        memcpy(&cpu->memory[adr], &mem, 2);
                        
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, cpu->memory[adr]);
                        }
                        
                        cpu->memory[adr] -= 1;
                        result = (char)cpu->memory[adr];
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    cpu->memory[(int)inst.rm] -= 1;
                    result = (char)cpu->memory[inst.rm];
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                    
                default:
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            
            cpu->z = result == 0 ? 1 : 0;
            cpu->s = result >> 15 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int decRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction10 inst;
	parseInstraction10(&inst, binary);

    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("dec %s\n", registerLog[1][(int)inst.reg]);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlagsWithoutC(cpu);
            
            cpu->reg[(int)inst.reg]--;
            
            cpu->s = cpu->reg[(int)inst.reg]>>15 == 1 ? 1 : 0;
            cpu->z = cpu->reg[inst.reg] == 0 ? 1 : 0;
            
            break;
        }
    }
    
	return inst.length;
}

int callDirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction9 inst;
	parseInstraction9(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("call ");
            instraction9Log(inst, count);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            short countShort = (short)(count + inst.length);
            inst.length += inst.disp;
            cpu->reg[4] -=2;
            memcpy(&cpu->memory[cpu->reg[4]], &countShort, 2);
            break;
        }
    }

	return inst.length;
}

int callIndirectIntersegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction12 inst;
	parseInstraction12(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("call ");
            instraction12Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            break;
        }
    }

	return inst.length;
}

int callIndirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction12 inst;
	parseInstraction12(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("call ");
            instraction12Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            switch (inst.mod) {
                case 0x3:
                {
                    short next_count = (short)(count + inst.length);
                    inst.length = cpu->reg[inst.rm] - count;
                    
                    cpu->reg[4] -=2;
                    memcpy(&cpu->memory[cpu->reg[4]], &next_count, 2);
                }
            }
            break;
        }
    }

	return inst.length;
}

int halt(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	int length = 1;

    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("hlt\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            
            break;
        }
    }

	return length;
}

int cld(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    int length = 1;
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("cld\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            
            break;
        }
    }
    
    return length;
}

int std(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    int length = 1;
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("std\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            
            break;
        }
    }
    
    return length;
}


int cliClearInterrupt(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    int length = 1;
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("cli\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            cpu->isINT = 0;
            break;
        }
    }
    
    return length;
}

int shl(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction11 inst;
	parseInstraction11(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("shl ");
            instraction11Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            switch (inst.mod) {
                case 0x3:
                {
                    if (inst.v) {
                        char cl = cpu->reg[1] & 0xff;
                        
                        cpu->c = (cpu->reg[inst.rm] >> (16-cl)) & 0x1 ? 1 : 0;
                        cpu->o = (cpu->reg[inst.rm] >> (15-cl)) > 0 ? 1 : 0;
                        
                        cpu->reg[inst.rm] <<= cl;
                        
                    }else{
                        cpu->c = cpu->o = cpu->reg[inst.rm]>>15 ? 1 : 0;
                        
                        cpu->reg[inst.rm] <<= 1;
                    }
                    
                    break;
                }
            }
            
            
            break;
        }
    }

	return inst.length;
}

int shr(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction11 inst;
    parseInstraction11(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("shr ");
            instraction11Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            
            break;
        }
    }
    
    return inst.length;
}

int sar(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction11 inst;
    parseInstraction11(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("sar ");
            instraction11Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            unsigned short result = 0;
            short reg;
            
            switch (inst.mod) {
                case 0x3:
                {
                    reg = cpu->reg[inst.rm];
                    
                    if (inst.w) {
                        char cl = cpu->reg[1] & 0xff;
                        cpu->c = (cpu->reg[inst.rm] >> (cl-1)) & 0x1;
                        
                        result = cpu->reg[inst.rm] = reg >> (cpu->reg[1] & 0xff);
                    }else{
                        cpu->c = reg & 0x1;
                        
                        result = cpu->reg[inst.rm] = reg >> 1;
                    }
                    break;
                }
            }
            
            cpu->z = result == 0 ? 1 : 0;
            cpu->s = result >> 15? 1 : 0;

            break;
        }
    }
    
    return inst.length;
}

int rcl(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction11 inst;
    parseInstraction11(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("rcl ");
            instraction11Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            
            break;
        }
    }
    
    return inst.length;
}

int neg(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction6 inst;
	parseInstraction6(&inst, binary);

    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("neg ");
            instraction6Log(inst, inst.length);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            cpu->c = 1;
            
            switch (inst.mod) {
                case 0x1 :
                {
                    unsigned char adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    
                    cpu->memory[adr] = (~cpu->memory[adr])+1;
                    break;
                }
                case 0x3 :
                {
                    if (inst.w) {
                        cpu->reg[inst.rm] = (~cpu->reg[inst.rm])+1;
                    }else{
                        //cpu->reg[inst.rm] |= 0x80;
                    }
                    break;
                }
            }
            
            break;
        }
    }

	return inst.length;
}

int mulMultiply(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction6 inst;
    parseInstraction6(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("mul ");
            instraction6Log(inst, inst.length);
            printf("\n");
            
            if(op == DISASSEMBLE){
                break;
            }
        }
        case NONE :
        {
            switch (inst.mod) {
                case 0x3:
                {
                    unsigned int result;
                    result = cpu->reg[0] * cpu->reg[(int)inst.rm];
                    cpu->reg[2] = result >> 8;
                    cpu->reg[0] = result & 0xff;
                    break;
                }
            }
            break;
        }
    
    }
    
    return inst.length;
}

int divDivide(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction6 inst;
    parseInstraction6(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("div ");
            instraction6Log(inst, count);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            if (op == MONITOR) {
                printf("\n");
            }
            
            switch (inst.mod) {
                case 0x3:
                {
                    int ans, extra;
                    
                    if (inst.w) {
                        unsigned int num;
                        num = cpu->reg[2];
                        num <<= 16;
                        num |= cpu->reg[0];
                        
                        ans = num / cpu->reg[inst.rm];
                        extra = num % cpu->reg[inst.rm];
                        
                        cpu->reg[0] = ans;
                        cpu->reg[2] = extra;
                        
                    }else{
                        
                        
                        
                    }
                    
                    break;
                }
            }
            break;
        }
    }
    
    return inst.length;
}

int retWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	int length = 1;
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("ret\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            unsigned short target;
            memcpy(&target, &cpu->memory[cpu->reg[4]], 2);

            length = target - count;
            cpu->reg[4] += 2;

            break;
        }
    }

	return length;
}

int retWithinSegAddingImmedToSP(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction9 inst;
    parseInstraction9(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("ret %04x\n", inst.disp);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            unsigned short target;
            memcpy(&target, &cpu->memory[cpu->reg[4]], 2);
            
            inst.length = target - count;
            cpu->reg[4] += 2;
    
            cpu->reg[4] += inst.disp;
            break;
        }
    }
    
    return inst.length;
}

//  src/include/minix/callnr.c
//      it indecates each signal number.
//
//      exit
//
//          src/lib/posix/__exit.c
//          m.m1_i1 = status;
//
//
//          src/lib/i86/rts/_sendrec.s
//          write(fs, WRITE, &m)
//          fs = ax, WRITE = dx = 0x4, m = struct message = bx
//
//      ioctl
//
//          src/lib/posix/_ioctl.c
//          src/include/minix/com.h
//
//          m.TTY_LINE = m.m2_i1 = fd;              5th bytes
//          m.TTY_REQUEST = m.m2_i3 = request;      9th bytes
//          m.ADDRESS = m.m2_p1 = (char *) data;    19th bytes
//
//
//      write
//
//          src/lib/posix/_write.c
//          m.m1_i1 = fd;                 5th bytes
//          m.m1_i2 = nbytes;             7th bytes
//          m.m1_p1 = (char *) buffer;    11th bytes
//
//          include/minix/type.h
//          typedef struct {int m1i1, m1i2, m1i3; char *m1p1, *m1p2, *m1p3;} mess_1;
//          typedef struct {int m2i1, m2i2, m2i3; long m2l1, m2l2; char *m2p1;} mess_2;
//          typedef struct {int m3i1, m3i2; char *m3p1; char m3ca1[M3_STRING];} mess_3;
//          typedef struct {long m4l1, m4l2, m4l3, m4l4, m4l5;} mess_4;
//          typedef struct {char m5c1, m5c2; int m5i1, m5i2; long m5l1, m5l2, m5l3;}mess_5;
//          typedef struct {int m6i1, m6i2, m6i3; long m6l1; sighandler_t m6f1;} mess_6;
//
//          typedef struct {
//              int m_source;			/* who sent the message */
//              int m_type;			/* what kind of message is it */
//              union {
//                  mess_1 m_m1;
//                  mess_2 m_m2;
//                  mess_3 m_m3;
//                  mess_4 m_m4;
//                  mess_5 m_m5;
//                  mess_6 m_m6;
//              } m_u;
//          } message;
//
//
//      brk
//          src/lib/other/_brk.c
//          m.m1_p1 = addr;
//
//          _brksize = m.m2_p1;
//
//
//
//
//
//
//
//

int intInterrupt(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction5 inst;
	parseInstraction5(&inst, binary);

    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("int %x", inst.disp);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            switch (cpu->reg[2]) {
                //exit
                case 0x1 :
                    
                {
                    unsigned short m_p, status;
                    
                    m_p = cpu->reg[3];
                    
                    memcpy(&status, &cpu->memory[(m_p + 4)], 2);
                    
                    if (op == MONITOR) {
                        printf("<exit(%d)>\n", status);
                    }
                    
                    exit(status);
                    break;
                }
                //write
                case 0x4 :
                    
                {
                    unsigned short m_p, fd, buffer, count;
                    
                    m_p = cpu->reg[3];
                    memcpy(&fd, &cpu->memory[m_p + 4], 2);
                    memcpy(&buffer, &cpu->memory[m_p + 10], 2);
                    memcpy(&count, &cpu->memory[m_p + 6], 2);
                    
                    if (op == MONITOR) {
                        printf("<write(%d, 0x%04x, %d)", fd, buffer, count);
                        fflush(stdout);
                    }
                    
                    ssize_t result = write(fd, &cpu->memory[buffer], count);
                    
                    if (op == MONITOR) {
                        printf(" => %zd>\n", result);
                    }
                    
                    memcpy(&cpu->memory[m_p+2], &result, 2);
                    
                    cpu->reg[0] = 0x0;
                    break;
                }
                //brk
                case 0x11 :
                {
                    unsigned short m_p, adr, result = 0;
                    
                    m_p = cpu->reg[3];
                    memcpy(&adr, &cpu->memory[m_p+10], 2);
                    
                    if (op == MONITOR) {
                        printf("<brk(0x%04x) => %d>\n", adr, result);
                    }
                    
                    memcpy(&cpu->memory[m_p + 18], &adr, 2);    //????
                    memcpy(&cpu->memory[m_p + 2], &result, 2);
                    
                    break;
                }
                //ioctl
                case 0x36 :
                {
                    unsigned short m_p, fd, request, data_p;
                    
                    m_p = cpu->reg[3];
                    memcpy(&fd, &cpu->memory[m_p + 4], 2);
                    memcpy(&request, &cpu->memory[m_p + 8], 2);
                    memcpy(&data_p, &cpu->memory[m_p + 18], 2);
                    
                    if (op == MONITOR) {
                        printf("<ioctl(%d, 0x%04x, 0x%04x)>\n", fd, request, data_p);
                    }
                    
//                    short result = (short)ioclt(fd, request, &cpu->memory[data_p]);
                    short result = 0xffea;
                    memcpy(&cpu->memory[m_p+2], &result, 2);
                    
                    cpu->reg[0] = 0x0;
                    
                    break;
                }
                default:
                {
                    printf("undefined : %x\n", inst.disp);
                    break;
                }
            }
            break;
        }
    }

	return inst.length;
}

int cbwConvertByteToWord(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    int length = 1;
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE:
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("cbw\n");
            
            if(op == DISASSEMBLE){
                break;
            }
        }
        case NONE :
        {
            char al = cpu->reg[0] & 0xff;
            cpu->reg[0] = al;
            
            break;
        }
    }
    
    return length;
}

int cwdConvertWordToDoubleWord(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    int length = 1;
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, length);
            printf("cwd\n");
            
            if(op == DISASSEMBLE){
                break;
            }
        }
        case NONE :
        {
            if (cpu->reg[0]>>15) {
                cpu->reg[2] = 0xffff;
            }else{
                cpu->reg[2] = 0x0;
            }
            break;
        }
    }
    
    return length;
}

int pushRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction8 inst;
	parseInstraction8(&inst, binary);
    
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
           	printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("push %s", registerLog[1][(int)inst.reg]);
            printf("\n");
           
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            cpu->reg[4] -= 2;
            memcpy(&cpu->memory[cpu->reg[4]], &cpu->reg[(int)inst.reg], 2);
//            if (op == MONITOR) {
//                printf(" ;[%04x]%02x%02x\n", cpu->reg[4], cpu->memory[cpu->reg[4]+1], cpu->memory[cpu->reg[4]]);
//            }
            break;
        }
    }

	return inst.length;
}

int pushRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction12 inst;
	parseInstraction12(&inst, binary);
    
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("push ");
            instraction12Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            switch(inst.mod){
                case 0x0 :
                {
                    if (inst.rm == 6) {
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", inst.disp, cpu->memory[inst.disp+1], cpu->memory[inst.disp]);
                        }
                        
                        cpu->reg[4] -= 2;
                        memcpy(&cpu->memory[cpu->reg[4]], &cpu->memory[inst.disp], 2);
                    }else{
                        unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                        }
                        
                        cpu->reg[4] -= 2;
                        memcpy(&cpu->memory[cpu->reg[4]], &cpu->memory[adr], 2);
                    }
                    
                    break;
                }
                case 0x1 :
                case 0x2 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
      
                    cpu->reg[4] -=2;
                    if (op == MONITOR) {
                        printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                    }
                    memcpy(&cpu->memory[cpu->reg[4]], &cpu->memory[adr], 2);
                    break;
                }
                case 0x3 :
                {
                    cpu->reg[4] -=2;
                    memcpy(&cpu->memory[cpu->reg[4]], &cpu->reg[inst.rm], 2);
                    if(op == MONITOR){
                        printf("\n");
                    }
                    break;
                }
                    
                default:
                {
                    if (op == MONITOR) {
                        printf("undefined \n");
                    }
                    
                    break;
                }
            }
            
            break;
        }
    }

	return inst.length;
}

int popRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction8 inst;
	parseInstraction8(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("pop %s\n", registerLog[1][(int)inst.reg]);
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            memcpy(&cpu->reg[(int)inst.reg], &cpu->memory[cpu->reg[4]], 2);
            cpu->reg[4] += 2;
            break;
        }
    }

	return inst.length;
}


int xchgRegisterMemoryWithRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction16 inst;
    parseInstraction16(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("xchg ");
            instraction16Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            
            switch (inst.mod) {
                case 0x0:
                {
                    if (inst.rm == 6) {
                        if (inst.w) {
                            unsigned short mem, tmp;
                            
                            memcpy(&mem, &cpu->memory[inst.disp], 2);
                            
                            tmp = cpu->reg[inst.reg];
                            cpu->reg[inst.reg] = mem;
                            mem = tmp;
                            
                            memcpy(&cpu->memory[inst.disp], &cpu->reg[inst.reg], 2);
                        }
                    }
                    break;
                }
            }
    
            break;
        }
    }
    
    return inst.length;
}

int xchgExchange(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction8 inst;
    parseInstraction8(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("xchg %s, %s\n", registerLog[1][inst.reg] , registerLog[1][0]);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            unsigned short tmp;
            
            tmp = cpu->reg[inst.reg];
            cpu->reg[inst.reg] = cpu->reg[0];
            cpu->reg[0] = tmp;
            
            break;
        }
    }
    
    return inst.length;
}

int loadEAtoRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction2 inst;
	parseInstraction2(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
           	printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("lea ");
            printf("%s, ", registerLog[1][(int)inst.reg]);
            if(inst.disp > 0){
                printf("[%s+%x]", memoryLog[(int)inst.rm], inst.disp);
            }else{
                printf("[%s-%x]", memoryLog[(int)inst.rm], ~inst.disp+1);
            }
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            unsigned short adr = '\0';
            switch((int)inst.rm){
                case 0 :
                {
                    adr = cpu->reg[3] + cpu->reg[6] + inst.disp;
                    break;
                }
                case 1 :
                {
                    adr = cpu->reg[3] + cpu->reg[7] + inst.disp;
                    break;
                }
                case 2 :
                {
                    adr = cpu->reg[5] + cpu->reg[6] + inst.disp;
                    break;
                }
                case 3 :
                {
                    adr = cpu->reg[5] + cpu->reg[7] + inst.disp;
                    break;
                }
                case 4 : 
                {
                    adr = cpu->reg[6] + inst.disp;
                    break;
                }
                case 5 :
                {
                    adr = cpu->reg[7] + inst.disp;
                    break;
                }
                case 6 : 
                {
                    adr = cpu->reg[5] + inst.disp;
                    break;
                }
                case 7 : 
                {
                    adr = cpu->reg[3] + inst.disp;
                    break;
                }
            }
            cpu->reg[(int)inst.reg] = adr;
            
            if (op == MONITOR) {
                printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
            }
            break;
        }
    }

	return inst.length;
}

int cmpRegisterMemoryAndRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction1 inst;
    parseInstraction1(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("cmp ");
            instraction1Log(inst);
            
            if(op == DISASSEMBLE){
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            unsigned short mem, adr, result = 0;
            
            switch(inst.mod){
                case 0x0 :
                {
                    if (inst.rm == 0x6) {
                        memcpy(&mem, &cpu->memory[inst.disp], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", inst.disp, mem);
                        }
                        
                        if (inst.d) {
                            result = cpu->reg[inst.reg] - mem;
                            
                            if (cpu->reg[inst.reg] < mem) {
                                cpu->s = cpu->c = 1;
                            }else{
                                cpu->z = result == 0 ? 1 : 0;
                                cpu->s = result >> 15 == 1 ? 1 : 0;
                            }
                        }else{
                            result = mem - cpu->reg[inst.reg];
                            
                            if (mem < cpu->reg[inst.reg]) {
                                cpu->s = cpu->c = 1;
                            }else{
                                cpu->z = result == 0 ? 1 : 0;
                                cpu->s = result >> 15 == 1 ? 1 : 0;
                            }
                        }
                    }else{
                        adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        memcpy(&mem, &cpu->memory[adr], 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        if (inst.d) {
                            result = cpu->reg[inst.reg] - mem;
                            
                            cpu->c = cpu->reg[inst.reg] < mem ? 1 : 0;
                        }else{
                            result = mem - cpu->reg[inst.reg];
                            
                            cpu->c = mem < cpu->reg[inst.reg] ? 1 : 0;
                        }
                        
                        cpu->s = result >> 15 == 1 ? 1 : 0;
                        cpu->z = result == 0 ? 1 : 0;
                        
                    }
                    
                    break;
                }
                case 0x1 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    if (inst.d) {
                        result = cpu->reg[inst.reg] - mem;
                        
                        if (cpu->reg[inst.reg] < mem) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                    }else{
                        result = mem - cpu->reg[inst.reg];
                        
                        if (mem < cpu->reg[inst.reg]) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                    }
                    
                    
                    
                    break;
                }
                case 0x2 :
                {
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    result = cpu->reg[inst.reg] - mem;
                    
                    if (cpu->reg[inst.reg] < mem) {
                        cpu->s = cpu->c = 1;
                    }else{
                        cpu->z = result == 0 ? 1 : 0;
                        cpu->s = result >> 15 == 1 ? 1 : 0;
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    if(inst.d){
                        result = cpu->reg[(int)inst.reg] - cpu->reg[(int)inst.rm];
                        
                        if (cpu->reg[inst.reg] < cpu->reg[inst.rm]) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                    }else{
                        result = cpu->reg[(int)inst.rm] - cpu->reg[(int)inst.reg];
                        
                        if (cpu->reg[inst.rm] < cpu->reg[inst.reg]) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                    
            }
            
        }
    }
    
    return inst.length;
}

int cmpImmediateWithRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction4 inst;
	parseInstraction4(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("cmp ");
            instraction4Log(inst);

            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            switch(inst.mod){
                case 0x0 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                    
                    if (inst.rm == 6) {
                        if (inst.s == 1 && inst.w == 1) {
                            unsigned short mem, data, result;
                            memcpy(&mem, &cpu->memory[inst.disp], 2);
                            data = (char)inst.data[0];
                            
                            result = mem - data;
                            
                            if (op == MONITOR) {
                                printf(" ;[%04x]%04x\n", inst.disp, mem);
                            }
                            
                            if (mem < data) {
                                cpu->c = 1;
                                cpu->s = result >> 15 == 1 ? 1 : 0;
                            }else{
                                cpu->z = result == 0 ? 1 : 0;
                                cpu->s = result >> 15 == 1 ? 1 : 0;
                            }
                        }else if(inst.s == 0 && inst.w == 0){
                            
                            
                            
                            
                        }else{
                            if (cpu->memory[inst.disp] == inst.data[0]) {
                                cpu->z = 1;
                            }
                            if(op == MONITOR){
                                printf(" ;[%04x]%04x\n", inst.disp, cpu->memory[inst.disp]);
                            }
                        }
                    }else if(inst.s == 0 && inst.w == 1){
                        if(!memcmp(inst.data, &cpu->memory[adr], 2)){
                            cpu->z = 1;
                        }
                        
                        if(op == MONITOR){
                            printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr], cpu->memory[adr + 1]);
                        }
                    }else if(inst.s == 1 && inst.w == 1){
                        unsigned short result;
                        result = cpu->memory[adr] - inst.data[0];
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%x\n", adr, cpu->memory[adr]);
                        }
                        
                        if (cpu->memory[adr] < inst.data[0]) {
                            cpu->c = 1;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                        
                    }else if(inst.s == 0 && inst.w == 0){
                        unsigned short result;
                        adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        result = cpu->memory[adr] - inst.data[0];
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                        }
                        
                        cpu->c = cpu->memory[adr] < inst.data[0] ? 1 : 0;
                        cpu->s = result >> 15 ? 1 : 0;
                        cpu->z = result == 0 ? 1 : 0;
                        
                    }else{
                        if (op == MONITOR) {
                            printf("\n");
                        }
                        
                    
                    }
                    break;
                }
                case 0x1 :
                case 0x2 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    
                    if (inst.s == 0 && inst.w == 1) {
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                        }
                        
                        short data, mem;
                        memcpy(&data, inst.data, 2);
                        memcpy(&mem, &cpu->memory[adr], 2);
                        
                        unsigned int result = mem - data;
                        
                        if (mem < data) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                    }else if(inst.s == 1 && inst.w == 1){
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                        }
                        
                        unsigned short data, mem;
                        data = (char)inst.data[0];
                        memcpy(&mem, &cpu->memory[adr], 2);
                        
                        unsigned int result = mem - data;
                        
                        if (mem < data) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%x\n", adr, cpu->memory[adr]);
                        }
                        
                        unsigned int result = cpu->memory[adr] - inst.data[0];
                        
                        if (cpu->memory[adr] < inst.data[0]) {
                            cpu->s = cpu->c = 1;
                        }else{
                            cpu->z = result == 0 ? 1 : 0;
                            cpu->s = result >> 15 == 1 ? 1 : 0;
                        }
                        
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    unsigned short data, result;
                    if(inst.s == 0 && inst.w == 1){
                        memcpy(&data, inst.data, 2);
                        
                        if(cpu->reg[(int)inst.rm] - data < 0){
                            cpu->s = 1;
                            cpu->c = 1;
                        }else if(cpu->reg[(int)inst.rm] - data == 0){
                            cpu->z = 1;
                        }
                        
                    }else if(inst.s == 1 && inst.w == 1){
                        
                        data = (char)inst.data[0];
                        
                        result = cpu->reg[inst.rm] - data;
                        
                        cpu->c = cpu->reg[inst.rm] < data ? 1 : 0;
                        cpu->s = result >> 15 == 1 ? 1 : 0;
                        cpu->z = result == 0 ? 1 : 0;
                    }
                        
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            break;
        }
    }

	return inst.length;
}

int cmpImmediateWithAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction14 inst;
    parseInstraction14(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("cmp ");
            instraction14Log(inst);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            if (inst.w) {
                unsigned short data, result;
                memcpy(&data, inst.data, 2);
                
                result = cpu->reg[0] - data;
                
                if (cpu->reg[0] < data) {
                    cpu->c = 1;
                    cpu->s = result >> 15 == 1 ? 1 : 0;
                }else{
                    cpu->z = result == 0 ? 1 : 0;
                    cpu->s = result >> 15 == 1 ? 1 : 0;
                }
            }else{
                unsigned char result;
                
                result = (cpu->reg[0] & 0xff) - inst.data[0];
                
                if ((cpu->reg[0] & 0xff) < inst.data[0]) {
                    cpu->c = 1;
                    cpu->s = result >> 15 == 1 ? 1 : 0;
                }else{
                    cpu->z = result == 0 ? 1 : 0;
                    cpu->s = result >> 15 == 1 ? 1 : 0;
                }
            }
            break;
        }
    }
    
    
    return inst.length;
}

int jmpDirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction9 inst;
	parseInstraction9(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jmp ");
            instraction9Log(inst, count);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            inst.length += inst.disp;
            break;
        }
    }

	return inst.length;
}

int jmpDirectWithinSegmentShort(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction5 inst;
	parseInstraction5(&inst, binary);
    
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jmp short ");
            instraction5Log(inst, count);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            inst.length += inst.disp;
            break;
        }
    }

	return inst.length;
}

int jmpIndirectWithinSegment(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction12 inst;
    parseInstraction12(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jmp ");
            instraction12Log(inst);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            switch (inst.mod) {
                case 0x3 :
                {
                    inst.length = cpu->reg[inst.rm] - count;
                    break;
                }
            }
            
            break;
        }
    }
    
    return inst.length;
}

int jbJnae(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jb ");
            instraction5Log(inst, count);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->c == 1) {
                inst.length += inst.disp;
            }
            
            break;
        }
    }
    
    return inst.length;
}

int jlJnge(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction5 inst;
	parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jl %04x\n", inst.disp+inst.length+count);
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->s != cpu->o) {
                inst.length += inst.disp;
            }
            
            break;
        }
    }

	return inst.length;
}

int jleJng(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jle %04x\n", inst.disp + inst.length + count);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->z == 1 || cpu->s != cpu->o) {
                inst.length += inst.disp;
            }
            
            break;
        }
    }
    
    return inst.length;
}

int jnlJge(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jnl %04x\n", inst.disp + inst.length + count);
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->s == cpu->o) {
                inst.length += inst.disp;
            }
            break;
        }
    }
    
    return inst.length;
}

int jnleJg(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jnle %04x\n", inst.disp + inst.length + count);
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->z == 0 & cpu->s == cpu->o) {
                inst.length += inst.disp;
            }
            break;
        }
    }
    
    return inst.length;
}

int jeJz(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction5 inst;
	parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("je ");
            instraction5Log(inst, count);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if(cpu->z){
                inst.length += inst.disp;
            }
            break;
        }
    }

	return inst.length;
}

int jnbJae(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction5 inst;
	parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jnb ");
            instraction5Log(inst, count);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if(!cpu->c){
                inst.length += inst.disp;
            }
            break;
        }
    }

	return inst.length;
}

int jneJnz(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction5 inst;
	parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jne ");
            instraction5Log(inst, count);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->z == 0) {
                inst.length += inst.disp;
            }
            break;
        }
    }

	return inst.length;
}

int jbeJna(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jbe ");
            instraction5Log(inst, count);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->c == 1 || cpu->z == 1) {
                inst.length += inst.disp;
            }
            break;
        }
    }
    
    return inst.length;
}

int jnbeJa(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("jnbe ");
            instraction5Log(inst, count);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            if (cpu->c == 0 && cpu->z == 0) {
                inst.length += inst.disp;
            }
            break;
        }
    }
    
    return inst.length;
}

int loop(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction5 inst;
    parseInstraction5(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("loop ");
            instraction5Log(inst, count);
            printf("\n");
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            break;
        }
    }
    
    return inst.length;
}

int inFixedPort(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction13 inst;
	parseInstraction13(&inst, binary);

	printf("%04x: ", count);
	hexDumpWithWhiteSpace(binary, inst.length);
	printf("in ax, %x\n", inst.port);

	return inst.length;
}

int inVariablePort(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction13 inst;
	parseInstraction13(&inst, binary);
	inst.length = 1;

	printf("%04x: ", count);
	hexDumpWithWhiteSpace(binary, inst.length);
	printf("in al, dx\n");

	return inst.length;
}

int testRegisterMemoryAndRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction1 inst;
    parseInstraction1(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
            
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("test ");
            instraction1Log(inst);
            
            if(op == DISASSEMBLE){
                printf("\n");
                
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result = 0;
            
            switch (inst.mod) {
                case 0x0 :
                {
                    if (inst.rm == 0x6) {
                        if (inst.w) {
                            unsigned short mem;
                            memcpy(&mem, &cpu->memory[inst.disp], 2);
                            
                            if (op == MONITOR) {
                                printf(" ;[%04x]%04x\n", inst.disp, mem);
                            }
                            
                            result = cpu->reg[(int)inst.reg] & mem;
                        }else{
                            unsigned char reg = cpu->reg[(int)inst.reg] & 0x00ff;
                            
                            if (op == MONITOR) {
                                printf(" ;[%04x]%02x\n", inst.disp, cpu->memory[inst.disp]);
                            }
                            result = (char)(reg & cpu->memory[inst.disp]);
                        }
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", inst.rm, cpu->memory[(int)inst.rm+1], cpu->memory[(int)inst.rm]);
                        }
                        
                        result = cpu->reg[(int)inst.reg] & cpu->memory[(int)inst.rm];
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
            
                    break;
                }
                case 0x1 :
                case 0x2 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    unsigned short mem;
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    result = cpu->reg[(int)inst.reg] & cpu->memory[adr];
                    
                    break;
                }
                case 0x3 :
                {
                    result = cpu->reg[(int)inst.reg] & cpu->reg[(int)inst.rm];
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    break;
                }
            }
            
            cpu->s = result>>15 == 1 ? 1 : 0;
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int testImmediateDataAndRegistertoEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction7 inst;
	parseInstraction7(&inst, binary);

    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("test ");
            instraction7Log(inst, count);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result = 0;
            
            switch (inst.mod) {
                case 0x0 :
                {
                    unsigned short mem, data;
                    memcpy(&mem, &cpu->memory[inst.disp], 2);
                    memcpy(&data, inst.data, 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", inst.disp, data);
                    }
                    
                    result = data & mem;
                    
                    break;
                }
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    if (inst.w) {
                        unsigned short mem, data;
                        memcpy(&mem, &cpu->memory[adr], 2);
                        memcpy(&data, inst.data, 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem & data;
                        
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                        }
                        
                        result = (short)(cpu->memory[adr] & inst.data[0]);
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    unsigned short data;
                    memcpy(&data, inst.data, 2);
                    
                    if (inst.w) {
                        result = cpu->reg[(int)inst.rm] & data;
                    }else{
                        result = (short)((cpu->reg[(int)inst.rm] & 0x00ff) & inst.data[0]);
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            
            cpu->z = result == 0 ? 1 : 0;
            cpu->s = result >> 15 == 1 ? 1 : 0;
            
//            if(inst.mod==0x3){
//                if(inst.w){
//                    
//                }else{
//                    cpu->z = 1;
//                }
//            }
            
            break;
        }
    }

	return inst.length;
}

int testImmediateDataAndAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction14 inst;
    parseInstraction14(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("test ");
            instraction14Log(inst);
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result = 0;
            
            if (inst.w) {
                unsigned short data;
                memcpy(&data, inst.data, 2);
                
                result = cpu->reg[0] & data;
            }else{
                result = (short)((cpu->reg[0] & 0x00ff) & inst.data[0]);
            }
            
            cpu->z = result == 0 ? 1 : 0;
            cpu->s = result >> 15 == 1 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int andRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction1 inst;
	parseInstraction1(&inst, binary);

    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("and ");
            instraction1Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result;
            
            switch (inst.mod) {
                case 0x0:
                {
                    if (inst.rm == 0x6) {
                        unsigned short mem;
                        memcpy(&mem, &cpu->memory[inst.disp], 2);
                        if (inst.d) {
                            if (op == MONITOR) {
                                printf(" ;[%04x]%04x\n", inst.disp, mem);
                            }
                            
                            result = cpu->reg[inst.reg] &= mem;
                            
                        }else{
                            if (op == MONITOR) {
                                printf(" ;[%04x]%04x\n", inst.disp, mem);
                            }
                            
                            result = mem &= cpu->reg[inst.reg];
                            memcpy(&cpu->memory[inst.disp], &mem, 2);
                            
                        }
                    }else{
                        if (op == MONITOR) {
                            printf("\n");
                        }
                        
                        if (inst.d) {
                            result = cpu->reg[inst.reg] &= cpu->reg[inst.rm];
                            
                        }else{
                            result = cpu->reg[inst.rm] &= cpu->reg[inst.reg];
                            
                        }
                    }
                    
                    break;
                }
                case 0x1 :
                {
                    unsigned short adr, mem;
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    if (inst.d) {
                        result = cpu->reg[inst.reg] &= mem;
                        
                    }else{
                        result = mem &= cpu->reg[inst.reg];
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }
                    break;
                }
                case 0x2 :
                {
                    unsigned short adr, mem;
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    result = cpu->reg[inst.reg] &= mem;
                    
                    break;
                }
                case 0x3 :
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    if (inst.d) {
                        result = cpu->reg[inst.reg] &= cpu->reg[inst.rm];
                    }else{
                        result = cpu->reg[inst.rm] &= cpu->reg[inst.reg];
                    }
                    
                    break;
                }
                default:
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    result = 0;
                    break;
                }
            }
            
            cpu->s = result >> 15 == 1 ? 1 : 0;
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }

    }

	return inst.length;
}

int andImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction7 inst;
    parseInstraction7(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("and ");
            instraction7Log(inst, count);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result = 0;
            
            switch (inst.mod) {
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    
                    if (inst.w) {
                        unsigned short mem, data;
                        memcpy(&mem, &cpu->memory[adr], 2);
                        memcpy(&data, inst.data, 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem &= data;
                        
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                        }
                        result = cpu->memory[adr] &= cpu->data[0];
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    if (inst.w) {
                        unsigned short data;
                        memcpy(&data, inst.data, 2);
                        
                        result = cpu->reg[inst.rm] &= data;
                    }else{
                        result = cpu->reg[inst.rm] &= (unsigned short)inst.data[0];
                    }
                    
                    break;
                }
            }
            
            cpu->s = result >> 15 == 1 ? 1 : 0;
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int orRegMemoryAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction1 inst;
	parseInstraction1(&inst, binary);

    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("or ");
            instraction1Log(inst);
           
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result;
            
            switch (inst.mod) {
                case 0x0:
                {
                    if (inst.rm == 0x6) {
                        unsigned short mem;
                        memcpy(&mem, &cpu->memory[inst.disp], 2);
                        if (inst.d) {
                            if (op == MONITOR) {
                                printf(" ;[%04x]%04x\n", inst.disp, mem);
                            }
                            
                            result = cpu->reg[inst.reg] |= mem;
                            
                        }else{
                            if (op == MONITOR) {
                                printf(" ;[%04x]%04x\n", inst.disp, mem);
                            }
                            
                            result = mem |= cpu->reg[inst.reg];
                            memcpy(&cpu->memory[inst.disp], &mem, 2);
                            
                        }
                    }else{
                        if (inst.d) {
                            result = cpu->reg[inst.reg] |= cpu->reg[inst.rm];
                            
                        }else{
                            result = cpu->reg[inst.rm] |= cpu->reg[inst.reg];
                            
                        }
                    }
                    
                    break;
                }
                case 0x1 :
                {
                    unsigned short adr, mem;
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    if (inst.d) {
                        result = cpu->reg[inst.reg] |= mem;
                        
                    }else{
                        result = mem |= cpu->reg[inst.reg];
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }
                    break;
                }
                case 0x2 :
                {
                    unsigned short adr, mem;
                    adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&mem, &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%04x\n", adr, mem);
                    }
                    
                    result = cpu->reg[inst.reg] |= mem;
                    
                    break;
                }
                case 0x3 :
                {
                    if (inst.d) {
                        result = cpu->reg[inst.reg] |= cpu->reg[inst.rm];
                    }else{
                        result = cpu->reg[inst.rm] |= cpu->reg[inst.reg];
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
                default:
                {
                    result = 0;
                    break;
                }
            }
            
            cpu->s = result >> 15 == 1 ? 1 : 0;
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }
    }
    
	return inst.length;
}

int orImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction7 inst;
    parseInstraction7(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("or ");
            instraction7Log(inst, count);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            unsigned short result = 0;
            
            switch (inst.mod) {
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    
                    if (inst.w) {
                        unsigned short mem, data;
                        memcpy(&mem, &cpu->memory[adr], 2);
                        memcpy(&data, inst.data, 2);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%04x\n", adr, mem);
                        }
                        
                        result = mem |= data;
                        
                        memcpy(&cpu->memory[adr], &mem, 2);
                    }else{
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                        }
                        result = cpu->memory[adr] |= cpu->data[0];
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    if (inst.w) {
                        unsigned short data;
                        memcpy(&data, inst.data, 2);
                        
                        result = cpu->reg[inst.rm] |= data;
                    }else{
                        result = cpu->reg[inst.rm] |= (unsigned short)inst.data[0];
                    }
                    
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            
            cpu->s = result >> 15 == 1 ? 1 : 0;
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }
    }
    
    return inst.length;
}

int xorRrmAndRegisterToEither(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction1 inst;
	parseInstraction1(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary,inst.length);
            printf("xor ");
            instraction1Log(inst);
            printf("\n");
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            resetFlags(cpu);
            
            unsigned short result = 0;
            switch(inst.mod){
                case 0x3 :
                {
                    if(inst.d){
                        if (inst.w) {
                            result = cpu->reg[(int)inst.reg] ^= cpu->reg[(int)inst.rm];
                        }else{
                            unsigned char reg_tmp1, reg_tmp2;
                            
                            if (inst.reg < 4) {
                                reg_tmp1 = cpu->reg[inst.reg] & 0xff;
                                reg_tmp2 = cpu->reg[inst.rm] & 0xff;
                                
                                result = reg_tmp1 ^= reg_tmp2;
                                cpu->reg[inst.reg] &= 0xff00;
                                cpu->reg[inst.reg] |= reg_tmp1;
                            }else{
                                inst.reg %= 4;
                                inst.rm %= 4;
                                
                                reg_tmp1 = cpu->reg[inst.reg] >> 8;
                                reg_tmp2 = cpu->reg[inst.rm] >> 8;
                                
                                result = reg_tmp1 ^= reg_tmp2;
                                cpu->reg[inst.reg] &= 0xff;
                                unsigned short reg_tmp3 = reg_tmp1;
                                reg_tmp3 <<= 8;
                                cpu->reg[inst.reg] |= reg_tmp3;
                            }
                            
                        }
                        
                    }else{
                        if (inst.w) {
                            result = cpu->reg[(int)inst.rm] ^= cpu->reg[(int)inst.reg];
                        }else{
                            unsigned char reg_tmp1, reg_tmp2;
                            
                            if (inst.reg < 4) {
                                reg_tmp1 = cpu->reg[inst.rm] & 0xff;
                                reg_tmp2 = cpu->reg[inst.reg] & 0xff;
                                
                                result = reg_tmp1 ^= reg_tmp2;
                                cpu->reg[inst.rm] &= 0xff00;
                                cpu->reg[inst.rm] |= reg_tmp1;
                            }else{
                                inst.reg %=4;
                                inst.rm %= 4;
                                
                                reg_tmp1 = cpu->reg[inst.rm] >> 8;
                                reg_tmp2 = cpu->reg[inst.reg] >> 8;
                                
                                result = reg_tmp1 ^= reg_tmp2;
                                cpu->reg[inst.rm] &= 0xff;
                                unsigned short reg_tmp3 = reg_tmp1;
                                reg_tmp3 <<= 8;
                                cpu->reg[inst.rm] |= reg_tmp3;
                                
                            }
                            
                        }
                    }
                    
                    break;
                }
            }
            
            cpu->z = result == 0 ? 1 : 0;
            
            break;
        }
    }

	return inst.length;
}

int rep(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction15 inst;
    parseInstraction15(&inst, binary);
    
    inst.length = 2;
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("rep ");
            
            switch (binary[1]) {
                case 0xA4 :
                {
                    printf("movsb\n");
                    break;
                }
                case 0xA5 :
                {
                    printf("movsw\n");
                    break;
                }
                case 0xA6 :
                {
                    printf("cmpw\n");
                    break;
                }
                case 0xA7 :
                {
                    printf("cmpb\n");
                    break;
                }
                case 0xAE :
                {
                    printf("scaw\n");
                }
                case 0xAF :
                {
                    printf("scab\n");
                    break;
                }
            }
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            break;
        }
    }
    
    return inst.length;
}

int movs(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction15 inst;
    parseInstraction15(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            if (inst.w) {
                printf("movsw\n");
            }else{
                printf("movsb\n");
            }
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            break;
        }
    }
    
    return inst.length;
}

int cmps(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction15 inst;
    parseInstraction15(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            if (inst.w) {
                printf("cmpw\n");
            }else{
                printf("cmpb\n");
            }
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            break;
        }
    }
    
    return inst.length;
}

int scas(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction15 inst;
    parseInstraction15(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            if (inst.w) {
                printf("scaw\n");
            }else{
                printf("scab\n");
            }
            
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            break;
        }
    }
    
    return inst.length;
}

int movMemoryToAccumulator(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction14 inst;
    parseInstraction14(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE:
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("mov ");
            instraction14Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE:
        {
            if (inst.w) {
                memcpy(&cpu->reg[AX], inst.data, 2);
            } else {
                memcpy(&cpu->reg[AL], inst.data, 1);
            }
            break;
        }
    }
    
    if (!inst.w) {
        inst.length -= 1;
    }
    
    return inst.length;
}

int movRegisterMemoryToFromRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction1 inst;
	parseInstraction1(&inst, binary);
    
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary,inst.length);
            printf("mov ");
            instraction1Log(inst);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            switch(inst.mod){
                case 0x0 :
                {
                    if(inst.rm==0x6){
                        if(inst.d){
                            if (inst.w) {
                                if(op == MONITOR){
                                    printf(" ;[%04x]%02x%02x\n", inst.disp, cpu->memory[inst.disp+1], cpu->memory[inst.disp]);
                                }
                                
                                memcpy(&cpu->reg[(int)inst.reg], &cpu->memory[inst.disp], 2);
                            }else{
                                if (op == MONITOR) {
                                    printf(" ;[%04x]%x\n", inst.disp, cpu->memory[inst.disp]);
                                }
                                
                                cpu->reg[inst.reg] = (cpu->reg[inst.reg] & 0xff00) & cpu->memory[inst.disp];
                            }
                            
                        }else{
                            if (inst.w) {
                                if (op == MONITOR) {
                                    printf(" ;[%04x]%02x%02x\n", inst.disp, cpu->memory[inst.disp+1], cpu->memory[inst.disp]);
                                }
                                
                                memcpy(&cpu->memory[inst.disp], &cpu->reg[(int)inst.reg], 2);
                            }else{
                                if (op == MONITOR) {
                                    printf(" ;[%04x]%x\n", inst.disp, cpu->memory[inst.disp]);
                                }
                                
                                cpu->memory[inst.disp] = (cpu->reg[inst.reg] & 0xff);
                            }
                            
                        }
                        
                    }else{
                        if(inst.d){
                            unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                            
                            if (inst.w) {
                                if (op == MONITOR) {
                                    printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                                }
                                
                                memcpy(&cpu->reg[(int)inst.reg], &cpu->memory[adr], 2);
                            }else{
                                if (op == MONITOR) {
                                    printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                                }
                                
                                cpu->reg[(int)inst.reg] &= 0xff00;
                                cpu->reg[(int)inst.reg] |= cpu->memory[adr];
                            }
                            
                        }else{
                            unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                            
                            if (inst.w) {
                                if(op == MONITOR){
                                    printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                                }
                                
                                memcpy(&cpu->memory[adr], &cpu->reg[(int)inst.reg], 2);
                            }else{
                                if (op == MONITOR) {
                                    printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                                }
                                
                                cpu->memory[adr] = (cpu->reg[inst.reg] & 0xff);
                            }
                        }
                    }
                    break;
                }
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    if(inst.d){
                        if (inst.w) {
                            if(op == MONITOR){
                                printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr + 1], cpu->memory[adr]);
                            }
                            
                            memcpy(&cpu->reg[(int)inst.reg], &cpu->memory[adr], 2);
                        }else{
                            if (op == MONITOR) {
                                printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                            }
                            
                            cpu->reg[inst.reg] &= 0xff00;
                            cpu->reg[inst.reg] |= cpu->memory[adr];
                        }
                        
                    }else{
                        if (inst.w) {
                            if(op == MONITOR){
                                printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr + 1], cpu->memory[adr]);
                            }
                            
                            memcpy(&cpu->memory[adr], &cpu->reg[(int)inst.reg], 2);
                        }else{
                            if (op == MONITOR) {
                                printf(" ;[%04x]%02x\n", adr, cpu->memory[adr]);
                            }
                            
                            //cpu->memory[adr] = cpu->reg[inst.reg];
                        }
                        
                    }
                    break;
                }
                case 0x2 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    memcpy(&cpu->reg[inst.reg], &cpu->memory[adr], 2);
                    
                    if (op == MONITOR) {
                        printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                    }
                    
                    break;
                }
                case 0x3 :
                {
                    if(inst.d){
                        cpu->reg[(int)inst.reg] = cpu->reg[(int)inst.rm];
                        
                        if (op == MONITOR) {
                            printf("\n");
                        }
                    }else{
                        cpu->reg[(int)inst.rm] = cpu->reg[(int)inst.reg];
                        
                        if(op == MONITOR){
                            printf("\n");
                        }
                    }
                    break;
                }
                default:
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            break;
        }
    }

	return inst.length;
}

int movImmediateToRegisterMemory(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
    instraction7 inst;
    parseInstraction7(&inst, binary);
    
    switch (op) {
        case MONITOR:
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
            printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("mov ");
            instraction7Log(inst, inst.length);
            
            if (op == DISASSEMBLE) {
                printf("\n");
                break;
            }
        }
        case NONE :
        {
            switch (inst.mod) {
                case 0x0 :
                {
                    
                    
                    if (inst.rm == 6) {
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", inst.disp, cpu->memory[inst.disp+1], cpu->memory[inst.disp]);
                        }
                        memcpy(&cpu->memory[inst.disp], inst.data, 2);
                    }else{
                        unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, 0);
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                        }
                        
                        memcpy(&cpu->memory[adr], inst.data, 2);
                    }
                    break;
                }
                case 0x1 :
                {
                    unsigned short adr = createAdressFromRegAndDISP(*cpu, inst.rm, inst.disp);
                    if (inst.w) {
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%02x%02x\n", adr, cpu->memory[adr+1], cpu->memory[adr]);
                        }
                        
                        memcpy(&cpu->memory[adr], inst.data, 2);
                        
                    }else{
                        
                        if (op == MONITOR) {
                            printf(" ;[%04x]%x\n", adr, cpu->memory[adr]);
                        }
                        
                        cpu->memory[adr] = inst.data[0];
                    }
           
                    break;
                }
                default:
                {
                    if (op == MONITOR) {
                        printf("\n");
                    }
                    
                    break;
                }
            }
            break;
        }
    }
    
    return inst.length;
}

int movImmediateToRegister(const unsigned char* binary, const int count, cpu* cpu, const enum Option op){
	instraction3 inst;
	parseInstraction3(&inst, binary);
    
    switch (op) {
        case MONITOR :
        {
            cpuLog(*cpu);
        }
        case DISASSEMBLE :
        {
           	printf("%04x: ", count);
            hexDumpWithWhiteSpace(binary, inst.length);
            printf("mov ");
            instraction3Log(inst);
            if (op == DISASSEMBLE) {
                break;
            }
        }
        case NONE :
        {
            memcpy(&cpu->reg[(int)inst.reg], inst.data, 2);
            break;
        }
    }

	return inst.length;
}

unsigned short createAdressFromRegAndDISP(const cpu cpu, const char reg, const short disp){
    unsigned short adr = '\0';
    switch((int)reg){
        case 0 :
        {
            adr = cpu.reg[3] + cpu.reg[6] + disp;
            break;
        }
        case 1 :
        {
            adr = cpu.reg[3] + cpu.reg[7] + disp;
            break;
        }
        case 2 :
        {
            adr = cpu.reg[5] + cpu.reg[6] + disp;
            break;
        }
        case 3 :
        {
            adr = cpu.reg[5] + cpu.reg[7] + disp;
            break;
        }
        case 4 :
        {
            adr = cpu.reg[6] + disp;
            break;
        }
        case 5 :
        {
            adr = cpu.reg[7] + disp;
            break;
        }
        case 6 :
        {
            adr = cpu.reg[5] + disp;
            break;
        }
        case 7 :
        {
            adr = cpu.reg[3] + disp;
            break;
        }
    }
    
    return adr;
}



