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
    int pArgc = argc;
    char** pArgv = argv;

	if(!strcmp(argv[1], "-d")){
		disassembleWithFilename(argv[2], DISASSEMBLE, pArgc, pArgv);
	}else if(!strcmp(argv[1], "-m")){
		disassembleWithFilename(argv[2], MONITOR, pArgc, pArgv);
	}else if(argc == 3){
		printf("unknown option: %s\n", argv[1]);
    }else if(argc == 2){
        disassembleWithFilename(argv[1], NONE, pArgc, pArgv);
    }

	printf("all end\n");
	return 0;
}
