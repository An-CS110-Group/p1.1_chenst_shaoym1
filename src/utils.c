#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

//typedef enum InsType { I, U, S, R, SB, UJ } InsType;

int readline(FILE *in, char *target) {
	/* 1.1 Check validation of input objects */
	if (target == NULL | in == NULL) { return 1; }

	/* 1.2 Read in a line of original file and check if we've met the end */
	if (fgets(target, 35, in) == NULL) { return 2; }

	/* 1.3 Get rid of '\n' */
	target[32] = 0;

	/* 1.4 Return 0 when nothing unusual happens */
	return 0;
}

int writeline(FILE *out, const char *target) {
	/* 2.1 Check validation of input objects */
	if (target == NULL | out == NULL) { return 1; }

	/* 2.2 Write into the file */
	if (fputs(target, out) == EOF) { return 2; }

	/* 2.3 Newline after successfully written */
	if (fputs("\n", out) == EOF) { return 2; }

	/* 2.4 Return 0 when nothing unusual happens */
	return 0;
}

unsigned long stringToBinaryInt(const char *instruction) {

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


int isCompressAble(unsigned long instruction) {
	return (int) instruction;
}
