#ifndef RAM_H
#define RAM_H

enum memory_map {
	VIDEO_RAM = 0x8000,
	MEMORY_END = 0xFFFF
};

struct ram {
	unsigned char *memory;
};
struct registers;

int init_ram(struct ram *ram);

unsigned char read_byte(struct ram *ram, int position);
unsigned short read_short(struct ram *ram, int position);

void write_byte(struct ram *ram, int position, unsigned char towrite);
void write_short(struct ram *ram, int position, unsigned short towrite);

void push_byte(struct ram *ram, struct registers *reg, unsigned char towrite);
void push_short(struct ram *ram, struct registers *reg, unsigned short towrite);

unsigned char pop_byte(struct ram *ram, struct registers *reg);
unsigned short pop_short(struct ram *ram, struct registers *reg);

#endif
