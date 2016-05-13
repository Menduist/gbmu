#include "gbmu.h"
#include "cpu.h"

static void handle_interrupts(struct gbmu *gbmu)
{
	unsigned char IF;

	IF = read_byte(&gbmu->ram, 0xFFFF);
	if (FLAGS_ISSET(gbmu->cpu.interrupts.incoming_interrupts, INT_VBLANK)) {

		FLAGS_CLEAR(gbmu->cpu.interrupts.incoming_interrupts, INT_VBLANK);
		if (gbmu->cpu.interrupts.master && FLAGS_ISSET(IF, INT_VBLANK)) {
			gbmu->cpu.interrupts.master = 0;
			push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.pc);
			gbmu->cpu.registers.pc = 0x40;
			return;
		}
	}
}

int cpu_step(struct gbmu *gbmu)
{
	unsigned char opcode;
	unsigned short arg;
	int pc;
	struct instruction *instruction;

	handle_interrupts(gbmu);

	pc = gbmu->cpu.registers.pc;
	opcode = read_byte(&gbmu->ram, pc);
	instruction = &instructions[opcode];
	if (instruction->func == 0) {
		fprintf(stderr, "%04x: WARNING: unknown opcode %02x\n", pc, opcode);
		return (1);
	}

	if (pc == 0x2A18) {
		printf("lol %x\n", read_byte(&gbmu->ram, 0xFF00));
	}

	gbmu->cpu.registers.pc += 1 + instruction->paramcount;
	
	if (instruction->paramcount == 0)
		arg = 0;
	else if (instruction->paramcount == 1)
		arg = (unsigned char)read_byte(&gbmu->ram, pc + 1);
	else if (instruction->paramcount == 2)
		arg = (unsigned short)read_short(&gbmu->ram, pc + 1);
	else {
		fprintf(stderr, "%04x: unknown paramcount %d for opcode %02x\n", pc, instruction->paramcount, opcode);
		return (1);
	}
	instruction->func(gbmu, arg);
	if (gbmu->show_instructions) {
		printf("%04x: executed %s (%02x) with 0x%x\n", pc, instruction->debugname, opcode, arg);
		if (gbmu->show_instructions > 0) gbmu->show_instructions--;
	}
	return (0);
}
