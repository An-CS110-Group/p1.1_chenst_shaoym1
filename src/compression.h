#ifndef COMPRESSION_H
#define COMPRESSION_H

/* All kinds of compressed instruction */
typedef enum Ctype {
	NON = 0, ADD = 1, MV, JR, JALR, LI, LUI, ADDI, SLLI, LW, SW, AND, OR, XOR, SUB, BEQZ, BNEZ, SRLI, SRAI, ANDI, J, JAL
} Ctype;

typedef struct Compressed {
	/* The type of compressed instruction */
	Ctype type;
	/* 1 ~ 0 bit of compressed instruction */
	short opcode;
	/* 15 ~ 12 bit of compressed instruction */
	short funct4;
	/* 15 ~ 13 bit of compressed instruction */
	short funct3;
	/* 15 ~ 10 bit of compressed instruction */
	short funct6;
	/* 11 ~ 10 bit of compressed instruction */
	short funct2;
	/* Immediate value in the instruction (if exists) */
	int imm;
	/* rd, aka rd/rs1 */
	short rd;
	/* rs1 */
	short rs1;
	/* rs2 */
	short rs2;
} Compressed;

short compressRegister(short reg);

#endif
