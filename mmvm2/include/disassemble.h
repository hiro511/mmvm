#ifndef _DISASSEMBLE_H_
#define _DISASSEMBLE_H_

enum Option {
	MONITOR,
    DISASSEMBLE,
	NONE
};

extern void disassembleWithFilename(const char* filename, const enum Option op, const int argc, char* argv[]);
extern void disassemble(const unsigned char* binary, const enum Option op, const int argc, char* argv[]);

#endif
