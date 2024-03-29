#ifndef GBMU_H
#define GBMU_H

#include "cart.h"
#include "ram.h"
#include "cpu.h"
#include "gpu.h"
#include "input.h"

struct gbmu {
	struct ram ram;
	struct cart cart;
	struct cpu cpu;
	struct gpu gpu;
	struct input input;

	int show_instructions;
};

#endif
