#include "gbmu.h"
#include "input.h"

void update_inputs(struct gbmu *gbmu)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			gbmu->input.keypressed[event.key.keysym.sym] = 1;
			printf("%d pressed\n", event.key.keysym.sym);
		}
		if (event.type == SDL_KEYUP) {
			gbmu->input.keypressed[event.key.keysym.sym] = 0;
		}
	}
	unsigned char orbyte = read_byte(&gbmu->ram, 0xFF00);
	orbyte &= 0xF0;
	if ((orbyte & 0x10) && gbmu->input.keypressed[275])
		orbyte |= 0x1;
	if ((orbyte & 0x10) && gbmu->input.keypressed[276])
		orbyte |= 0x2;
	write_byte(&gbmu->ram, 0xFF00, orbyte);
	//printf("%d\n", read_byte(&gbmu->ram, 0xFF00));
}
