#include <stdio.h>
#include "common.h"
#include "headerPaser.h"
#include "textPaser.h"
#include "disassemble.h"

void disassembleWithFilename(const char* filename, const enum Option op, const int argc, char* argv[]){
	unsigned char binary[BINARY_SIZE];

	FILE *fp = openFile(filename);
	readFileToChar(binary, fp);
	closeFile(fp);

	disassemble(binary, op, argc, argv);
}

void disassemble(const unsigned char* binary, const enum Option op, const int argc, char* argv[]){
	header header = {{0x0,0x0}, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	parseHeader(&header, binary);
    
    /* Debug */
	//showHeader(header);
    
	parseText(binary, header, header.length, (int)(header.length + header.text), op, argc, argv);
}
