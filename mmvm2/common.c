#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"

FILE* openFile(const char* filename){
	FILE *fp;

	fp = fopen(filename, "rb");
	if(fp == NULL){
		printf("failed to open %s\n", filename);
		exit(0);
	}
	return fp;
}

void closeFile(FILE* fp){
	fclose(fp);
}

void readFileToChar(unsigned char* binary, FILE* fp){
	long size = fread(binary, sizeof(char), BINARY_SIZE, fp);

	assert(size < BINARY_SIZE); //for debug
	assert(size > 0);  //for debug
	
	//hexDumpWith16bite(binary, size); //for debug
}

void hexDump(const unsigned char* binary, const int size){
	for(int i=0; i<size; i++){
		printf("%02x", binary[i]);
	}
}

void hexDumpWithWhiteSpace(const unsigned char* binary, const int size){
	hexDump(binary, size);
	printWhiteSpace(13-size*2);
}

void hexDumpWith16bite(const unsigned char* binary, const int size){
	printf("- binary -\n");
	for(int i=0; i < size; i++){
		printf("%02X ", binary[i]);
		if(i % 15 == 0 && i != 0) printf("\n");
	}
	printf("\n");
}

void binaryDump(const unsigned char* originalBinary, const int size){
	unsigned char binary[size];
	memcpy(binary, originalBinary, size);
	//printf("- binary -\n");
	for(int k = 0; k < size; k++){
		for (int i = 7; i >= 0; i--){
			printf("%d", (binary[k] >> i) & 1);
		}
		printf(" ");
	}
	printf("\n");
}

void littleToBigEndian(unsigned char* littleEndian2bite){
	unsigned char binary[2];
	memcpy(binary, littleEndian2bite, sizeof(char)*2);
	littleEndian2bite[1] = binary[0];
	littleEndian2bite[0] = binary[1];


	// for debug
	// unsigned char tmpBinary[4];
	// memcpy(tmpBinary, littleEndian4bite, sizeof(char)*4);
	// hexDumpWith16bite(binary, 4);
	// hexDumpWith16bite(tmpBinary, 4);
}

void printWhiteSpace(int size){
	for(int i=0; i<size; i++){
		printf(" ");
	}
}