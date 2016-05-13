#ifndef INPUT_H
#define INPUT_H

struct input {
	char keypressed[323];
};

struct gbmu;
void update_inputs(struct gbmu *gbmu);

#endif
