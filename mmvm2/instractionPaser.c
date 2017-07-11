#include <stdio.h>
#include <string.h>
#include "instractionPaser.h"

void parseInstraction1(instraction1* inst, const unsigned char* binary){
	inst->d = (*binary>>1) & 0x1;
	inst->w = *binary & 0x1;
	inst->mod = binary[1]>>6;
	inst->reg = (binary[1]>>3)&0x7;
	inst->rm = binary[1]&0x7;

	switch(inst->mod){
		case 0x0 :
        {
            if(inst->rm==0x6){
                memcpy(&inst->disp, &binary[2],2);
                inst->length = 4;
            }else{
                inst->disp = 0;
                inst->length = 2;
            }
            break;
        }
		case 0x1 : inst->length = 3; inst->disp = (char)binary[2]; break;
		case 0x2 : inst->length = 4; memcpy(&inst->disp, &binary[2], 2); break;
		case 0x3 : inst->length = 2; memcpy(&inst->disp, &binary[2], 2); break;
	}
}

void parseInstraction2(instraction2* inst, const unsigned char* binary){
	inst->mod = binary[1]>>6;
	inst->reg = (binary[1]>>3)&0x7;
	inst->rm = binary[1]&0x7;

	switch(inst->mod){
		case 0x1 :
        {
            inst->disp = (char)binary[2];
            inst->length = 3;
            break;
        }
        case 0x3 :
        {
            inst->length = 2;
            break;
        }
		default  :
        {
            memcpy(&inst->disp, &binary[2], 2);
            inst->length = 4;
            break;
        }
	}
}

void parseInstraction3(instraction3* inst, const unsigned char* binary){
	inst->w = (*binary>>3) & 0x1;
	inst->reg = *binary & 0x7;
	if(inst->w){
		memcpy(&inst->data, &binary[1],2);
		inst->length = 3;
	}else{
		inst->data[0] = binary[2];
		inst->length = 2;
	}
}

void parseInstraction4(instraction4* inst, const unsigned char* binary){
	inst->s = (binary[0]>>1)&0x1;
	inst->w = binary[0]&0x1;
	inst->mod = (binary[1]>>6)&0x3;
	inst->fixed = (binary[1]>>3)&0x7;
	inst->rm = binary[1]&0x7;

    switch (inst->mod) {
        case 0x0 :
        {
            if(inst->rm == 6){
                memcpy(&inst->disp, &binary[2], 2);
                inst->data[0] = binary[4];
                inst->length = 5;
            }else if(inst->s == 0 && inst->w == 1){
                inst->disp = 0;
                memcpy(&inst->data, &binary[2], 2);
                inst->length = 4;
            }else{
                inst->disp = 0;
                inst->data[0] = binary[2];
                inst->length = 3;
            }
            break;
        }
        case 0x2 :
        {
            memcpy(&inst->disp, &binary[2], 2);
            
            if (inst->s == 0 && inst->w == 1) {
                inst->length = 6;
                memcpy(inst->data, &binary[4], 2);
            }else{
                inst->length = 5;
                inst->data[0] = (char)binary[4];
            }
            break;
        }
        case 0x1 :
        {
//            if (inst->s == 1 && inst->w == 1 && inst->mod == 0x11) {
//                inst->data[0] = binary[2];
//                inst->length = 3;
//            }else{
//                memcpy(inst->data, &binary[2], 2);
//                inst->length = 4;
//            }
//            break;
            
            if (inst->s == 0 && inst->w == 1) {
                inst->disp = (char)binary[2];
                memcpy(inst->data, &binary[3], 2);
                inst->length = 5;
            }else{
                inst->data[0] = binary[3];
                inst->disp = (char)binary[2];
                inst->length = 4;
            }
            break;
        }
        case 0x3 :
        {
            if(inst->s == 0 && inst->w == 1){
                memcpy(inst->data, &binary[2], 2);
                inst->length = 4;
            }else{
                inst->length = 3;
                inst->data[0] = binary[2];
            }
            break;
        }
    }
}

void parseInstraction5(instraction5* inst, const unsigned char* binary){
	inst->length = 2;
	inst->disp = binary[1];
}

void parseInstraction6(instraction6* inst, const unsigned char* binary){
	inst->w = binary[0] & 0x1;
	inst->mod = (binary[1]>>6)&0x3;
	inst->rm = binary[1]&0x7;
    
    switch (inst->mod) {
        case 0x0 :
        {
            if (inst->w) {
                memcpy(&inst->disp, &binary[2], 2);
                inst->length = 4;
            }else{
                inst->disp = binary[2];
                inst->length = 3;
            }
            break;
        }
        case 0x1 :
        {
            inst->disp = (char)binary[2];
            inst->length = 3;
            break;
        }
        default:
        {
            inst->length = 2;
            break;
        }
    }
}

void parseInstraction7(instraction7* inst, const unsigned char* binary){
	inst->w = binary[0]&0x1;
	inst->mod = (binary[1]>>6)&0x3;
	inst->rm = binary[1]&0x7;
    
    switch (inst->mod) {
        case 0x0 :
        {
            if(inst->rm == 6){
                memcpy(&inst->disp, &binary[2], 2);
                memcpy(inst->data, &binary[4], 2);
                inst->length = 6;
            }else if(inst->w){
                memcpy(inst->data, &binary[2],2);
                inst->length = 4;
            }else{
                inst->data[0] = binary[2];
                inst->length = 3;
            }
            break;
        }
        case 0x1 :
        {
            inst->disp = (char)binary[2];
            
            if(inst->w){
                memcpy(inst->data, &binary[3],2);
                inst->length = 5;
            }else{
                inst->data[0] = binary[3];
                inst->length = 4;
            }
            break;
        }
        case 0x2 :
        {
            memcpy(&inst->disp, &binary[2], 2);
            if (inst->w) {
                memcpy(inst->data, &binary[4], 2);
                inst->length = 6;
            }else{
                inst->data[0] = binary[4];
                inst->length = 5;
            }
            break;
        }
        default:
        {
            if(inst->w){
                memcpy(inst->data, &binary[2],2);
                inst->length = 4;
            }else{
                inst->data[0] = binary[2];
                inst->length = 3;
            }
        }
    }
    
}

void parseInstraction8(instraction8* inst, const unsigned char* binary){
	inst->length = 1;
	inst->reg = binary[0] & 0x7;
}

void parseInstraction9(instraction9* inst, const unsigned char* binary){
	inst->length = 3;
	memcpy(&inst->disp, &binary[1], 2);
}

void parseInstraction10(instraction10* inst, const unsigned char* binary){
	inst->length = 1;
	inst->reg = binary[0]&0x7;
}

void parseInstraction11(instraction11* inst, const unsigned char* binary){
	inst->length = 2;
	inst->v = (*binary>>1) & 0x1;
	inst->w = *binary & 0x1;
	inst->mod = binary[1]>>6;
	inst->fixed = (binary[1]>>3)&0x7;
	inst->rm = binary[1]&0x7;
}

void parseInstraction12(instraction12* inst, const unsigned char* binary){
	inst->length = 2;
	inst->mod = (binary[1]>>6)&0x3;
	inst->fixed = (binary[0]>>3)&0x3;
	inst->rm = binary[1]&0x7;
    
	switch(inst->mod){
        case 0x0 :
        {
            if (inst->rm == 6) {
                memcpy(&inst->disp, &binary[2], 2);
                inst->length = 4;
            }else{
                inst->disp = 0;
                inst->length = 2;
            }
            break;
        }
		case 0x1 :
        {
            inst->disp = (char)binary[2];
            inst->length = 3;
            break;
        }
        case 0x2 :
        {
            memcpy(&inst->disp, &binary[2], 2);
            inst->length = 4;
            break;
        }
	}
}

void parseInstraction13(instraction13* inst, const unsigned char* binary){
	inst->length = 2;
	inst->w = binary[0]&0x1;
	inst->port = binary[1];
}

void parseInstraction14(instraction14* inst, const unsigned char* binary){
    inst->w = binary[0]&0x1;
    if (inst->w) {
        inst->length = 3;
        memcpy(inst->data, &binary[1], 2);
    }else{
        inst->length = 2;
        inst->data[0] = binary[1];
    }
}

void parseInstraction15(instraction15* inst, const unsigned char* binary){
    inst->length = 1;
    inst->w = binary[0] & 0x1;
}

void parseInstraction16(instraction16* inst, const unsigned char* binary){
    inst->w = binary[0] & 0x1;
    inst->mod = binary[1] >> 6;
    inst->reg = (binary[1] >> 3) & 0x7;
    inst->rm = binary[1] & 0x7;
    
    if (inst->w) {
        memcpy(&inst->disp, &binary[2], 2);
        inst->length = 4;
    }else{
        inst->disp = binary[2];
        inst->length = 3;
    }
    
}


