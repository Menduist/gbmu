#include "gbmu.h"
#include "ram.h"
#include <stdlib.h>

int init_ram(struct ram *ram)
{
	ram->memory = malloc(0xFFFF);
	return (0);
}

unsigned char read_byte(struct ram *ram, int position)
{
	//if (position == 0xff00) __asm("int $3");
	return ram->memory[position];
}

unsigned short read_short(struct ram *ram, int position)
{
	return read_byte(ram, position) | (read_byte(ram, position + 1) << 8);
}

extern struct gbmu *ggbmu;
void write_byte(struct ram *ram, int position, unsigned char towrite)
{
	if (position == 0xff80) return;
	if (position == 0xff0f) towrite |= 0xE0;
	ram->memory[position] = towrite;
}

void write_short(struct ram *ram, int position, unsigned short towrite)
{
	write_byte(ram, position, (unsigned char)(towrite & 0x00FF));
	write_byte(ram, position + 1, (unsigned char)((towrite & 0xFF00) >> 8));
}

void push_byte(struct ram *ram, struct registers *reg, unsigned char towrite)
{
	reg->sp--;
	write_byte(ram, reg->sp, towrite);
}

void push_short(struct ram *ram, struct registers *reg, unsigned short towrite)
{
	reg->sp -= 2;
	write_short(ram, reg->sp, towrite);
}

unsigned char pop_byte(struct ram *ram, struct registers *reg)
{
	reg->sp++;
	return read_byte(ram, reg->sp - 1);
}

unsigned short pop_short(struct ram *ram, struct registers *reg)
{
	reg->sp += 2;
	return read_short(ram, reg->sp - 2);
}
