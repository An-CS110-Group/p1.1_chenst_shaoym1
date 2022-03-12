#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "utils.h"

short compressRegister(const short reg) {
	/* 1. Check whether it can be compressed */
	if ((reg >> 3) == 1) return -1;
	/* 2. Return the last 3 digits */
	return (short) (reg & 0x8);
}

int parseNumber(unsigned long imm, int bits) {
	/* 1. This function decides whether a 12-bit number can fit into bits */
	unsigned long a = (imm >> 11) & 1;
	if (a == 1) {
		/* 2. Two's complement for negative numbers */
		if ((~(imm - 1) & 0xFFF) >> bits == 0) { return (int) -(~(imm - 1) & 0xFFF); }
		/* 3. Positive numbers */
	} else {
		if (imm >> bits == 0) { return (int) imm; }
	}
	/* 4. Other cases */
	return INT_MIN;
}

static Ctype checkR(const Instruction *source) {
	/* 1. 4 conditions of R-type instruction can be compressed */
	switch (source->funct3) {
		case 0x0:
			/* 2. c.sub, need to check whether registers can be compressed */
			if (source->funct7 == 0x20) {
				if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1) return NON;
				return SUB;
			} else {
				/* 3. c.add */
				if (source->rs1 == source->rd && source->rd != 0x0 && source->rs2 != 0x0) {
					return ADD;
				}
				/* 4. c.mv */
				else if (source->rs1 == 0x0 && source->rd != 0x0 && source->rs2 != 0x0) {
					return MV;
				}
				/* 5. Cannot be compressed */
				return NON;
			}
		case 0x4:
			/* 6. c.xor, need to check whether registers can be compressed */
			if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1) return NON;
			return XOR;
		case 0x6:
			/* 7. c.or, need to check whether registers can be compressed */
			if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1) return NON;
			return OR;
		case 0x7:
			/* 8. c.and, need to check whether registers can be compressed */
			if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1) return NON;
			return AND;
	}
	return NON;
}


static Ctype checkI(const Instruction *source) {
	/* 1. 3 conditions of I-type instruction can be compressed */
	switch (source->opcode) {
		case 0x67:
			if (source->rs1 == 0 || source->imm != 0) return NON;
			/* 2. c.jr */
			if (source->rd == 0) return JR;
			/* 3. c.jalr */
			if (source->rd == 1) return JALR;
			return NON;
		case 0x03:
			/* 4. c.lw */
			if (compressRegister(source->rd) != -1 && compressRegister(source->rs1) != -1) return LW;
			return NON;
		case 0x13:
			switch (source->funct3) {
				case 0x0:
					/* 5. c.Li */
					if (source->rd != 0 && source->rs1 == 0) return LI;
					else if ((source->rd == source->rs1) && (source->rd != 0x0) && (source->imm != 0x0))
						return ADDI;
					return NON;
				case 0x1: /*6.c.slli */
					if (((source->rd == source->rs1) && (source->rd != 0x0)) && (source->imm == 0x0)) { return SLLI; }
					return NON;


				case 0x5:
					if (source->funct7 == 0x0) {
						if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd) && (source->imm == 0)) { return SRLI; }
						return NON;
					} else if (source->funct7 == 0x20) {
						if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd) && (source->imm == 0)) { return SRAI; }
						return NON;
					}

				case 0x7:
					if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd)) { return ANDI; }
					return NON;
			}
	}
	return NON;
}

static Ctype checkU(const Instruction *source) {
	if ((source->rd != 0x0 && source->rd != 0x2) && (source->imm != 0x0)) {
		return LUI;
	} else {
		return NON;
	}
}

static Ctype checkSB(const Instruction *source) {
	switch (source->funct3) {
		case 0x0: /*beq*/
			if ((source->rs2 == 0x0 && compressRegister(source->rs1) != -1)) {
				return BEQZ;
			} else {
				return NON;
			}

		case 0x1:/*bne*/
			if ((source->rs2==  0x0 && compressRegister(source->rs1) != -1))
				{
					return BNEZ;
				}
				else{
					return NON;
				}

	}
	return NON;
}

static Ctype checkS(const Instruction *source) {
	if (compressRegister(source->rs1) != -1) {
		return SW;
	} else {
		return NON;
	}
}

static Ctype checkUJ(const Instruction *source)
{
	if(source->rd==0x0)
	{
		return J;
	}
	else if(source->rd==0x1)
	{
		return JAL;
	}
	return NON;
}


Ctype getCType(const Instruction *source) {
	/* 1. Check validation */
	if (source == NULL) return NON;
	/* 2. Cannot be compressed */
	if (source->inCompressAbleList == 0) return NON;
	/* 3. Switch cases */
	switch (source->type) {
		case R:
			return checkR(source);
		case I:
			return checkI(source);
		case U:
			return checkU(source);
		case S:
			return checkS(source);
		case SB:
			return checkSB(source);
		case UJ:
			return checkUJ(source);
	}
	return NON;
}

Compressed **primaryCompression(const Instruction **source) {
	Compressed **target;
	/* 1. Check validation */
	if (source == NULL) { return NULL; }
	/* 2. Allocate spce for pointers */
	target = malloc(sizeof(Compressed *) * 60);
	/* 3. Loop through all instructions */
	{
		int i;
		for (i = 0; i < 60; ++i) {
			if (source[i] == NULL) break;

			target[i] = malloc(sizeof(Compressed));
		}
	}


	return target;
}


/*before we get certain Ctype and stored it in the Struct Compressed(which contains paticular compressed binary and related opcode ,type ...section),
we should first check whether "soure" is valid .since the instruction sets' constranit problem are left behind,we need to compensate for it .we see that
R,SB,S,CJ cant cause ambigous,so just neglect their constrants line ,but notice they have other limitations*/
