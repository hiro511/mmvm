#include <stdio.h>
#include <string.h>
#include "common.h"
#include "instractionPaser.h"
#include "8086.h"
#include "8086Log.h"
#include "disassemble.h"
#include "headerPaser.h"
#include "textPaser.h"

void parseText(const unsigned char* binary, const header header, const int start, const int end, const enum Option op){
	cpu cpu;
	initCpu(&cpu, header);
	initStack(&cpu, header);
    
//    for (int i = 0xffdc; i < 0xffff; i++) {
//        printf("%04x : %02x\n", i, cpu.memory[i]);
//    }

	memcpy(&cpu.memory, &binary[start + header.text], header.data);
	memcpy(&cpu.memory[header.text], &binary[start], header.text);
    
    if(op == MONITOR){
        headerOfCpuLog();
    }
    
	for(int count = 0; count < end-start;){
		if(end-start-count >= 2){
			count += parseMnemonic(&binary[start+count], count, &cpu, op);
            
		}else{	
			printf("%04x: ", count);
			hexDumpWithWhiteSpace(&binary[start+count], 1);
			printf("undefined\n");
			break;
		}
	}
}

int parseMnemonic(const unsigned char* binary, int count, cpu* cpu, const enum Option op){
	int length = 0;

	switch(*binary){
		case 0x0  :
		case 0x1  :
		case 0x2  :
		case 0x3  : 
		{
			length = addRegisterMemoryWithRegisterToEither(binary, count, cpu, op); 
			break;
		}
		case 0x8 :
		case 0x9 :
		case 0xA :
		case 0xB :
		{
			length = orRegMemoryAndRegisterToEither(binary, count, cpu, op);
			break;
		}
        case 0x10 :
        case 0x11 :
        case 0x12 :
        case 0x13 :
        {
            length = adcRegMemoryWithRegisterToEither(binary, count, cpu, op);
            break;
        }
		case 0x18 :
		case 0x19 :
		case 0x1A :
		case 0x1B : 
		{
			length = ssbRegMemoryAndRegisterToEither(binary, count, cpu, op); 
			break;
		}
		case 0x20 :
		case 0x21 :
		case 0x22 :
		case 0x23 : 
		{
			length = andRegMemoryAndRegisterToEither(binary, count, cpu, op); 
			break;
		}
        case 0x28 :
        case 0x29 :
        case 0x2A :
        case 0x2B :
        {
            length = subRegMemoryAndRegisterToEither(binary, count, cpu, op);
            break;
        }
        case 0x2C :
        case 0x2D :
        {
            length = subImmediateFromAccumulator(binary, count, cpu, op);
            break;
        }
		case 0x30 :
		case 0x31 :
		case 0x32 :
		case 0x33 : 
		{
			length = xorRrmAndRegisterToEither(binary, count, cpu, op);
			break;
        }
        case 0x38 :
        case 0x39 :
        case 0x3A :
        case 0x3B :
        {
            length = cmpRegisterMemoryAndRegister(binary, count, cpu, op);
            break;
        }
        case 0x3C :
        case 0x3D :
        {
            length = cmpImmediateWithAccumulator(binary, count, cpu, op);
            break;
        }
        case 0x40 :
        case 0x41 :
        case 0x42 :
        case 0x43 :
        case 0x44 :
        case 0x45 :
        case 0x46 :
        case 0x47 :
        {
            length = incRegister(binary, count, cpu, op);
            break;
        }
		case 0x48 :
		case 0x49 : 
		case 0x4A : 
		case 0x4B : 
		case 0x4C :
		case 0x4D : 
		case 0x4E :
		case 0x4F : 
		{
			length = decRegister(binary, count, cpu, op);
			break;
		}
		case 0x50 :
		case 0x51 : 
		case 0x52 :
		case 0x53 :
		case 0x54 :
		case 0x55 :
		case 0x56 :
		case 0x57 :
		{
			length = pushRegister(binary, count, cpu, op);
			break;
		}
		case 0x58 :
		case 0x59 :
		case 0x5A :
		case 0x5B :
		case 0x5C :
		case 0x5D :
		case 0x5E :
		case 0x5F : 
		{
			length = popRegister(binary, count, cpu, op);
			break;
		}
        case 0x72 :
        {
            length = jbJnae(binary, count, cpu, op);
            break;
        }
		case 0x73 :
		{
			length = jnbJae(binary, count, cpu, op); 
			break;
		}
		case 0x74 : 
		{
			length = jeJz(binary, count, cpu, op); 
			break;
		}
		case 0x75 : 
		{
			length = jneJnz(binary, count, cpu, op); 
			break;
		}
        case 0x76 :
        {
            length = jbeJna(binary, count, cpu, op);
            break;
        }
        case 0x77 :
        {
            length = jnbeJa(binary, count, cpu, op);
            break;
        }
		case 0x7C :
		{
			length = jlJnge(binary, count, cpu, op); 
			break;
		}
        case 0x7E :
        {
            length = jleJng(binary, count, cpu, op);
            break;
        }
        case 0x7D :
        {
            length = jnlJge(binary, count, cpu, op);
            break;
        }
        case 0x7F :
        {
            length = jnleJg(binary, count, cpu, op);
            break;
        }
		case 0x80 :
		case 0x81 :
		case 0x82 :
		case 0x83 : 
		{
			unsigned char fixed = (binary[1]>>3) & 0x7;
			switch(fixed){
				case 0x0 : {
					length = addImmediateToRegisterMemory(binary, count, cpu, op);
					break;
				}
                case 0x1 :
                {
                    length = orImmediateToRegisterMemory(binary, count, cpu, op);
                    break;
                }
                case 0x3 :
                {
                    length = ssbImmediateFromRegisterMemory(binary, count, cpu, op);
                    break;
                }
                case 0x4 :
                {
                    length = andImmediateToRegisterMemory(binary, count, cpu, op);
                    break;
                }
				case 0x5 : {
					length = subImmediateFromRegisterMemory(binary, count, cpu, op);
					break;
				}
				case 0x7 : {
					length = cmpImmediateWithRegisterMemory(binary, count, cpu, op);
					break;
				}
				default : {
                    printf("undefined : ");
                    binaryDump(binary, 1);
                    printf("\n");
                    exit(1);
                    break;
				}
			}
			break;
		}
        case 0x84 :
        case 0x85 :
        {
            length = testRegisterMemoryAndRegister(binary, count, cpu, op);
            break;
        }
        case 0x86 :
        case 0x87 :
        {
            length = xchgRegisterMemoryWithRegister(binary, count, cpu, op);
            break;
        }
		case 0x88 :
		case 0x89 :
		case 0x8A :
		case 0x8B : 
		{
			length = movRegisterMemoryToFromRegister(binary, count, cpu, op); 
			break;
		}
		case 0x8D : 
		{
			length = loadEAtoRegister(binary, count, cpu, op); 
			break;
		}
        case 0x90 :
        case 0x91 :
        case 0x92 :
        case 0x93 :
        case 0x94 :
        case 0x95 :
        case 0x96 :
        case 0x97 :
        {
            length = xchgExchange(binary, count, cpu, op);
            break;
        }
        case 0x98 :
        {
            length = cbwConvertByteToWord(binary, count, cpu, op);
            break;
        }
        case 0x99 :
        {
            length = cwdConvertWordToDoubleWord(binary, count, cpu, op);
            break;
        }
        case 0xA0 :
        case 0xA1 :
        {
            length = movMemoryToAccumulator(binary, count, cpu, op);
            break;
        }
        case 0xA4 :
        case 0xA5 :
        {
            length = movs(binary, count, cpu, op);
            break;
        }
        case 0xA6 :
        case 0xA7 :
        {
            length = cmps(binary, count, cpu, op);
            break;
        }
        case 0xA8 :
        case 0xA9 :
        {
            length = testImmediateDataAndAccumulator(binary, count, cpu, op);
            break;
        }
        case 0xAE :
        case 0xAF :
        {
            length = scas(binary, count, cpu, op);
            break;
        }
		case 0xB0 :
		case 0xB1 : 
		case 0xB2 : 
		case 0xB3 : 
		case 0xB4 :
		case 0xB5 : 
		case 0xB6 :
		case 0xB7 : 
		case 0xB8 :
		case 0xB9 :
		case 0xBA :
		case 0xBB :
		case 0xBC :
		case 0xBD :
		case 0xBE :
		case 0xBF : 
		{
			length = movImmediateToRegister(binary, count, cpu, op); 
			break;
		}
        case 0xC2 :
        {
            length = retWithinSegAddingImmedToSP(binary, count, cpu, op);
            break;
        }
		case 0xC3 :
		{
			length = retWithinSegment(binary, count, cpu, op);
			break;
		}
        case 0xC6 :
        case 0xC7 :
        {
            unsigned char fixed = (binary[1] >> 3) & 0x7;
            switch (fixed) {
                case 0x0:
                {
                    length = movImmediateToRegisterMemory(binary, count, cpu, op);
                    break;
                }
                default:
                {
                    printf("undefined : ");
                    binaryDump(binary, 1);
                    printf("\n");
                    
                    exit(1);
                    break;
                }
            }
            break;
        }
		case 0xCD :
		{
			length = intInterrupt(binary, count, cpu, op);
			break;
		}
        case 0xD0 :
		case 0xD1 :
        case 0xD2 :
        case 0xD3 :
        case 0xD4 :
        case 0xD5 :
        case 0xD6 :
        case 0xD7 :
		{
			unsigned char fixed = (binary[1]>>3) & 0x7;
            
			switch(fixed){
                case 0x2 :
                {
                    length = rcl(binary, count, cpu, op);
                    break;
                }
				case 0x4 :
				{
					length = shl(binary, count, cpu, op); 
					break;
				}
                case 0x5 :
                {
                    length = shr(binary, count, cpu, op);
                    break;
                }
                case 0x7 :
                {
                    length = sar(binary, count, cpu, op);
                    break;
                }
                default:
                {
                    printf("undefined : ");
                    binaryDump(binary, 1);
                    printf("\n");
                    
                    exit(1);
                    break;
                }
			}
			break;
		}
        case 0xE2 :
        {
            length = loop(binary, count, cpu, op);
            break;
        }
		case 0xE4 :
		case 0xE5 : 
		{
			length = inFixedPort(binary, count, cpu, op); 
			break;
		}
		case 0xE8 : 
		{
			length = callDirectWithinSegment(binary, count, cpu, op); 
			break;
		}
		case 0xE9 : 
		{
			length = jmpDirectWithinSegment(binary, count, cpu, op);
			break;
		}
		case 0xEB : 
		{
			length = jmpDirectWithinSegmentShort(binary, count, cpu, op); 
			break;
		}
		case 0xEC : 
		case 0xED : 
		{
			length = inVariablePort(binary, count, cpu, op); 
			break;
		}
        case 0xF2 :
        case 0xF3 :
        {
            length = rep(binary, count, cpu, op);
            break;
        }
		case 0xF4 :
		{
			length = halt(binary, count, cpu, op); 
			break;
		}
		case 0xF6 :
		case 0xF7 : 
		{
			unsigned char fixed = (binary[1]>>3) & 0x7;
            
			switch(fixed){
				case 0x0  :
				{
					length = testImmediateDataAndRegistertoEither(binary, count, cpu, op);
					break;
				}
				case 0x3  :
				{
					length = neg(binary, count, cpu, op);
					break;
				}
                case 0x4 :
                {
                    length = mulMultiply(binary, count, cpu, op);
                    break;
                }
                case 0x6 :
                {
                    length = divDivide(binary, count, cpu, op);
                    break;
                }
                default :
                {
                    printf("undefined : ");
                    binaryDump(binary, 1);
                    printf("\n");
                    
                    exit(1);
                    break;
                }
			}
			break;
		}
            
        case 0xFA :
        {
            length = cliClearInterrupt(binary, count, cpu, op);
            break;
        }
            
        case 0xFC :
        {
            length = cld(binary, count, cpu, op);
            break;
        }
            
        case 0xFD :
        {
            length = std(binary, count, cpu, op);
            break;
        }
            
		case 0xFF :
		{
			unsigned char fixed = (binary[1]>>3) & 0x7;
			switch(fixed){
                case 0x0 :
                {
                    length = incRegisterMemory(binary, count, cpu, op);
                    break;
                }
                case 0x1 :
                {
                    length = decRegisterMemory(binary, count, cpu, op);
                    break;
                }
				case 0x2 :
				{
					length = callIndirectWithinSegment(binary, count, cpu, op);
					break;
				}
				case 0x3 : 
				{
					length = callIndirectIntersegment(binary, count, cpu, op); 
					break;
				}
                case 0x4 :
                {
                    length = jmpIndirectWithinSegment(binary, count, cpu, op);
                    break;
                }
				case 0x6 :
				{
					length = pushRegisterMemory(binary, count, cpu, op); 
					break;
				}
				default : {
                    printf("undefined : ");
                    binaryDump(binary, 1);
                    printf("\n");
                    
                    exit(1);
                    break;
				}
			}
			break;
		}
            
		default   : 
		{
			printf("mnonic undefined : "); 
			binaryDump(binary,1); 
			printf("\n");
            
			exit(1); 
			break;
		}
	}


	return length;
}
