#ifndef GPU_G
#define GPU_G

#include <SDL/SDL.h>

struct gpu {
	int scanline;
	int ticks;
	int lasttick;

	SDL_Surface *screen;
};

struct gbmu;

int init_gpu(struct gbmu *gbmu);

int gpu_step(struct gbmu *gbmu);

#endif
