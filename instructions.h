#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

struct gbmu;
typedef void (*instruction_func)(struct gbmu *gbmu, unsigned short operand);

struct instruction {
	instruction_func func;
	unsigned char opcode;
	unsigned char paramcount;
	char *debugname;
};

extern struct instruction instructions[256];

#endif
