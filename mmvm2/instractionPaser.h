#ifndef _INSTRACTIONPASER_H_
#define _INSTRACTIONPASER_H_

//ex : 000000dw mod_reg_r/m
typedef struct instraction1{
	int length;
	int d;
	int w;
	char mod;
	char reg;
	char rm;
	short disp;
    short data;
}instraction1;

//ex : 10001101 mod_reg_r/m
typedef struct instraction2{
	int length;
	char mod;
	char reg;
	char rm;
	short disp;
}instraction2;

// ex : 1011_w_reg data data
typedef struct instraction3{
	int length;
	int w;
	char reg;
	unsigned char data[2];
}instraction3;

//ex : 111101sw mod_fixed_rm data data
typedef struct instraction4{
	int length;
	int s;
	int w;
	char mod;
	char fixed;
	char rm;
	unsigned char data[2];
	short disp;
}instraction4;

//ex : 01111000 disp
typedef struct instraction5{
	int length;
	char disp;
}instraction5;

// ex : 1111011w mod_111_r/m
typedef struct instraction6{
	int length;
	int w;
	char mod;
	char rm;
    short disp;
}instraction6;

//ex : 1111011w mod_000_rm data data
typedef struct instraction7{
	int length;
	int w;
	char mod;
	char rm;
	unsigned char data[2];
    short disp;
}instraction7;

//ex : 01010reg
typedef struct instraction8{
	int length;
	int reg;
}instraction8;

//ex : 11101001 disp disp
typedef struct instraction9{
	int length;
	short disp;
}instraction9;

//ex : 01001reg
typedef struct instraction10{
	int length;
	char reg;
}instraction10;

//ex : 110100vw mod_fixed_r/m
typedef struct instraction11{
	int length;
	int v;
	int w;
	char mod;
	char fixed;
	char rm;
}instraction11;

//ex : 11111111 mod_fixed_r/m
typedef struct instraction12{
	int length;
	char mod;
	char fixed;
	char rm;
	short disp;
}instraction12;

//ex : 1110010x port
typedef struct instraction13{
	int length;
	int w;
	unsigned char port;
}instraction13;

//ex : 0010110w data data
typedef struct instraction14{
    int length;
    int w;
    unsigned char data[2];
}instraction14;

//ex : 1111001w
typedef struct instraction15{
    int length;
    int w;
}instraction15;

//ex : 1000011w mod_reg_r/m
typedef struct instraction16{
    int length;
    int w;
    char mod;
    char reg;
    char rm;
    short disp;
}instraction16;

extern void parseInstraction1(instraction1* inst, const unsigned char* binary);
extern void parseInstraction2(instraction2* inst, const unsigned char* binary);
extern void parseInstraction3(instraction3* inst, const unsigned char* binary);
extern void parseInstraction4(instraction4* inst, const unsigned char* binary);
extern void parseInstraction5(instraction5* inst, const unsigned char* binary);
extern void parseInstraction6(instraction6* inst, const unsigned char* binary);
extern void parseInstraction7(instraction7* inst, const unsigned char* binary);
extern void parseInstraction8(instraction8* inst, const unsigned char* binary);
extern void parseInstraction9(instraction9* inst, const unsigned char* binary);
extern void parseInstraction10(instraction10* inst, const unsigned char* binary);
extern void parseInstraction11(instraction11* inst, const unsigned char* binary);
extern void parseInstraction12(instraction12* inst, const unsigned char* binary);
extern void parseInstraction13(instraction13* inst, const unsigned char* binary);
extern void parseInstraction14(instraction14* inst, const unsigned char* binary);
extern void parseInstraction15(instraction15* inst, const unsigned char* binary);
extern void parseInstraction16(instraction16* inst, const unsigned char* binary);
#endif