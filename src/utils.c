#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

static unsigned long stringToBinaryNumber(const char *instruction) {

	/* 1.1 Check validation of input object */
	if (instruction == NULL) { return 0; }

	/* 1.2 Perform string-to-long transformation and return */
	return strtoul(instruction, NULL, 2);
}
/* TODO: consider tabs and indents */
static int readline(FILE *in, unsigned long *returnValue) {
	char temp[33];
	/* 2.1 Check validation of input objects */
	if (in == NULL | returnValue == NULL) { return 1; }

	/* 2.2 Read in a line of original file and check if we've met the end */
	if (fscanf(in,"%s",temp)==NULL) { return 2; }
	

	/* 2.3 Get rid of '\n' */
	temp[32] = 0;

	/* 2.4 Return the converted number */
	*returnValue = stringToBinaryNumber(temp);
	return 0;
}

int writeline(FILE *out, unsigned long target, int length) {
	/* 3.1 Check validation of input objects */
	if (out == NULL) { return 1; }
	if (length != 16 && length != 32) { return 2; }

	/* 3.2 Write into the file */
	{
		int i;
		for (i = length - 1; i >= 0; --i) {
			/* 3.3 0 has ASCII code 48 */
			if (fputc((((1 << i) & target) != 0) + 48, out) == EOF) return 3;
		}
	}

	/* 3.4 Newline after successfully written */
	if (fputc('\n', out) == EOF) { return 3; }

	/* 3.5 Return 0 when nothing unusual happens */
	return 0;
}

static short getOpcode(unsigned long instruction) {
	/* 4.1 For any kind of instruction, opcode lies in the last 7 digits */
	return (short) (instruction & 0x7F);
	/*since the instructions are 32 bit long ,so 0x7F gets the least_final 7 number which the opcode located*/
}

static short getFunct3(unsigned long instruction) {
	/* 5.1 For any kind of instruction that Funct3 code exists, it lies in digits 14 ~ 12 */
	return (short) ((instruction >> 12) & 0x7);
	/*func3 occupies three location*/
}

static short getFunct7(unsigned long instruction) {
	/* 6.1 For any kind of instruction that Funct7 code exists, it lies in digits 31 ~ 25*/
	return (short) ((instruction >> 25) & 0x7F);
	/*func7 occupies 7 location*/

}
/*having examined this function all match*/
static int isInCompressAbleList(unsigned long instruction) {
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

static InsType getType(unsigned long instruction) {
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
			printf("Error getting type!");
			exit(0);
	}
}

static short getRD(unsigned long instruction) {
	/* 9.1 For any kind of instruction that rd code exists, it lies in digits 11 ~ 7 */
	return (short) ((instruction >> 7) & 0x1F);
}

static short getRS1(unsigned long instruction) {
	/* 10.1 For any kind of instruction that rd code exists, it lies in digits 19 ~ 15 */
	return (short) ((instruction >> 15) & 0x1F);
}

static short getRS2(unsigned long instruction) {
	/* 11.1 For any kind of instruction that rd code exists, it lies in digits 24 ~ 20 */
	return (short) ((instruction >> 20) & 0x1F);
}

static unsigned long getImm(unsigned long instruction) {
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
			return (((instruction >> 31) << 12) | ((instruction & 0x80) << 11) | ((instruction & 0x7E000000) >> 20) |
			        ((instruction & 0xF00) >> 7));
		case U:
			/* 12.6 Imm lies in 31 ~ 12 in a U-type instruction */
			return ((instruction >> 12) << 12);
			break;
		case UJ:
			/* 12.7 Imm lies in 31 ~ 12 in a U-type instruction */
			return (((instruction & 0x80000000) >> 11) | (instruction & 0xFF000) | ((instruction & 0x100000) >> 9) |
			        (instruction & 0x3FF00000) >> 19);
	}
}

static void parse(unsigned long instruction, Instruction *target) {
	/* 13.1 Check validation */
	if (target == NULL) return;
	/* 13.2 Original value */
	target->originalValue = instruction;
	/* 13.3 Whether it can be compressed */
	target->inCompressAbleList = isInCompressAbleList(instruction); /* Note: Not guaranteed */
	/* 13.4 Type of instruction */
	target->type = getType(instruction);
	/* 13.5 Opcode */
	target->opcode = getOpcode(instruction);
	/* 13.6 Funct7 */
	target->funct7 = getFunct7(instruction);
	/* 13.7 Funct3 */
	target->funct3 = getFunct3(instruction);
	/* 13.8 rd */
	target->rd = getRD(instruction);
	/* 13.9 rs1 */
	target->rs1 = getRS1(instruction);
	/* 13.10 rs2 */
	target->rs2 = getRS2(instruction);
	/* 13.11 imm */
	target->imm = getImm(instruction);
}

Instruction **readFromFile(FILE *in) {
	int i;
	/* 14.1 Allocate spce for pointers */
	Instruction **target = malloc(sizeof(Instruction *) * 60);
	/* 14.2 Read in all data with a single loop */
	unsigned long *num = malloc(sizeof(unsigned long));
	for (i = 0; i < 60; ++i) {
		Instruction *temp = malloc(sizeof(Instruction));
		/* 14.3 readline() has non-zero return value if something unusual happens */
		if (!readline(in, num)) {
			parse(*num, temp);
			target[i] = temp;
			continue;
		}
		/* 14.4 Finish if nothing else is read */
		free(temp);
		break;
	}

	/* 14.5 Free all space allocated */
	free(num);
	/* 14.6 Return instructions read */
	return target;
}
