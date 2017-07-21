#ifndef _HEADERPASER_H_
#define _HEADERPASER_H_

typedef struct header{
	unsigned char magic[2];
	unsigned char flags;
	unsigned char cpu;
	unsigned char length;
	long text;
	long data;
	long bss;
	long total;
	long syms;
}header;

extern void parseHeader(header* header, const unsigned char* binary);
extern void showHeader(const header header);

#endif