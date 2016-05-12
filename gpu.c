#include "gbmu.h"
#include "gpu.h"
#include <SDL/SDL.h>

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			*p = pixel;
			break;

		case 2:
			*(Uint16 *)p = pixel;
			break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;

		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}

void putpixel_scaled(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	putpixel(surface, x * 2, y * 2, pixel);
	putpixel(surface, x * 2 + 1, y * 2, pixel);
	putpixel(surface, x * 2, y * 2 + 1, pixel);
	putpixel(surface, x * 2 + 1, y * 2 + 1, pixel);
}

int init_gpu(struct gbmu *gbmu)
{
	SDL_Init(SDL_INIT_VIDEO);
	gbmu->gpu.screen = SDL_SetVideoMode(160 * 2, 144 * 2, 32, SDL_HWSURFACE);
	(void) gbmu;
	return 0;
}

static unsigned char getpxtile(unsigned char *tiles, int tile, int x, int y)
{
	int x2 = 7 - x;
	int yolo = 0x1 << x2;
	int yolo2 = x2;
	unsigned char a = tiles[tile * 16 + y * 2 ] & yolo;
	unsigned char b = tiles[tile * 16 + y * 2 + 1] & yolo;

	if (yolo2 <= 0)
		return a | b << 1;
	return ((a >> yolo2 | b >> (yolo2 - 1))) & 3;
}

int gpu_step(struct gbmu *gbmu)
{
	static int color[] = {0xFF, 0xBD, 0x7E, 0};
	int x;
	int y;

	gbmu->gpu.ticks++;

	x = gbmu->gpu.ticks % 1232;
	y = gbmu->gpu.ticks / 1232;

	if (x < 160 && y < 144) {
		int tileid = gbmu->ram.memory[0x9800 + (y / 8) * 32 + (x / 8)] & 0xff;

		if (SDL_MUSTLOCK(gbmu->gpu.screen))
			SDL_LockSurface(gbmu->gpu.screen);

		int colorn = color[getpxtile((unsigned char *)gbmu->ram.memory + 0x8000, tileid, x % 8, y % 8)];

		//if (tileid != 0x2F) {
		putpixel_scaled(gbmu->gpu.screen, x, y, SDL_MapRGB(gbmu->gpu.screen->format, colorn, colorn, colorn));

		if (SDL_MUSTLOCK(gbmu->gpu.screen))
			SDL_UnlockSurface(gbmu->gpu.screen);
	}

	write_byte(&gbmu->ram, 0xFF44, y);
	if (gbmu->gpu.ticks == 197120)
		FLAGS_SET(gbmu->cpu.interrupts.incoming_interrupts, INT_VBLANK);

	if (gbmu->gpu.ticks > 280986) {
		gbmu->gpu.ticks = 0;
		SDL_Flip(gbmu->gpu.screen);
	}
	return 0;
}

void printtile(struct gbmu *gbmu, FILE *f, int tileid)
{
	int x, y;

	y = 0;
	while (y < 8) {
		x = 0;
		while (x < 8) {
			fprintf(f, "%02x ", getpxtile((unsigned char *)gbmu->ram.memory + 0x8000, tileid, x, y));
			x++;
		}
		fprintf(f, "\n");
		y++;
	}
}

void print(struct gbmu *gbmu, FILE *f)
{
	int x, y;

	y = 0;
	while (y < 32) {
		x = 0;
		while (x < 32) {
			fprintf(f, "%02x ", gbmu->ram.memory[0x9800 + y * 32 + x] & 0xFF);
			x++;
		}
		fprintf(f, "\n");
		y++;
	}
}
