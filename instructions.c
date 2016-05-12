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


#define op(name, code) void name(struct gbmu *gbmu, unsigned short arg) { (void) gbmu; (void) arg; code; }
/* 0x00 */
void nop(struct gbmu *gbmu, unsigned short dest) { (void) gbmu; (void) dest; }

/* 0x01 */
op(ld_bc_nn, gbmu->cpu.registers.bc = arg);

/* 0x03 */
op(inc_bc, gbmu->cpu.registers.bc++);

/* 0x05 */
op(dec_b, dec(gbmu->cpu.registers.b, &gbmu->cpu.registers));

/* 0x06 */
op(ld_b_n, gbmu->cpu.registers.b = arg);

/* 0x08 */
op(ld_nnp_sp, write_short(&gbmu->ram, arg, gbmu->cpu.registers.sp));

/* 0x09 */
op(add_hl_bc, add_short(&gbmu->cpu.registers, &gbmu->cpu.registers.hl, gbmu->cpu.registers.bc));

/* 0x0A */
op(ld_a_bcp, gbmu->cpu.registers.a = read_byte(&gbmu->ram, gbmu->cpu.registers.bc));

/* 0x0B */
op(dec_bc, gbmu->cpu.registers.bc--);

/* 0x0C */
op(inc_c, inc(gbmu->cpu.registers.c, &gbmu->cpu.registers));

/* 0x0D */
op(dec_c, dec(gbmu->cpu.registers.c, &gbmu->cpu.registers));

/* 0x20 */
op(jr_nz_n, 
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) == 0) {
		gbmu->cpu.registers.pc += (char)arg;
		//printf("going to %hx\n", gbmu->cpu.registers.pc);
	}
);

/* 0x23 */
op(inc_hl, gbmu->cpu.registers.hl++);

/* 0x28 */
op(jr_z_n, 
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) != 0) {
		gbmu->cpu.registers.pc += (char)arg;
		//printf("going to %hx\n", gbmu->cpu.registers.pc);
	}
);

/* 0x0e */
op(ld_c_n, gbmu->cpu.registers.c = arg);

/* 0x11 */
op(ld_de_nn, gbmu->cpu.registers.de = arg);

/* 0x12 */
op(ld_dep_a, write_byte(&gbmu->ram, gbmu->cpu.registers.de, gbmu->cpu.registers.a));

/* 0x13 */
op(inc_de, gbmu->cpu.registers.de++);

/* 0x16 */
op(ld_d_n, gbmu->cpu.registers.d = arg);

/* 0x18 */
op(jr_n, 
		gbmu->cpu.registers.pc += (char)arg;
		//printf("going to %hx\n", gbmu->cpu.registers.pc);
);

/* 0x19 */
op(add_hl_de, add_short(&gbmu->cpu.registers, &gbmu->cpu.registers.hl, gbmu->cpu.registers.de));

/* 0x1A */
op(ld_a_dep, gbmu->cpu.registers.a = read_byte(&gbmu->ram, gbmu->cpu.registers.de));

/* 0x1B */
op(dec_de, gbmu->cpu.registers.de--);

/* 0x1C */
op(inc_e, inc(gbmu->cpu.registers.e, &gbmu->cpu.registers));

/* 0x1E */
op(ld_e_n, gbmu->cpu.registers.e = arg);

/* 0x21 */
op(ld_hl_nn, gbmu->cpu.registers.hl = arg);

/* 0x22 */
op(ldd_hli_a, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, gbmu->cpu.registers.a); gbmu->cpu.registers.hl++);

/* 0x26 */
op(ld_h_n, gbmu->cpu.registers.h = arg);

/* 0x2a */
op(ldd_a_hli, gbmu->cpu.registers.a = read_byte(&gbmu->ram, gbmu->cpu.registers.hl); gbmu->cpu.registers.hl++);

/* 0x2C */
op(inc_l, inc(gbmu->cpu.registers.l, &gbmu->cpu.registers));

/* 0x2D */
op(dec_l, dec(gbmu->cpu.registers.l, &gbmu->cpu.registers));

/* 0x2F */
op(cpl, gbmu->cpu.registers.a = ~gbmu->cpu.registers.a; FLAGS_SET(gbmu->cpu.registers.f, FLAG_NEGATIVE | FLAG_HALF_CARRY));

/* 0x31 */
op(ld_sp_nn, gbmu->cpu.registers.sp = arg);

/* 0x32 */
op(ldd_hld_a, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, gbmu->cpu.registers.a); gbmu->cpu.registers.hl--;);

/* 0x34 */
op(inc_hlp, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, increment(&gbmu->cpu.registers, read_byte(&gbmu->ram, gbmu->cpu.registers.hl))));

/* 0x35 */
op(dec_hlp, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, decrement(&gbmu->cpu.registers, read_byte(&gbmu->ram, gbmu->cpu.registers.hl))));

/* 0x36 */
op(ld_hlp_n, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, arg));

/* 0x38 */
op(jr_c_n, 
		if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_CARRY) != 0)
			gbmu->cpu.registers.pc += (char)arg;
);

/* 0x39 */
op(add_hl_sp, add_short(&gbmu->cpu.registers, &gbmu->cpu.registers.hl, gbmu->cpu.registers.sp));

/* 0x3A */
op(ld_a_hld, gbmu->cpu.registers.a = read_byte(&gbmu->ram, gbmu->cpu.registers.hl); gbmu->cpu.registers.hl--);

/* 0x3C */
op(inc_a, inc(gbmu->cpu.registers.a, &gbmu->cpu.registers));

/* 0x3D */
op(dec_a, dec(gbmu->cpu.registers.a, &gbmu->cpu.registers));

/* 0x3E */
op(ln_a_n, gbmu->cpu.registers.a = arg);

#define ld8(va, vb) void ld_ ## va ## _ ## vb(struct gbmu *gbmu, unsigned short dest) { (void) gbmu; (void) dest; gbmu->cpu.registers.va = gbmu->cpu.registers.vb; }
/* 0x40 */
ld8(b, b);

/* 0x41 */
ld8(b, c);

/* 0x42 */
ld8(b, d);

/* 0x43 */
ld8(b, e);

/* 0x44 */
ld8(b, h);

/* 0x45 */
ld8(b, l);

/* 0x46 */
op(ld_b_hlp, gbmu->cpu.registers.b = read_byte(&gbmu->ram, gbmu->cpu.registers.hl));

/* 0x47 */
ld8(b, a);

/* 0x48 */
ld8(c, b);

/* 0x4A */
ld8(c, d);

/* 0x4B */
ld8(c, e);

/* 0x4E */
op(ld_c_hlp, gbmu->cpu.registers.c = read_byte(&gbmu->ram, gbmu->cpu.registers.hl));

/* 0x4F */
ld8(c, a);

/* 0x4C */
ld8(c, h);

/* 0x54 */
ld8(d, h);

/* 0x55 */
ld8(d, l);

/* 0x56 */
op(ld_d_hlp, gbmu->cpu.registers.d = read_byte(&gbmu->ram, gbmu->cpu.registers.hl));

/* 0x57 */
ld8(d, a);

/* 0x58 */
ld8(e, b);

/* 0x5D */
ld8(e, l);

/* 0x5E */
op(ld_e_hlp, gbmu->cpu.registers.e = read_byte(&gbmu->ram, gbmu->cpu.registers.hl));

/* 0x5F */
ld8(e, a);

/* 0x60 */
ld8(h, b);

/* 0x62 */
ld8(h, d);

/* 0x67 */
ld8(h, a);

/* 0x69 */
ld8(l, c);

/* 0x6B */
ld8(l, e);

/* 0x6C */
ld8(l, h);

/* 0x6F */
ld8(l, a);

/* 0x71 */
op(ld_hlp_c, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, gbmu->cpu.registers.c));

/* 0x72 */
op(ld_hlp_d, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, gbmu->cpu.registers.d));

/* 0x73 */
op(ld_hlp_e, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, gbmu->cpu.registers.e));

/* 0x77 */
op(ld_hlp_a, write_byte(&gbmu->ram, gbmu->cpu.registers.hl, gbmu->cpu.registers.a));

/* 0x78 */
op(ld_a_b, gbmu->cpu.registers.a = gbmu->cpu.registers.b);

/* 0x79 */
op(ld_a_c, gbmu->cpu.registers.a = gbmu->cpu.registers.c);

/* 0x7A */
ld8(a, d);

/* 0x7B */
ld8(a, e);

/* 0x7c */
op(ld_a_h, gbmu->cpu.registers.a = gbmu->cpu.registers.h);

/* 0x7d */
op(ld_a_l, gbmu->cpu.registers.a = gbmu->cpu.registers.l);

/* 0x7e */
op(ld_a_hlp, gbmu->cpu.registers.a = read_byte(&gbmu->ram, gbmu->cpu.registers.hl));

/* 0x85 */
op(add_a_l, add(&gbmu->cpu.registers, &gbmu->cpu.registers.a, gbmu->cpu.registers.l));

/* 0x87 */
op(add_a_a, add(&gbmu->cpu.registers, &gbmu->cpu.registers.a, gbmu->cpu.registers.a));

/* 0xA0 */
op(and_b, and(&gbmu->cpu.registers, gbmu->cpu.registers.b)); 

/* 0xA1 */
op(and_c, and(&gbmu->cpu.registers, gbmu->cpu.registers.c)); 

/* 0xA7 */
op(and_a, and(&gbmu->cpu.registers, gbmu->cpu.registers.a)); 

/* 0xA9 */
op(xor_c, xor(&gbmu->cpu.registers, gbmu->cpu.registers.c));

/* 0xAF */
op(xor_a, xor(&gbmu->cpu.registers, gbmu->cpu.registers.a));

/* 0xB0 */
op(or_b, or(&gbmu->cpu.registers, gbmu->cpu.registers.b));

/* 0xB1 */
op(or_c, or(&gbmu->cpu.registers, gbmu->cpu.registers.c));

/* 0xB3 */
op(or_e, or(&gbmu->cpu.registers, gbmu->cpu.registers.e));

/* 0xB6 */
op(or_hlp, or(&gbmu->cpu.registers, read_byte(&gbmu->ram, gbmu->cpu.registers.hl)));

/* 0xB7 */
op(or_a, or(&gbmu->cpu.registers, gbmu->cpu.registers.a));

/* 0xC8 */
op(ret_nz,
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) == 0) {
		gbmu->cpu.registers.pc = pop_short(&gbmu->ram, &gbmu->cpu.registers);
	}
  );

/* 0xC1 */
op(pop_bc, gbmu->cpu.registers.bc = pop_short(&gbmu->ram, &gbmu->cpu.registers));

/* 0xC2 */
op(jp_nz_nn,
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) != 0) {
		gbmu->cpu.registers.pc = arg;
	}
  );

/* 0xC3 */
op(jp_nn, //printf("going to %hx\n", arg);
		gbmu->cpu.registers.pc = arg);

/* 0xC4 */
op(call_nz_nn, if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) == 0) push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.pc); gbmu->cpu.registers.pc = arg);

/* 0xC5 */
op(push_bc, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.bc));

/* 0xC6 */
op(add_a_n, add(&gbmu->cpu.registers, &gbmu->cpu.registers.a, arg));

/* 0xC8 */
op(ret_z,
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) != 0) {
		gbmu->cpu.registers.pc = pop_short(&gbmu->ram, &gbmu->cpu.registers);
	}
  );

/* 0xC9 */
op(ret, gbmu->cpu.registers.pc = pop_short(&gbmu->ram, &gbmu->cpu.registers));

/* 0xCA */
op(jp_z_n, 
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_ZERO) != 0) {
		gbmu->cpu.registers.pc = arg;
		//printf("going to %hx\n", gbmu->cpu.registers.pc);
	}
);

/* 0xCD */
op(call_nn, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.pc); gbmu->cpu.registers.pc = arg);

/* 0xD0 */
op(ret_nc,
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_CARRY) == 0) {
		gbmu->cpu.registers.pc = pop_short(&gbmu->ram, &gbmu->cpu.registers);
	}
  );

/* 0xD1 */
op(pop_de, gbmu->cpu.registers.de = pop_short(&gbmu->ram, &gbmu->cpu.registers));

/* 0xD5 */
op(push_de, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.de));

/* 0xD8 */
op(ret_c,
	if (FLAGS_ISSET(gbmu->cpu.registers.f, FLAG_CARRY) != 0) {
		gbmu->cpu.registers.pc = pop_short(&gbmu->ram, &gbmu->cpu.registers);
	}
  );

/* 0xD9 */
op(reti, 
		ret(gbmu, arg);
		gbmu->cpu.interrupts.master = 1;
	);

/* 0xDF */
op(rst_18, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.pc); gbmu->cpu.registers.pc = 0x0018);

/* 0xE0 */
op(ldh_ff_np_a, write_byte(&gbmu->ram, 0xFF00 + arg, gbmu->cpu.registers.a));

/* 0xE1 */
op(pop_hl, gbmu->cpu.registers.hl = pop_short(&gbmu->ram, &gbmu->cpu.registers));

/* 0xE2 */
op(ld_ff_cp_a, write_byte(&gbmu->ram, 0xFF00 + gbmu->cpu.registers.c, gbmu->cpu.registers.a));

/* 0xE5 */
op(push_hl, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.hl)); 

/* 0xE6 */
op(and_n, and(&gbmu->cpu.registers, arg)); 

/* 0xE9 */
op(jp_hlp, gbmu->cpu.registers.pc = gbmu->cpu.registers.hl);

/* 0xEA */
op(ld_nnp_a, write_byte(&gbmu->ram, arg, gbmu->cpu.registers.a));

/* 0xEF */
op(rst_28, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.pc); gbmu->cpu.registers.pc = 0x0028);

/* 0xF0 */
op(ldh_a_ff_np, gbmu->cpu.registers.a = read_byte(&gbmu->ram, 0xFF00 + arg));

/* 0xF1 */
op(pop_af, gbmu->cpu.registers.af = pop_short(&gbmu->ram, &gbmu->cpu.registers));

/* 0xF3 */
op(di, gbmu->cpu.interrupts.master = 0);

/* 0xF5 */
op(push_af, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.af));

/* 0xFA */
op(ld_a_nnp, gbmu->cpu.registers.a = read_byte(&gbmu->ram, arg));

/* 0xFB */
op(ei, gbmu->cpu.interrupts.master = 1);

/* 0xFE */
op(cp_n, FLAGS_SET(gbmu->cpu.registers.f, FLAG_NEGATIVE);

		if(gbmu->cpu.registers.a == arg) FLAGS_SET(gbmu->cpu.registers.f, FLAG_ZERO);
		else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_ZERO);

		if(arg > gbmu->cpu.registers.a) FLAGS_SET(gbmu->cpu.registers.f, FLAG_CARRY);
		else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_CARRY);

		if((arg & 0x0f) > (gbmu->cpu.registers.a & 0x0f)) FLAGS_SET(gbmu->cpu.registers.f, FLAG_HALF_CARRY);
		else FLAGS_CLEAR(gbmu->cpu.registers.f, FLAG_HALF_CARRY);
);

/* 0xFF */
op(rst_38, push_short(&gbmu->ram, &gbmu->cpu.registers, gbmu->cpu.registers.pc); gbmu->cpu.registers.pc = 0x0038);

struct instruction instructions[256] = {
	{nop, 0x00, 0, "NOP"},
	{ld_bc_nn, 0x01, 2, "LD BC,nn"},
	{0, 0x02, 0, 0},
	{inc_bc, 0x03, 0, "INC BC"},
	{0, 0x04, 0, 0},
	{dec_b, 0x05, 0, "DEC B"},
	{ld_b_n, 0x06, 1, "LD B,n"},
	{0, 0x07, 0, 0},
	{ld_nnp_sp, 0x08, 2, "LD (nn),SP"},
	{add_hl_bc, 0x09, 0, "ADD HL,BC"},
	{ld_a_bcp, 0x0a, 0, "LD A,(BC)"},
	{dec_bc, 0x0b, 0, "DEC BC"},
	{inc_c, 0x0c, 0, "INC C"},
	{dec_c, 0x0d, 0, "DEC C"},
	{ld_c_n, 0x0e, 1, "LD C,n"},
	{0, 0x0f, 0, 0},
	{0, 0x10, 0, 0},
	{ld_de_nn, 0x11, 2, "LD DE,nn"},
	{ld_dep_a, 0x12, 0, "LD (DE),A"},
	{inc_de, 0x13, 0, "INC DE"},
	{0, 0x14, 0, 0},
	{0, 0x15, 0, 0},
	{ld_d_n, 0x16, 1, "LD D,n"},
	{0, 0x17, 0, 0},
	{jr_n, 0x18, 1, "JR n"},
	{add_hl_de, 0x19, 0, "ADD HL,DE"},
	{ld_a_dep, 0x1a, 0, "LD A,(DE)"},
	{dec_de, 0x1b, 0, "DEC DE"},
	{inc_e, 0x1c, 0, "INC E"},
	{0, 0x1d, 0, 0},
	{ld_e_n, 0x1e, 1, "LD E,n"},
	{0, 0x1f, 0, 0},
	{jr_nz_n, 0x20, 1, "JR NZ,n"},
	{ld_hl_nn, 0x21, 2, "LD HL,nn"},
	{ldd_hli_a, 0x22, 0, "LD (HL+),A"},
	{inc_hl, 0x23, 0, "INC HL"},
	{0, 0x24, 0, 0},
	{0, 0x25, 0, 0},
	{ld_h_n, 0x26, 1, "LD H,n"},
	{0, 0x27, 0, 0},
	{jr_z_n, 0x28, 1, "NR Z,n"},
	{0, 0x29, 0, 0},
	{ldd_a_hli, 0x2a, 0, "LDD A,(HL+)"},
	{0, 0x2b, 0, 0},
	{inc_l, 0x2c, 0, "INC L"},
	{dec_l, 0x2d, 0, "DEC L"},
	{0, 0x2e, 0, 0},
	{cpl, 0x2f, 0, "CPL"},
	{0, 0x30, 0, 0},
	{ld_sp_nn, 0x31, 2, "LD SP,nn"},
	{ldd_hld_a, 0x32, 0, "LDD (HL-),A"},
	{0, 0x33, 0, 0},
	{inc_hlp, 0x34, 0, "INC (HL)"},
	{dec_hlp, 0x35, 0, "DEC (HL)"},
	{ld_hlp_n, 0x36, 1, "LD (HL),n"},
	{0, 0x37, 0, 0},
	{jr_c_n, 0x38, 1, "JR C,n"},
	{add_hl_sp, 0x39, 0, "ADD HL,SP"},
	{ld_a_hld, 0x3a, 0, "LD A,(HL-)"},
	{0, 0x3b, 0, 0},
	{inc_a, 0x3c, 0, "INC A"},
	{dec_a, 0x3d, 0, "DEC A"},
	{ln_a_n, 0x3e, 1, "LN A,n"},
	{0, 0x3f, 0, 0},
	{ld_b_b, 0x40, 0, "LD B,B"},
	{ld_b_c, 0x41, 0, "LD B,C"},
	{ld_b_d, 0x42, 0, "LD B,D"},
	{ld_b_e, 0x43, 0, "LD B,E"},
	{ld_b_h, 0x44, 0, "LD B,H"},
	{ld_b_l, 0x45, 0, "LD B,L"},
	{ld_b_hlp, 0x46, 0, "LD B,(HL)"},
	{ld_b_a, 0x47, 0, "LD B,A"},
	{ld_c_b, 0x48, 0, "LD C,B"},
	{0, 0x49, 0, 0},
	{ld_c_d, 0x4a, 0, "LD C,D"},
	{ld_c_e, 0x4b, 0, "LD C,E"},
	{0, 0x4c, 0, 0},
	{0, 0x4d, 0, 0},
	{ld_c_hlp, 0x4e, 0, "LD C,(HL)"},
	{ld_c_a, 0x4f, 0, "LD C,A"},
	{0, 0x50, 0, 0},
	{0, 0x51, 0, 0},
	{0, 0x52, 0, 0},
	{0, 0x53, 0, 0},
	{ld_d_h, 0x54, 0, "LD D,E"},
	{ld_d_l, 0x55, 0, "LD D,L"},
	{ld_d_hlp, 0x56, 0, "LD D,(HL)"},
	{ld_d_a, 0x57, 0, "LD D,A"},
	{ld_e_b, 0x58, 0, "LD E,B"},
	{0, 0x59, 0, 0},
	{0, 0x5a, 0, 0},
	{0, 0x5b, 0, 0},
	{0, 0x5c, 0, 0},
	{ld_e_l, 0x5d, 0, "LD E,L"},
	{ld_e_hlp, 0x5e, 0, "LD E,(HL)"},
	{ld_e_a, 0x5f, 0, "LD E,A"},
	{ld_h_b, 0x60, 0, "LD H,B"},
	{0, 0x61, 0, 0},
	{ld_h_d, 0x62, 0, "LD H,D"},
	{0, 0x63, 0, 0},
	{0, 0x64, 0, 0},
	{0, 0x65, 0, 0},
	{0, 0x66, 0, 0},
	{ld_h_a, 0x67, 0, "LD H,A"},
	{0, 0x68, 0, 0},
	{ld_l_c, 0x69, 0, "LD L,C"},
	{0, 0x6a, 0, 0},
	{ld_l_e, 0x6b, 0, "LE L,E"},
	{ld_l_h, 0x6c, 0, "LD L,H"},
	{0, 0x6d, 0, 0},
	{0, 0x6e, 0, 0},
	{ld_l_a, 0x6f, 0, "LD L,A"},
	{0, 0x70, 0, 0},
	{ld_hlp_c, 0x71, 0, "LD (HL),C"},
	{ld_hlp_d, 0x72, 0, "LD (HL),D"},
	{ld_hlp_e, 0x73, 0, "LD (HL),E"},
	{0, 0x74, 0, 0},
	{0, 0x75, 0, 0},
	{0, 0x76, 0, 0},
	{ld_hlp_a, 0x77, 0, "LD (HL),A"},
	{ld_a_b, 0x78, 0, "LD A,B"},
	{ld_a_c, 0x79, 0, "LD A,C"},
	{ld_a_d, 0x7a, 0, "LD A,D"},
	{ld_a_e, 0x7b, 0, "LD A,E"},
	{ld_a_h, 0x7c, 0, "LD A,H"},
	{ld_a_l, 0x7d, 0, "LD A,L"},
	{ld_a_hlp, 0x7e, 0, "LD A,(HL)"},
	{0, 0x7f, 0, 0},
	{0, 0x80, 0, 0},
	{0, 0x81, 0, 0},
	{0, 0x82, 0, 0},
	{0, 0x83, 0, 0},
	{0, 0x84, 0, 0},
	{add_a_l, 0x85, 0, "ADD A,L"},
	{0, 0x86, 0, 0},
	{add_a_a, 0x87, 0, "ADD A,A"},
	{0, 0x88, 0, 0},
	{0, 0x89, 0, 0},
	{0, 0x8a, 0, 0},
	{0, 0x8b, 0, 0},
	{0, 0x8c, 0, 0},
	{0, 0x8d, 0, 0},
	{0, 0x8e, 0, 0},
	{0, 0x8f, 0, 0},
	{0, 0x90, 0, 0},
	{0, 0x91, 0, 0},
	{0, 0x92, 0, 0},
	{0, 0x93, 0, 0},
	{0, 0x94, 0, 0},
	{0, 0x95, 0, 0},
	{0, 0x96, 0, 0},
	{0, 0x97, 0, 0},
	{0, 0x98, 0, 0},
	{0, 0x99, 0, 0},
	{0, 0x9a, 0, 0},
	{0, 0x9b, 0, 0},
	{0, 0x9c, 0, 0},
	{0, 0x9d, 0, 0},
	{0, 0x9e, 0, 0},
	{0, 0x9f, 0, 0},
	{and_b, 0xa0, 0, "AND B"},
	{and_c, 0xa1, 0, "AND C"},
	{0, 0xa2, 0, 0},
	{0, 0xa3, 0, 0},
	{0, 0xa4, 0, 0},
	{0, 0xa5, 0, 0},
	{0, 0xa6, 0, 0},
	{and_a, 0xa7, 0, "AND A"},
	{0, 0xa8, 0, 0},
	{xor_c, 0xa9, 0, "XOR C"},
	{0, 0xaa, 0, 0},
	{0, 0xab, 0, 0},
	{0, 0xac, 0, 0},
	{0, 0xad, 0, 0},
	{0, 0xae, 0, 0},
	{xor_a, 0xaf, 0, "XOR A"},
	{or_b, 0xb0, 0, "OR B"},
	{or_c, 0xb1, 0, "OR C"},
	{0, 0xb2, 0, 0},
	{or_e, 0xb3, 0, "OR E"},
	{0, 0xb4, 0, 0},
	{0, 0xb5, 0, 0},
	{or_hlp, 0xb6, 0, "OR (HL)"},
	{or_a, 0xb7, 0, "OR A"},
	{0, 0xb8, 0, 0},
	{0, 0xb9, 0, 0},
	{0, 0xba, 0, 0},
	{0, 0xbb, 0, 0},
	{0, 0xbc, 0, 0},
	{0, 0xbd, 0, 0},
	{0, 0xbe, 0, 0},
	{0, 0xbf, 0, 0},
	{ret_nz, 0xc0, 0, "RET NZ"},
	{pop_bc, 0xc1, 0, "POP BC"},
	{jp_nz_nn, 0xc2, 2, "JP NZ,nn"},
	{jp_nn, 0xc3, 2, "JP nn"},
	{call_nz_nn, 0xc4, 2, "CALL NZ,nn"},
	{push_bc, 0xc5, 0, "PUSH BC"},
	{add_a_n, 0xc6, 1, "ADD A,N"},
	{0, 0xc7, 0, 0},
	{ret_z, 0xc8, 0, "RET Z"},
	{ret, 0xc9, 0, "RET"},
	{jp_z_n, 0xca, 2, "JP Z,nn"},
	{nop, 0xcb, 1, "CBFUCKING"},
	{0, 0xcc, 0, 0},
	{call_nn, 0xcd, 2, "CALL nn"},
	{0, 0xce, 0, 0},
	{0, 0xcf, 0, 0},
	{ret_nc, 0xd0, 0, "RET NC"},
	{pop_de, 0xd1, 0, "POP DE"},
	{0, 0xd2, 0, 0},
	{0, 0xd3, 0, 0},
	{0, 0xd4, 0, 0},
	{push_de, 0xd5, 0, "PUSH DE"},
	{0, 0xd6, 0, 0},
	{0, 0xd7, 0, 0},
	{ret_c, 0xd8, 0, "RET C"},
	{reti, 0xd9, 0, "RETI"},
	{0, 0xda, 0, 0},
	{0, 0xdb, 0, 0},
	{0, 0xdc, 0, 0},
	{0, 0xdd, 0, 0},
	{0, 0xde, 0, 0},
	{rst_18, 0xdf, 0, "RST 18"},
	{ldh_ff_np_a, 0xe0, 1, "LDH ($FF00+n),A"},
	{pop_hl, 0xe1, 0, "POP HL"},
	{ld_ff_cp_a, 0xe2, 0, "LD ($FF00+C),A"},
	{0, 0xe3, 0, 0},
	{0, 0xe4, 0, 0},
	{push_hl, 0xe5, 0, "PUSH HL"},
	{and_n, 0xe6, 1, "AND n"},
	{0, 0xe7, 0, 0},
	{0, 0xe8, 0, 0},
	{jp_hlp, 0xe9, 0, "JP (HL)"},
	{ld_nnp_a, 0xea, 2, "LD (nn),a"},
	{0, 0xeb, 0, 0},
	{0, 0xec, 0, 0},
	{0, 0xed, 0, 0},
	{0, 0xee, 0, 0},
	{rst_28, 0xef, 0, "RST 28"},
	{ldh_a_ff_np, 0xf0, 1, "LDH A,($FF00+n)"},
	{pop_af, 0xf1, 0, "POP AF"},
	{0, 0xf2, 0, 0},
	{di, 0xf3, 0, "DI"},
	{0, 0xf4, 0, 0},
	{push_af, 0xf5, 0, "PUSH AF"},
	{0, 0xf6, 0, 0},
	{0, 0xf7, 0, 0},
	{0, 0xf8, 0, 0},
	{0, 0xf9, 0, 0},
	{ld_a_nnp, 0xfa, 2, "LD A,(nn)"},
	{ei, 0xfb, 0, "EI"},
	{0, 0xfc, 0, 0},
	{0, 0xfd, 0, 0},
	{cp_n, 0xfe, 1, "CP n"},
	{rst_38, 0xff, 0, "RST 38"},
};
