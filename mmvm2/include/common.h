#ifndef _COMMON_H_
#define _COMMON_H_

#define BINARY_SIZE 10000

#include <stdio.h>
#include <stdlib.h>

enum BYTE {
    AL = 0,
    CL = 1,
    DL = 2,
    BL = 3,
    AH = 4,
    CH = 5,
    DH = 6,
    BH = 7
};

enum WORD {
    AX = 0,
    CX = 1,
    DX = 2,
    BX = 3,
    SP = 4,
    BP = 5,
    SI = 6,
    DI = 7
};

//File
extern FILE* openFile(const char* filename);
extern void closeFile(FILE* fp);
extern void readFileToChar(unsigned char* binary, FILE *fp);

//Debug Log
extern void hexDump(const unsigned char* binary, const int size);
extern void hexDumpWith16bite(const unsigned char* binary, const int size);
extern void binaryDump(const unsigned char* originalBinary, const int size);
extern void printWhiteSpace(int size);
extern void hexDumpWithWhiteSpace(const unsigned char* binary, const int size);

//Convert
extern void littleToBigEndian(unsigned char* littleEndian2bite);

#endif
