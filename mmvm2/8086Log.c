#include <stdio.h>
#include "instractionPaser.h"
#include "8086.h"
#include "textPaser.h"
#include "8086Log.h"

void cpuLog(const cpu cpu){
	printf("%04x %04x %04x %04x %04x %04x %04x %04x ",
		cpu.reg[0], cpu.reg[3], cpu.reg[1], cpu.reg[2], cpu.reg[4], cpu.reg[5], cpu.reg[6], cpu.reg[7]);
	if(cpu.o) printf("O"); else printf("-");
	if(cpu.s) printf("S"); else printf("-");
	if(cpu.z) printf("Z"); else printf("-");
	if(cpu.c) printf("C"); else printf("-");
	printf(" ");
}

void headerOfCpuLog(){
	printf(" AX   BX   CX   DX   SP   BP   SI   DI  FLAGS IP\n");
}

void instraction1Log(const instraction1 inst){
	switch(inst.mod){
		case 0x0 : 
		{
			if(inst.rm==0x6){
				if(inst.d){
					printf("%s, [%04x]", registerLog[inst.w][(int)inst.reg], inst.disp);
				}else{
					printf("[%04x], %s", inst.disp, registerLog[inst.w][(int)inst.reg]);
				}
			}else{
				if(inst.d){ 
					printf("%s, [%s]", registerLog[inst.w][(int)inst.reg], memoryLog[(int)inst.rm]);
				}else{
					printf("[%s], %s", memoryLog[(int)inst.rm], registerLog[inst.w][(int)inst.reg]);
				}
			} 
			break;
		}
		case 0x1 : 
		{
			if(inst.d){
                if (inst.disp>0) {
                    printf("%s, [%s+%x]", registerLog[inst.w][(int)inst.reg], memoryLog[(int)inst.rm], inst.disp);
                }else{
                    printf("%s, [%s-%x]", registerLog[inst.w][(int)inst.reg], memoryLog[(int)inst.rm], (~inst.disp)+1);
                }
				
			}else{
				if(inst.disp>0){
					printf("[%s+%x], %s", memoryLog[(int)inst.rm], inst.disp, registerLog[inst.w][(int)inst.reg]);
				}else{
					printf("[%s-%x], %s", memoryLog[(int)inst.rm], (~inst.disp)+1, registerLog[inst.w][(int)inst.reg]);
				}
				
			}
			break;
		}
		case 0x2 : 
		{
            if (inst.d) {
                if(inst.disp>0){
                    printf("%s, [%s+%x]", registerLog[inst.w][(int)inst.reg], memoryLog[(int)inst.rm], inst.disp);
                }else{
                    printf("%s, [%s-%x]", registerLog[inst.w][(int)inst.reg], memoryLog[(int)inst.rm], (~inst.disp)+1);
                }
            }else{
                if(inst.disp>0){
                    printf("[%s+%x], %s", memoryLog[(int)inst.rm], inst.disp, registerLog[inst.w][inst.reg]);
                }else{
                    printf("[%s-%x], %s", memoryLog[(int)inst.rm], (~inst.disp)+1, registerLog[inst.w][inst.reg]);
                }
            }
			
			break;
		}
		case 0x3 : 
		{
			if(inst.d){
				printf("%s, %s", registerLog[inst.w][(int)inst.reg], registerLog[inst.w][(int)inst.rm]);
			}else{
				printf("%s, %s", registerLog[inst.w][(int)inst.rm], registerLog[inst.w][(int)inst.reg]);
			}
			break;
		}
	}
}

void instraction3Log(const instraction3 inst){
	printf("%s, ", registerLog[inst.w][(int)inst.reg]);
	if(inst.w){
		printf("%02x%02x\n", inst.data[1], inst.data[0]);
	}else{

	}
}

void instraction4Log(const instraction4 inst){
	switch(inst.mod){
		case 0x0 : 
		{
            if (inst.rm == 6) {
                printf("[%04x], %x", inst.disp, inst.data[0]);
            }else if(inst.s==0 && inst.w == 1){
				printf("[%s], %x%x", memoryLog[(int)inst.rm], inst.data[0], inst.data[1]);
			}else if(inst.s == 1 && inst.w == 1){
                printf("[%s], %x", memoryLog[inst.rm], inst.data[0]);
            }else if(inst.s == 0 && inst.w == 0){
                printf("byte [%s], %x", memoryLog[inst.rm], inst.data[0]);
            }
			break;
		}
        case 0x2 :
        {
            if (inst.s == 0 && inst.w == 1) {
                if (inst.disp > 0) {
                    printf("[%s+%x], %02x%02x", memoryLog[(int)inst.rm], inst.disp, inst.data[1], inst.data[0]);
                }else{
                    printf("[%s-%x], %02x%02x", memoryLog[(int)inst.rm], (~inst.disp)+1, inst.data[1], inst.data[0]);
                }
            }else{
                if (inst.disp > 0) {
                    printf("[%s+%x], %x", memoryLog[(int)inst.rm], inst.disp, inst.data[0]);
                }else{
                    printf("[%s-%x], %x", memoryLog[(int)inst.rm], (~inst.disp)+1, inst.data[0]);
                }
            }
            
            break;
        }
        
        case 0x1 :
        {
            if (inst.s == 0 && inst.w == 1) {
                if (inst.disp > 0) {
                    printf("[%s+%x], %02x%02x", memoryLog[(int)inst.rm], inst.disp, inst.data[1], inst.data[0]);
                }else{
                    printf("[%s-%x], %02x%02x", memoryLog[(int)inst.rm], (~inst.disp)+1, inst.data[1], inst.data[0]);
                }
            }else{
                if (inst.disp > 0) {
                    printf("[%s+%x], %x", memoryLog[(int)inst.rm], inst.disp, inst.data[0]);
                }else{
                    printf("[%s-%x], %x", memoryLog[(int)inst.rm], (~inst.disp)+1, inst.data[0]);
                }
            }
            
            break;
        }
		case 0x3 :
		{
			if(inst.s == 0 && inst.w == 1){
				printf("%s, %02x%02x", registerLog[inst.w][(int)inst.rm], inst.data[1], inst.data[0]);
			}else if(inst.s == 1 && inst.w == 1){
                if (inst.data[0]>>7) {
                    printf("%s, -%x", registerLog[inst.w][(int)inst.rm], (~(char)inst.data[0])+1);
                }else{
                    printf("%s, %x", registerLog[inst.w][(int)inst.rm], inst.data[0]);
                }
                
			}else{
				printf("%s, %x", registerLog[inst.w][(int)inst.rm], inst.data[0]);
			}
			break;
		}
	}
}

void instraction5Log(const instraction5 inst, const int count){
	printf("%04x", inst.disp+inst.length+count);
}

void instraction6Log(const instraction6 inst, const int count){
    
    switch(inst.mod){
        case 0x0 :
        {
            if (inst.rm == 6) {
                if (inst.w) {
                    printf("[%04x], %s", inst.disp, registerLog[inst.w][inst.rm]);
                }else{
                    printf("[%04x], %s", inst.disp, registerLog[inst.w][inst.rm]);
                }
            }
            break;
        }
        case 0x1 :
        {
            if(inst.disp>0){
                printf("[%s+%x]", memoryLog[(int)inst.rm], inst.disp);
            }else{
                printf("[%s-%x]", memoryLog[(int)inst.rm], (~inst.disp)+1);
            }
            break;
        }
        case 0x3 :
        {
            printf("%s", registerLog[inst.w][(int)inst.rm]);
            break;
        }
    }
    
}

void instraction7Log(const instraction7 inst, const int count){
    
    switch (inst.mod) {
        case 0x0 :
        {
            if (inst.rm == 6) {
                printf("[%04x], %02x%02x", inst.disp, inst.data[1], inst.data[0]);
            }else if (inst.w) {
                printf("[%s], %02x%02x", memoryLog[inst.rm], inst.data[1], inst.data[0]);
            }else{
                printf("byte [%s], %02x", memoryLog[inst.rm], inst.data[0]);
            }
            break;
        }
        case 0x1 :
        {
            if (!inst.w) {
                printf("byte ");
            }
            
            if (inst.disp > 0) {
                printf("[%s+%x], ", memoryLog[(int)inst.rm], inst.disp);
            }else{
                printf("[%s-%x], ", memoryLog[(int)inst.rm], (~inst.disp)+1);
            }
            
            if(inst.w){
                printf("%02x%02x", inst.data[1], inst.data[0]);
            }else{
                printf("%x", inst.data[0]);
            }
            break;
            
        }
        case 0x2 :
        {
            printf("undefined");
            break;
        }
        case 0x3 :
        {
            printf("%s, ", registerLog[inst.w][(int)inst.rm]);
            
            if(inst.w){
                printf("%02x%02x", inst.data[1], inst.data[0]);
            }else{
                printf("%x", inst.data[0]);
            }
            
            break;
        }
    }
    
}

void instraction9Log(const instraction9 inst, const int count){
	printf("%04x", inst.disp+inst.length+count);
}

void instraction11Log(const instraction11 inst){
	switch(inst.mod){
		case 0x3 : 
		{
			if(inst.v){
				printf("%s, cl", registerLog[inst.w][(int)inst.rm]);
			}else{
				printf("%s, %d", registerLog[inst.w][(int)inst.rm], 1);
			}
			break;
		}
	}
}

void instraction12Log(const instraction12 inst){
	switch(inst.mod){
        case 0x0 :
        {
            if (inst.rm == 6) {
                if (inst.disp > 0) {
                    printf("[%04x]", inst.disp);
                }else{
                    printf("[%04x]", (~inst.disp)+1);
                }
            }else{
                printf("[%s]", memoryLog[inst.rm]);
            }
            break;
        }
		case 0x1 :
        case 0x2 :
        {
            if (inst.disp > 0) {
                printf("[%s+%x]", memoryLog[(int)inst.rm], inst.disp);
            }else{
                printf("[%s-%x]", memoryLog[(int)inst.rm], (~inst.disp)+1);
            }
            
			break;
        }
		case 0x3 :
		{
			printf("%s", registerLog[1][(int)inst.rm]); 
			break;
		}
	}
}

void instraction14Log(const instraction14 inst){
    if (inst.w) {
        printf("ax, %02x%02x\n", inst.data[1], inst.data[0]);
    }else{
        printf("al %02x\n", inst.data[0]);
    }
}

void instraction16Log(const instraction16 inst){
    
    switch (inst.mod) {
        case 0x0 :
        {
            if (inst.rm == 6) {
                printf("[%04x], %s", inst.disp, registerLog[inst.w][inst.reg]);
            }
            break;
        }
    }
}