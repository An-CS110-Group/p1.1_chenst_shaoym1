#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


int readline(FILE *in, unsigned long *returnValue) {
	char temp[33];

	/* 1.1 Check validation of input objects */
	if (in == NULL | returnValue == NULL) { return 1; }

	/* 1.2 Read in a line of original file and check if we've met the end */
	if (fgets(temp, 35, in) == NULL) { return 2; }

	/* 1.3 Get rid of '\n' */
	temp[32] = 0;

	/* 1.4 Return the converted number */
	*returnValue = stringToBinaryNumber(temp);
	return 0;
}

int writeline(FILE *out, unsigned long target, int length) {
	/* 2.1 Check validation of input objects */
	if (out == NULL) { return 1; }
	if (length != 16 && length != 32) { return 2; }

	/* 2.2 Write into the file */
	{
		int i;
		for (i = length - 1; i >= 0; --i) {
			/* 2.3 0 has ASCII code 48 */
			if (fputc((((1 << i) & target) != 0) + 48, out) == EOF) return 3;
		}
	}

	/* 2.4 Newline after successfully written */
	if (fputc('\n', out) == EOF) { return 3; }

	/* 2.5 Return 0 when nothing unusual happens */
	return 0;
}

unsigned long stringToBinaryNumber(const char *instruction) {

	/* 3.1 Check validation of input object */
	if (instruction == NULL) { return 0; }

	/* 3.2 Perform string-to-long transformation and return */
	return strtoul(instruction, NULL, 2);
}

short getOpcode(unsigned long instruction) {
	/* 4.1 For any kind of instruction, opcode lies in the last 7 digits */
	return (short) (instruction & 0x7F);
}

short getFunct3(unsigned long instruction) {
	/* 5.1 For any kind of instruction that Funct3 code exists, it lies in digits 14 ~ 12 */
	return (short) ((instruction >> 12) & 0x7);
}

short getFunct7(unsigned long instruction) {
	/* 6.1 For any kind of instruction that Funct7 code exists, it lies in digits 31 ~ 25*/
	return (short) ((instruction >> 25) & 0x7F);
}

int inCompressAbleList(unsigned long instruction) {
	switch (getOpcode(instruction)) {
			/* 7.1 I-type */
		case 0x67:
			/* 7.2 jalr */
			return 1;
		case 0x03:
			if (getFunct3(instruction) == 0x2) {
				/* 7.3 lw */
				return 1;
			}
			break;
		case 0x13:
			switch (getFunct3(instruction)) {
				case 0x0:
					/* 7.4 addi */
				case 0x1:
					/* 7.5 slli */
				case 0x5:
					/* 7.6 srli srai */
				case 0x7:
					/* 7.7 andi */
					return 1;
				default:
					return 0;
			}
			/* 7.8 U-type */
		case 0x37:
			/* 7.9 auipc */
			return 1;
			/* 7.10 S-type */
		case 0x23:
			if (getFunct3(instruction) == 0x2) {
				/* 7.11 sw */
				return 1;
			}
			break;
			/* 7.12 R-type */
		case 0x33:
			switch (getFunct3(instruction)) {
				case 0x0:
					/* 7.13 add sub */
				case 0x4:
					/* 7.14 xor */
				case 0x6:
					/* 7.15 or */
				case 0x7:
					/* 7.16 and */
					return 1;
				default:
					return 0;
			}
			/* 7.17 SB-type */
		case 0x63:
			switch (getFunct3(instruction)) {
				case 0x0:
					/* 7.18 beq */
				case 0x1:
					/* 7.19 bne */
					return 1;
				default:
					return 0;
			}
			/* 7.20 UJ-type */
		case 0x6F:
			/* 7.21 jal */
			return 1;
			/* 7.22 Not possible */
		default:
			return 0;
	}
	/* 7.23 By default */
	return 0;
}

InsType getType(unsigned long instruction) {
	/* 8.1 Get the opcode of the instruction, a certain opcode can decide the type of instruction*/
	switch (getOpcode(instruction)) {
			/* 8.2 I-type */
		case 0x67:
		case 0x73:
		case 0x03:
		case 0x13:
		case 0x1B:
			return I;
			/* 8.3 U-type */
		case 0x17:
		case 0x37:
			return U;
			/* 8.4 S-type */
		case 0x23:
			return S;
			/* 8.5 R-type */
		case 0x33:
		case 0x3B:
			return R;
			/* 8.6 SB-type */
		case 0x63:
			return SB;
			/* 8.7 UJ-type */
		case 0x6F:
			return UJ;
			/* 8.8 No such case */
		default:
			exit(0);
	}
}

short getRD(unsigned long instruction) {
	/* 9.1 For any kind of instruction that rd code exists, it lies in digits 11 ~ 7 */
	return (short) ((instruction >> 7) & 0x1F);
}

short getRS1(unsigned long instruction) {
	/* 10.1 For any kind of instruction that rd code exists, it lies in digits 19 ~ 15 */
	return (short) ((instruction >> 15) & 0x1F);
}

short getRS2(unsigned long instruction) {
	/* 11.1 For any kind of instruction that rd code exists, it lies in digits 24 ~ 20 */
	return (short) ((instruction >> 20) & 0x1F);
}

unsigned long getImm(unsigned long instruction) {
	/* 12.1 Get the type of instruction */
	switch (getType(instruction)) {
		case R:
			/* 12.2 No imm to parse */
			return 0;
		case I:
			/* 12.3 Imm lies in 31 ~ 20 in an I-type instruction */
			return (instruction >> 20);
		case S:
			/* 12.4 Imm lies in 31 ~ 25 and 11 ~ 7 in an S-type instruction */
			return (((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F));
		case SB:
			/* 12.5 Imm lies in 31 ~ 25 and 11 ~ 7 in an SB-type instruction */
			return (((instruction >> 31) << 12) | ((instruction & 0x80) << 11) | ((instruction & 0x7E000000) >> 20) | ((instruction & 0xF00) >> 7));
		case U:
			/* 12.6 Imm lies in 31 ~ 12 in a U-type instruction */
			return ((instruction >> 12) << 12);
			break;
		case UJ:
			/* 12.7 Imm lies in 31 ~ 12 in a U-type instruction */
			return (((instruction & 0x80000000) >> 11) | (instruction & 0xFF000) | ((instruction & 0x100000) >> 9) | (instruction & 0x3FF00000) >> 19);
	}
}

Instruction *parse(unsigned long instruction) {
	/* 13.1 Create a new struct */
	Instruction *i = (Instruction *) malloc(sizeof(Instruction));
	/* 13.2 Original value */
	i->originalValue = instruction;
	/* 13.3 Whether it can be compressed*/
	i->isCompressAble = inCompressAbleList(instruction); /* TODO: Not guaranteed */
	/* 13.4 Type of instruction */
	i->type = getType(instruction);
	/* 13.5 Opcode */
	i->opcode = getOpcode(instruction);
	/* 13.6 Funct7 */
	i->funct7 = getFunct7(instruction);
	/* 13.7 Funct3*/
	i->funct3 = getFunct3(instruction);
	/* 13.8 rd */
	i->rd = getRD(instruction);
	/* 13.9 rs1 */
	i->rs1 = getRS1(instruction);
	/* 13.10 rs2 */
	i->rs2 = getRS2(instruction);
	/* 13.11 imm */
	i->imm = getImm(instruction);
	/* 13.12 Return instruction */
	return i;
}
