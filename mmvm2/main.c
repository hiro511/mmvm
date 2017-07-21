// #define NDEBUG
// #define BINARY_SIZE 10000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "disassemble.h"
#include "headerPaser.h"

void printWhiteSpace(int size);

int main(int argc, char* argv[]){

	if(!strcmp(argv[1], "-d")){
		disassembleWithFilename(argv[2], DISASSEMBLE, argc-2, &argv[2]);
	}else if(!strcmp(argv[1], "-m")){
		disassembleWithFilename(argv[2], MONITOR, argc-2, &argv[2]);
    }else {
        disassembleWithFilename(argv[1], NONE, argc-1, &argv[1]);
    }

	printf("all end\n");
	return 0;
}
