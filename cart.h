#ifndef CART_H
#define CART_H

#include <stdio.h>
#include "ram.h"

struct cart {
	FILE *file;
	long file_size;
	int color_mode;
	int rom_bank_count;
	int ram_bank_count;
};

int read_cart(struct cart *cart, char *path);
int load_cart(struct ram *ram, struct cart *cart);

#endif
