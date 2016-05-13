#include "gbmu.h"
#include "instructions.h"

static unsigned char increment(struct registers *registers, unsigned char value)
{
	if ((value & 0x0f) == 0x0f) {
		FLAGS_SET(registers->f, FLAG_HALF_CARRY);
	} else {
		FLAGS_CLEAR(registers->f, FLAG_HALF_CARRY);
	}

	value = value + 1;

	if (value == 0) {
		FLAGS_SET(registers->f, FLAG_ZERO);
	} else {
		FLAGS_CLEAR(registers->f, FLAG_ZERO);
	}

	FLAGS_CLEAR(registers->f, FLAG_NEGATIVE);
	return value;
}

#define inc(val, reg) (val) = increment((reg), (val))

static unsigned char decrement(struct registers *registers, unsigned char value)
{
	if ((value & 0x0f) == 0x0f) {
		FLAGS_SET(registers->f, FLAG_HALF_CARRY);
	} else {
		FLAGS_CLEAR(registers->f, FLAG_HALF_CARRY);
	}

	value = value - 1;

	if (value == 0) {
		FLAGS_SET(registers->f, FLAG_ZERO);
	} else {
		FLAGS_CLEAR(registers->f, FLAG_ZERO);
	}

	FLAGS_CLEAR(registers->f, FLAG_NEGATIVE);
	return value;
}

static void add(struct registers *reg, unsigned char *destination, unsigned char value) {
	unsigned int result = *destination + value;

	if (result & 0xff00) FLAGS_SET(reg->f, FLAG_CARRY);
	else FLAGS_CLEAR(reg->f, FLAG_CARRY);

	*destination = (unsigned char)(result & 0xff);

	if (*destination) FLAGS_CLEAR(reg->f, FLAG_ZERO);
	else FLAGS_SET(reg->f, FLAG_ZERO);

	if (((*destination & 0x0f) + (value & 0x0f)) > 0x0f) FLAGS_SET(reg->f, FLAG_HALF_CARRY);
	else FLAGS_CLEAR(reg->f, FLAG_HALF_CARRY);

	FLAGS_CLEAR(reg->f, FLAG_NEGATIVE);
}

static void add_short(struct registers *reg, unsigned short *destination, unsigned short value) {
	unsigned long result = *destination + value;

	if (result & 0xffff0000) FLAGS_SET(reg->f, FLAG_CARRY);
	else FLAGS_CLEAR(reg->f, FLAG_CARRY);

	*destination = (unsigned short)(result & 0xffff);

	if(((*destination & 0x0f) + (value & 0x0f)) > 0x0f) FLAGS_SET(reg->f, FLAG_HALF_CARRY);
	else FLAGS_CLEAR(reg->f, FLAG_HALF_CARRY);


	FLAGS_CLEAR(reg->f, FLAG_NEGATIVE);
}

#define dec(val, reg) (val) = decrement((reg), (val))

static void and(struct registers *reg, unsigned char value) {
	reg->a &= value;

	if(reg->a) FLAGS_CLEAR(reg->f, FLAG_ZERO);
	else FLAGS_SET(reg->f, FLAG_ZERO);

	FLAGS_CLEAR(reg->f, FLAG_CARRY | FLAG_NEGATIVE);
	FLAGS_SET(reg->f, FLAG_HALF_CARRY);
}


static void xor(struct registers *reg, unsigned char value)
{
	reg->a ^= value;

	if (reg->a == 0) {
		FLAGS_SET(reg->f, FLAG_ZERO);
	} else {
		FLAGS_CLEAR(reg->f, FLAG_ZERO);
	}
	FLAGS_CLEAR(reg->f, FLAG_CARRY | FLAG_HALF_CARRY | FLAG_NEGATIVE);
}

static void or(struct registers *reg, unsigned char value)
{
	reg->a |= value;

	if (reg->a == 0) {
		FLAGS_SET(reg->f, FLAG_ZERO);
	} else {
		FLAGS_CLEAR(reg->f, FLAG_ZERO);
	}
	FLAGS_CLEAR(reg->f, FLAG_CARRY | FLAG_HALF_CARRY | FLAG_NEGATIVE);
}

static void sub(struct registers *reg, unsigned char value)
{
	FLAGS_SET(reg->f, FLAG_NEGATIVE);

	if (value > reg->a) FLAGS_SET(reg->f, FLAG_CARRY);
	else FLAGS_CLEAR(reg->f, FLAG_CARRY);

	if ((value & 0xf) > (reg->a & 0xf)) FLAGS_SET(reg->f, FLAG_HALF_CARRY);
	else FLAGS_CLEAR(reg->f, FLAG_HALF_CARRY);

	reg->a -= value;

	if (reg->a == 0) FLAGS_SET(reg->f, FLAG_ZERO);
	else FLAGS_CLEAR(reg->f, FLAG_ZERO);
}


#define A gbmu->cpu.registers.a
#define B gbmu->cpu.registers.b
#define C gbmu->cpu.registers.c
#define D gbmu->cpu.registers.d
#define E gbmu->cpu.registers.e
#define H gbmu->cpu.registers.h
#define L gbmu->cpu.registers.l
#define AF gbmu->cpu.registers.af
#define BC gbmu->cpu.registers.bc
#define HL gbmu->cpu.registers.hl
#define DE gbmu->cpu.registers.de
#define PC gbmu->cpu.registers.pc
#define SP gbmu->cpu.registers.sp
#define NN arg
#define N arg

#define LD(a, b) a = b;
#define JR(dest) PC += (char)dest;
#define JP(dest) PC = dest;

#define AND(d) and(&gbmu->cpu.registers, d);
#define XOR(d) xor(&gbmu->cpu.registers, d);
#define OR(d) or(&gbmu->cpu.registers, d);
#define ADD8(dst, src) add(&gbmu->cpu.registers, &dst, src);
#define ADD(dst, src) add_short(&gbmu->cpu.registers, &dst, src);

#define INC(a) a++
#define INC8(a) a = increment(&gbmu->cpu.registers, a)
#define INCG(a) increment(&gbmu->cpu.registers, a)
#define DEC(a) a--
#define DEC8(a) a = decrement(&gbmu->cpu.registers, a)
#define DECG(a) decrement(&gbmu->cpu.registers, a)

#define SUB(a) sub(&gbmu->cpu.registers, a)
#define SBC(a) sub(&gbmu->cpu.registers, a + FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_CARRY))

#define CP(n) FLAGS_SET(gbmu->cpu.registers.f, FLAG_NEGATIVE); \
	\
	if (A == n) FLAGS_SET(gbmu->cpu.registers.f, FLAG_ZERO); \
	else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_ZERO); \
	\
	if (n > A) FLAGS_SET(gbmu->cpu.registers.f, FLAG_CARRY); \
	else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_CARRY); \
	\
	if((n & 0x0f) > (A & 0x0f)) FLAGS_SET(gbmu->cpu.registers.f, FLAG_HALF_CARRY); \
	else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_HALF_CARRY);

#define RRCA(a) unsigned char carry = a & 0x01; \
	if (carry) FLAGS_SET(gbmu->cpu.registers.f, FLAG_CARRY); \
	else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_CARRY); \
	\
	a >>= 1; \
	if (carry) a |= 0x80; \
					 \
	FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_NEGATIVE | FLAG_ZERO | FLAG_HALF_CARRY);

#define RLCA(a) unsigned char carry = (a >> 7) & 0x1; \
	if (carry) FLAGS_SET(gbmu->cpu.registers.f, FLAG_CARRY); \
	else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_CARRY); \
	\
	a <<= 1; \
	if (carry) a |= 1; \
					 \
	FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_NEGATIVE | FLAG_ZERO | FLAG_HALF_CARRY);

#define CPL(a) a = ~a; FLAGS_SET(gbmu->cpu.registers.f, FLAG_NEGATIVE | FLAG_HALF_CARRY);

#define DI gbmu->cpu.interrupts.master = 0
#define EI gbmu->cpu.interrupts.master = 1

#define PUSH_SHORT(a) push_short(&gbmu->ram, &gbmu->cpu.registers, a);
#define POP_SHORT pop_short(&gbmu->ram, &gbmu->cpu.registers);

#define CALL(n) PUSH_SHORT(PC); PC = n;
#define RET PC = POP_SHORT
#define RST(x) CALL(x)

#define IZ(code) if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) != 0) { code }
#define NZ(code) if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) == 0) { code }

#define IC(code) if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_CARRY) != 0) { code }
#define NC(code) if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_CARRY) == 0) { code }

#define READ(a) read_byte(&gbmu->ram, a)
#define WRITE(a, b) write_byte(&gbmu->ram, a, b)


#define op(name, argcount, action) void name(struct gbmu *gbmu, unsigned short arg) { (void) gbmu; (void) arg; action; }
#define noop(name, argcount, action)
#define NOP 

#include "opcodes"

#undef op
#undef noop
#define noop(name, argcount, action) { 0, 0, 0, 0},
#define op(name, argcount, action) { name, 0, argcount, #name },
struct instruction instructions[256] = {
#include "opcodes"
};

void dump_instructions(FILE *f)
{
	for (int i = 0; i <= 0xff; i++) {
		fprintf(f, "%02x: %s\n", i, instructions[i].debugname);
	}
}
