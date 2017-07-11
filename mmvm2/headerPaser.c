#include <stdio.h>
#include <string.h>
#include "headerPaser.h"

void parseHeader(header* header, const unsigned char* binary){
	memcpy(header->magic, binary, sizeof(char)*2);
	header->flags = binary[2];
	header->cpu = binary[3];
	header->length = binary[4];
	memcpy(&header->text, &binary[8], sizeof(char)*4);
	memcpy(&header->data, &binary[12], sizeof(char)*4);
	memcpy(&header->bss, &binary[16], sizeof(unsigned char)*4);
	memcpy(&header->total, &binary[20], sizeof(char)*4);
	memcpy(&header->syms, &binary[24], sizeof(char)*4);

	//showHeader(*header); //for debug
}

void showHeader(const header header){
	printf("- Header -\n");
	printf("magic : %02X %02X\n", header.magic[0], header.magic[1]);
	printf("cpu   : %02X\n", header.cpu);
	printf("length: %d\n", header.length);
	printf("text  : %ld\n", header.text);
	printf("data  : %ld\n", header.data);
	printf("bss   : %ld\n", header.bss);
	printf("total : %ld\n", header.total);
	printf("syms  : %ld\n", header.syms);
}