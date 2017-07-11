#ifndef _TEXTPASER_H_
#define _TEXTPASER_H_

#include "instractionPaser.h"
#include "disassemble.h"
#include "headerPaser.h"
#include "8086.h"

typedef struct{
    unsigned char* binary;
    header header;
    int start;
    int end;
    enum Option op;
    
    int argc;
    char *argv[];
} textParserArgs;

static const char registerLog[2][8][3] =
	{
		{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"},
		{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
	};

static const char memoryLog[8][10] =
	{"bx+si", "bx+di", "bp+si", "bp+di", "si", "di", "bp", "bx"};

//parser
extern void parseText(const unsigned char* binary, const header header, const int start, const int end, const enum Option op);
extern int parseMnemonic(const unsigned char* binary, int count, cpu* cpu, const enum Option op);

#endif
