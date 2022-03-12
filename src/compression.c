#include <limits.h>
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
				case 0x1: /*6. c.slli */
					if (((source->rd == source->rs1) && (source->rd != 0x0)) && (source->imm == 0x0)) { return SLLI; }
					return NON;
				case 0x5:
					if (source->funct7 == 0x0) {
						/* c.srli */
						if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd) && (source->imm == 0)) { return SRLI; }
						return NON;
					} else if (source->funct7 == 0x20) {
						/* c.srai */
						if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd) && (source->imm == 0)) { return SRAI; }
						return NON;
					}
				case 0x7:
					/* c.andi */
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

		case 0x1: /*bne*/
			if ((source->rs2 == 0x0 && compressRegister(source->rs1) != -1)) {
				return BNEZ;
			} else {
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

static Ctype checkUJ(const Instruction *source) {
	if (source->rd == 0x0) {
		return J;
	} else if (source->rd == 0x1) {
		return JAL;
	}
	return NON;
}


Ctype assertCType(const Instruction *source) {
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

/* Return -1 by default */
static short assertOpcode(const Instruction *source) {
	switch (assertCType(source)) {
		case ADD:  /* CR-format */
		case MV:   /* CR-format */
		case JR:   /* CR-format */
		case JALR: /* CR-format */
		case SLLI: /* CI-format */
			return 2;
		case LI:   /* CI-format */
		case LUI:  /* CI-format */
		case ADDI: /* CI-format */
		case AND:  /* CS-format */
		case OR:   /* CS-format */
		case XOR:  /* CS-format */
		case SUB:  /* CS-format */
		case BEQZ: /* CB-format */
		case BNEZ: /* CB-format */
		case SRLI: /* CB-format */
		case SRAI: /* CB-format */
		case ANDI: /* CB-format */
		case J:    /* CJ-format */
		case JAL:  /* CJ-format */
			return 1;
		case LW: /* CL-format */
		case SW: /* CS-format */
			return 0;
		default:
			/* NON, such case should not exist! */
			printf("Cannot assert opcode!");
			return -1;
	}
}

/* Return -1 by default */
static short assertFunct4(const Instruction *source) {
	/* Only CR-format compressed instruction has Funct4 code */
	switch (assertCType(source)) {
		case ADD: /* 1001 */
		case JALR: /* 1001 */
			return 9;
		case MV: /* 1000 */
		case JR: /* 1000 */
			return 8;
		default: /* Other type of compressed instruction */
			return -1;
	}
}

/* Return -1 by default */
static short assertFunct3(const Instruction *source) {
	switch (assertCType(source)) {
		case LI: /* 010 */
		case LW: /* 010 */
			return 2;
		case LUI: /* 011 */
			return 3;
		case ADDI: /* 000 */
		case SLLI: /* 000 */
			return 0;
		case SW: /* 110 */
		case BEQZ: /* 110 */
			return 6;
		case BNEZ: /* 111 */
			return 7;
		case SRLI: /* 100 */
		case SRAI: /* 100 */
		case ANDI: /* 100 */
			return 4;
		case JAL: /* 001 */
			return 1;
		case J: /* 101 */
			return 5;
		default: /* Some instructions don't have a Funct3 code */
			return -1;
	}
}

/* Return -1 by default */
static short assertFunct6(const Instruction *source) {
	switch (assertCType(source)) {
		case AND: /* 100011 */
		case OR: /* 100011 */
		case XOR: /* 100011 */
		case SUB: /* 100011 */
			return 35;
		default: /* Some instructions don't have a Funct6 code */
			return -1;
	}
}

/* Return -1 by default */
static short assertFunct2(const Instruction *source) {
	switch (assertCType(source)) {
		case AND: /* 11 */
			return 3;
		case OR: /* 10 */
		case ANDI: /* 10 */
			return 2;
		case XOR: /* 01 */
		case SRAI: /* 01 */
			return 1;
		case SUB: /* 00 */
		case SRLI: /* 00 */
			return 0;
		default: /* Some instructions don't have a Funct6 code */
			return -1;
	}
}

Compressed **primaryCompression(const Instruction **source) {
	Compressed **target;
	/* 1. Check validation */
	if (source == NULL) { return NULL; }
	/* 2. Allocate space for pointers */
	target = malloc(sizeof(Compressed *) * 60);
	{
		int i;
		/* 3. Loop through all instructions */
		for (i = 0; i < 60; ++i) {
			/* 4. The end of all instructions */
			if (source[i] == NULL) break;
			/* 5. Impossible to compress */
			if (source[i]->inCompressAbleList) continue;
			/* 6. Can compress or not */
			if (assertCType(source[i]) == NON) continue;
			/* 7. Allocate space for compressed instruction */
			target[i] = malloc(sizeof(Compressed));
			/* 8. CType */
			target[i]->type = assertCType(source[i]);
			/* 9. opcode(compressed version) */
			target[i]->opcode = assertOpcode(source[i]);
			/* 10. Funct4 code */
			target[i]->funct4 = assertFunct4(source[i]);
			/* 11. Funct3 code */
			target[i]->funct3 = assertFunct3(source[i]);
			/* 12. Funct6 code */
			target[i]->funct6 = assertFunct6(source[i]);
			/* 13. Funct2 code */
			target[i]->funct2 = assertFunct2(source[i]);
			/* TODO: imm rd rs1 rs2 */
		}
	}


	return target;
}
