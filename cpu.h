#ifndef CPU_H
#define CPU_H

#include "instructions.h"

#define FLAG_CARRY (1 << 4)
#define FLAG_HALF_CARRY (1 << 5)
#define FLAG_NEGATIVE (1 << 6)
#define FLAG_ZERO (1 << 7)

#define INT_VBLANK (1 << 0)

#define FLAGS_ISSET(regf, x) (regf & (x))
#define FLAGS_SET(regf, x) (regf |= (x))
#define FLAGS_CLEAR(regf, x) (regf &= ~(x))

struct registers {
	struct {
		union {
			struct {
				unsigned char f;
				unsigned char a;
			};
			unsigned short af;
		};
	};

	struct {
		union {
			struct {
				unsigned char c;
				unsigned char b;
			};
			unsigned short bc;
		};
	};

	struct {
		union {
			struct {
				unsigned char e;
				unsigned char d;
			};
			unsigned short de;
		};
	};

	struct {
		union {
			struct {
				unsigned char l;
				unsigned char h;
			};
			unsigned short hl;
		};
	};

	unsigned short sp;
	unsigned short pc;
};

struct interrupts {
	unsigned char master;
	unsigned char incoming_interrupts;
};

struct cpu {
	struct registers registers;
	struct interrupts interrupts;
};

struct gbmu;

int cpu_step(struct gbmu *gbmu);

#endif
