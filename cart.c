#include "gbmu.h"
#include "cart.h"

#include <fcntl.h>
#include <stdio.h>

enum cart_table {
	RESTART_AND_INT_TABLE = 0x0000,
	CARTRIDGE_HEADER = 0x0100,
	CART_BANKS = 0x0150,
	BANK_ROM_1_END = 0x7FFF
};

enum cartridge_header {
	START_INSTRUCTION = 0x0100,
	NINTENDO_LOGO = 0x0103,
	GAME_TITLE = 0x0134,
	COLOR_COMPATIBILITY = 0x0143,
	LICENSEE_CODE = 0x0144,
	SGB_COMPATIBILITY = 0x0146,
	CART_TYPE = 0x0147,
	CART_ROM_SIZE = 0x0148,
	CART_RAM_SIZE = 0x0149,
	DESTINATION_CODE = 0x014A,
	OLD_LICENSEE_CODE = 0x014B,
	MASK_ROM_VERSION = 0x014C,
	CHECKSUM = 0x014B,
	HEADER_END = 0x014F
};

static int get_file_length(FILE *file)
{
	int result;

	fseek(file, 0, SEEK_END);
	result = ftell(file);
	rewind(file);
	return result;
}

int load_cart(struct ram *ram, struct cart *cart)
{
	int status;

	rewind(cart->file);
	status = fread(ram->memory, 1, VIDEO_RAM, cart->file);
	if (status == 0) {
		perror("fread");
		return (1);
	}
	return (0);
}

int read_cart(struct cart *cart, char *path)
{
	char header[HEADER_END];
	int status;

	cart->file = fopen(path, "r");
	if (cart->file == NULL) {
		perror("fopen");
		return (1);
	}

	cart->file_size = get_file_length(cart->file);
	if (cart->file_size < BANK_ROM_1_END) {
		fprintf(stderr, "cart file too small ! (cart is %ld, excepted at least %d)\n", cart->file_size, BANK_ROM_1_END);
		return (1);
	}
	status = fread(header, 1, HEADER_END, cart->file);
	if (status <= 0) {
		perror("fread");
		return (1);
	}

	printf("Cart name: %.16s\n\n", header + GAME_TITLE);
	return (0);
}
