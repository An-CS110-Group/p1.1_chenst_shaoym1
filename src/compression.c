#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "utils.h"

#define INT_MIN -100000000

static int powerOfTwo(const int num) {
	/* Returns 2^(num) */
	return 1 << (num);
}

static short compressRegister(const short reg) {
	/* 1. Check whether it can be compressed */
	if ((reg >> 3) != 1) return -1;
	/* 2. Return the last 3 digits */
	return (short) (reg & 0x7);
}

static int parseNumber(const unsigned long imm) {
	/* 1. This function decides whether a 12-bit number can fit into bits */
	unsigned long a = (imm >> 11) & 1;
	if (a == 1) {
		/* 2. Two's complement for negative numbers */
		return (int) -(~(imm - 1) & 0xFFF);
	} else {
		/* 3. Positive numbers */
		return (int) imm;
	}
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
			if (compressRegister(source->rd) != -1 && compressRegister(source->rs1) != -1 && (parseNumber(source->imm) >= 0) &&
			    (parseNumber(source->imm) % 4 == 0) && (parseNumber(source->imm) <= powerOfTwo(7) - 1))
				return LW;
			return NON;
			/* Things are tough when coming to immediate values */
		case 0x13:
			switch (source->funct3) {
				case 0x0:
					/* 5. c.li */
					if (source->rd != 0 && source->rs1 == 0 && (parseNumber(source->imm) >= -1 * powerOfTwo(5)) &&
					    (parseNumber(source->imm) <= powerOfTwo(5) - 1))
						return LI;
					/* c.addi */
					else if ((source->rd == source->rs1) && (source->rd != 0x0) && (source->imm != 0x0) && (parseNumber(source->imm) >= -1 * powerOfTwo(5)) &&
					         (parseNumber(source->imm) <= powerOfTwo(5) - 1))
						return ADDI;
					return NON;
				case 0x1: /*6. c.slli */
					if (((source->rd == source->rs1) && (source->rd != 0x0))) { return SLLI; }
					return NON;
				case 0x5:
					if (source->funct7 == 0x0) {
						/* c.srli */
						if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd)) { return SRLI; }
						return NON;
					} else if (source->funct7 == 0x20) {
						/* c.srai */
						if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd)) { return SRAI; }
						return NON;
					} else break;
				case 0x7:
					/* c.andi */
					if ((compressRegister(source->rd) != -1) && (source->rs1 == source->rd) && (parseNumber(source->imm) >= -1 * powerOfTwo(5)) &&
					    (parseNumber(source->imm) <= powerOfTwo(5) - 1)) {
						return ANDI;
					}
					return NON;
			}
	}
	/* Basically return NON to avoid returning nothing */
	return NON;
}

static Ctype checkU(const Instruction *source) {
	/* The function check if the source can be compressed into LUI type */
	if ((source->rd != 0x0 && source->rd != 0x2) && (source->imm != 0x0) && (parseNumber(source->imm >> 12) >= -1 * powerOfTwo(5)) &&
	    (parseNumber(source->imm >> 12) <= powerOfTwo(5) - 1)) {
		return LUI;
	} else {
		/* Return NON by default */
		return NON;
	}
}

static Ctype checkSB(const Instruction *source) {
	switch (source->funct3) {
		case 0x0: /*beq*/
			if ((source->rs2 == 0x0 && compressRegister(source->rs1) != -1) && (parseNumber(source->imm) % 2 == 0) &&
			    (parseNumber(source->imm) >= -1 * powerOfTwo(8)) && (parseNumber(source->imm) <= powerOfTwo(8) - 1)) {
				return BEQZ;
			} else {
				/* Return NON by default */
				return NON;
			}

		case 0x1: /*bne*/
			if ((source->rs2 == 0x0 && compressRegister(source->rs1) != -1) && (parseNumber(source->imm) % 2 == 0) &&
			    (parseNumber(source->imm) >= -1 * powerOfTwo(8)) && (parseNumber(source->imm) <= powerOfTwo(8) - 1)) {
				return BNEZ;
			} else {
				/* Return NON by default */
				return NON;
			}
	}
	/* Return NON by default */
	return NON;
}

static Ctype checkS(const Instruction *source) {
	/* The function check if the source can be compressed into SW type */
	if (compressRegister(source->rs1) != -1 && (parseNumber(source->imm) >= 0) && (parseNumber(source->imm) % 4 == 0) &&
	    (parseNumber(source->imm) <= powerOfTwo(7) - 1) && compressRegister(source->rs1) != -1 && compressRegister(source->rs2) != -1) {
		return SW;
	} else {
		/* Return NON by default */
		return NON;
	}
}

static Ctype checkUJ(const Instruction *source) {
	/* The function check if the source can be compressed into J / JAL type */
	if (source->rd == 0x0 && (parseNumber(source->imm) >= -1 * powerOfTwo(11)) &&
	    (parseNumber(source->imm) <= powerOfTwo(11) - 1)) {
		return J;
		/* JAL */
	} else if (source->rd == 0x1 && (parseNumber(source->imm) >= -1 * powerOfTwo(11)) &&
	           (parseNumber(source->imm) <= powerOfTwo(11) - 1)) {
		return JAL;
	}
	/* Return NON by default */
	return NON;
}

static Ctype assertCType(const Instruction *source) {
	/* 1. Check validation */
	if (source == NULL) return NON;
	/* 2. Cannot be compressed */
	if (source->inCompressAbleList == 0) return NON;
	/* 3. Switch cases */
	switch (source->type) {
		case R:
			return checkR(source);
		case I: /* We should design a function for every type of instruction */
			return checkI(source);
		case U:
			return checkU(source);
		case S: /* We should design a function for every type of instruction */
			return checkS(source);
		case SB:
			return checkSB(source);
		case UJ: /* We should design a function for every type of instruction */
			return checkUJ(source);
	}
	/* Return NON by default */
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
		case ADD:  /* 1001 */
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
		case SW:   /* 110 */
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
		case OR:  /* 100011 */
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
		case OR:   /* 10 */
		case ANDI: /* 10 */
			return 2;
		case XOR:  /* 01 */
		case SRAI: /* 01 */
			return 1;
		case SUB:  /* 00 */
		case SRLI: /* 00 */
			return 0;
		default: /* Some instructions don't have a Funct6 code */
			return -1;
	}
}

/* Return INT_MIN by default */
static int assertImm(const Instruction *source) {
	switch (assertCType(source)) {
		case LI: /* This case has sign-extended number */
		case ADDI: /* This case has sign-extended number */
		case SLLI: /* This case has sign-extended number */
		case SRLI: /* This case has sign-extended number */
		case SRAI: /* This case has sign-extended number */
		case ANDI: /* This case has sign-extended number */
			return parseNumber(source->imm);
		case LW:
		case SW:
			return parseNumber(source->imm >> 2);
		case LUI: /* This case has sign-extended number */
			return parseNumber(source->imm >> 12);
		case BEQZ: /* This case has sign-extended number */
		case BNEZ: /* This case has sign-extended number */
		case J: /* This case has sign-extended number */
		case JAL: /* This case has sign-extended number */
			return parseNumber(source->imm >> 1);
		default: /* Return INT_MIN on default */
			return INT_MIN;
	}
}

/* Return -1 by default */
static short assertRd(const Instruction *source) {
	switch (assertCType(source)) {
		case ADD: /* Return the original value */
		case MV: /* Return the original value */
		case JR: /* Return the original value */
		case JALR: /* Return the original value */
		case LI: /* Return the original value */
		case LUI: /* Return the original value */
		case ADDI: /* Return the original value */
		case SLLI: /* Return the original value */
			return source->rd;
		case LW: /* Return the compressed value */
		case AND: /* Return the compressed value */
		case OR: /* Return the compressed value */
		case XOR: /* Return the compressed value */
		case SUB: /* Return the compressed value */
		case SRLI: /* Return the compressed value */
		case SRAI: /* Return the compressed value */
		case ANDI: /* Return the compressed value */
			return compressRegister(source->rd);
		default: /* Return -1 by default */
			return -1;
	}
}

/* Return -1 by default */
static short assertRs1(const Instruction *source) {
	switch (assertCType(source)) {
		case LW: /* Return the compressed value */
		case SW: /* Return the compressed value */
		case BNEZ: /* Return the compressed value */
		case BEQZ: /* Return the compressed value */
			return compressRegister(source->rs1);
		case JR: /* Return the original value */
		case JALR: /* Return the original value */
			return source->rs1;
		default: /* Return -1 by default */
			return -1;
	}
}

/* Return -1 by default */
static short assertRs2(const Instruction *source) {
	switch (assertCType(source)) {
		case ADD: /* Return the original value */
		case MV: /* Return the original value */
			return source->rs2;
		case LW: /* Return the compressed value */
		case SW: /* Return the compressed value */
		case AND: /* Return the compressed value */
		case OR: /* Return the compressed value */
		case XOR: /* Return the compressed value */
		case SUB: /* Return the compressed value */
			return compressRegister(source->rs2);
		default: /* Return -1 by default */
			return -1;
	}
}

Compressed **primaryCompression(const Instruction **source) {
	Compressed **target;
	int i; /* Auxiliary vars */
	/* 1. Check validation */
	if (source == NULL) { return NULL; }
	/* 2. Allocate space for pointers */
	target = malloc(sizeof(Compressed *) * 60);
	for (i = 0; i < 60; ++i) { target[i] = NULL; }
	{
		/* 3. Loop through all instructions */
		for (i = 0; i < 60; ++i) {
			/* 4. The end of all instructions */
			if (source[i] == NULL) break;
			/* 5. Impossible to compress */
			if (!source[i]->inCompressAbleList) continue;
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
			/* 14. imm field */
			target[i]->imm = assertImm(source[i]);
			/* 15. rd, aka rd/rs1 */
			target[i]->rd = assertRd(source[i]);
			/* 16. rs1 */
			target[i]->rs1 = assertRs1(source[i]);
			/* 17. rs2 */
			target[i]->rs2 = assertRs2(source[i]);
		}
	}
	/* 18. Return object */
	return target;
}

static int addressNeedsUpdate(const Instruction *source) { return (source->type == SB || source->type == UJ); }

static void updateSBType(Instruction *toUpdate) {
	/* 1. Duplicate original imm */
	unsigned long duplicate = toUpdate->originalValue;
	/* 2. Clean the imm field */
	duplicate &= ~0xFE000F80;
	/* 3. Add the new imm */
	duplicate |= ((toUpdate->imm & (1 << 12)) >> 12 << 31);
	duplicate |= ((toUpdate->imm & 0x7E0) >> 5 << 25);
	duplicate |= ((toUpdate->imm & 0x1E) >> 1 << 8);
	duplicate |= ((toUpdate->imm & (1 << 11)) >> 11 << 7);
	/* 4. Correct the new value */
	toUpdate->originalValue = duplicate;
}

static void updateUJType(Instruction *toUpdate) {
	/* 1. Duplicate original imm */
	unsigned long duplicate = toUpdate->originalValue;
	/* 2. Clean the imm field */
	duplicate &= ~0xFFFFF000;
	/* 3. Add the new imm */
	duplicate |= ((toUpdate->imm & (1 << 20)) >> 20 << 31);
	duplicate |= ((toUpdate->imm & 0x7FE) >> 1 << 21);
	duplicate |= ((toUpdate->imm & (1 << 11)) >> 11 << 20);
	duplicate |= ((toUpdate->imm & 0xFF000) >> 12 << 12);
	/* 4. Correct the new value */
	toUpdate->originalValue = duplicate;
}

static int parseNumber20(const unsigned long imm) {
	/* 1. This function decides whether a 20-bit number can fit into bits */
	unsigned long a = (imm >> 19) & 1;
	if (a == 1) {
		/* 2. Two's complement for negative numbers */
		return (int) -(~(imm - 1) & 0xFFFFF);
	} else {
		/* 3. Positive numbers */
		return (int) imm;
	}
}

void confirmAddress(Instruction **origin, Compressed **compressed) {
	/* 1. Original value */
	int i, imm = 0;
	for (i = 0; i < 60; ++i) {
		int new = 0;
		if (origin[i] == NULL) break;
		/* 2. Some instructions don't need to be updated */
		if (!addressNeedsUpdate(origin[i])) continue;
		/* 3. Get the jump offset */
		if (origin[i]->type == SB) {
			imm = parseNumber(origin[i]->imm >> 1) * 2;
		} else if (origin[i]->type == UJ) {
			imm = parseNumber20(origin[i]->imm);
		}
		/* 4. Calculate new offsets */
		if (imm > 0) {
			int j = i;
			while (imm != 0) { /* When we need to count toward the end of file */
				imm -= 4;
				++j;
				new += compressed[j - 1] == NULL ? 4 : 2; /* New address according to how many instructions are compressed */
			}
		} else if (imm < 0) { /* When we need to count toward the head of file */
			int j = i;
			while (imm != 0) {
				imm += 4;
				--j;
				new -= compressed[j] == NULL ? 4 : 2; /* New address according to how many instructions are compressed */
			}
		}
		/* 5. Set the new offsets */
		if (origin[i]->type == SB) {
			if (compressed[i] == NULL) {
				origin[i]->imm = new & 0x1FFF;
				updateSBType(origin[i]); /* Call SB-Type instruction */
			} else {
				compressed[i]->imm = new;
			}

		} else if (origin[i]->type == UJ) { /* Actually should be "other cases" here */
			if (compressed[i] == NULL) {
				origin[i]->imm = new & 0x1FFFFF;
				updateUJType(origin[i]); /* Call UJ-Type instruction */
			} else {
				compressed[i]->imm = new;
			}
		}
	}
}
