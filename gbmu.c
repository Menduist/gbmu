#include "gbmu.h"
#include "cart.h"
#include <stdio.h>
#include <string.h>

#include <signal.h>

struct gbmu *ggbmu;

void print(struct gbmu *gbmu, FILE *f);
void printtile(struct gbmu *gbmu, FILE *f, int tileid);
void sig_handler(int signo)
{
	(void) signo;
	signal(SIGINT, SIG_DFL);
	fprintf(stderr, "\n");
	print(ggbmu, stderr);
}

int init_gbmu(struct gbmu *gbmu, char *path)
{
	signal(SIGINT, sig_handler);
	memset(gbmu, 0, sizeof(struct gbmu));
	init_ram(&gbmu->ram);
	read_cart(&gbmu->cart, path);
	load_cart(&gbmu->ram, &gbmu->cart);

	init_gpu(gbmu);

	gbmu->cpu.registers.af = 0x01B0;
	gbmu->cpu.registers.bc = 0x0013;
	gbmu->cpu.registers.de = 0x00D8;
	gbmu->cpu.registers.hl = 0x014D;
	gbmu->cpu.registers.pc = 0x0100;
	gbmu->cpu.registers.sp = 0xFFFE;
	return (0);
}

int main(int argc, char **argv)
{
	struct gbmu gbmu;

	ggbmu = &gbmu;
	if (argc != 2) {
		printf("usage: %s [rom]\n", argv[0]);
		return (1);
	}
	init_gbmu(&gbmu, argv[1]);
	while (cpu_step(&gbmu) == 0) {
		gpu_step(&gbmu);	
	}
}
