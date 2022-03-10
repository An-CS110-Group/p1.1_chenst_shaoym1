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

static Ctype checkR(const Instruction *source) {
	/* 1. 4 conditions of R-type instruction can be compressed */
	switch (source->funct3) {
		case 0x0:
			/* 2. c.sub, need to check whether registers can be compressed */
			if (source->funct7 == 0x20) {
				if (source->rd != source->rs1 || compressRegister(source->rd) == -1 ||
				    compressRegister(source->rs2) == -1)
					return NON;
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
			if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1)
				return NON;
			return XOR;
		case 0x6:
			/* 7. c.or, need to check whether registers can be compressed */
			if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1)
				return NON;
			return OR;
		case 0x7:
			/* 8. c.and, need to check whether registers can be compressed */
			if (source->rd != source->rs1 || compressRegister(source->rd) == -1 || compressRegister(source->rs2) == -1)
				return NON;
			return AND;
	}
	return NON;
}

static Ctype checkI(const Instruction *source) {
	/* 1. 3 conditions of I-type instruction can be compressed */
	switch (source->opcode) {
		case 0x67:
			if (source->rs1 == 0 || source->imm != 0) return NON;
			/* 2. JR */
			if (source->rd == 0) return JR;
			/* 3. JALR */
			if (source->rd == 1) return JALR;
			return NON;
		case 0x03:
			/* 4. TODO: */
		case 0x13:
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
			break;
		case S:
			break;
		case SB:
			break;
		case UJ:
			break;
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
