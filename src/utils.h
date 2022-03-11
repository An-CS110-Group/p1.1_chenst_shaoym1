#ifndef UTILS_H
#define UTILS_H


/* All kinds of instruction */
typedef enum InsType { I = 1, U, S, R, SB, UJ } InsType;

typedef struct Instruction {
	/* Original value of instruction */
	unsigned long originalValue;
	/* Whether the instruction can be compressed */
	int inCompressAbleList;
	/* The type of instruction */
	InsType type;
	/* opcode */
	short opcode;
	/* 31 ~ 25 bit of instruction */
	short funct7;
	/* 14 ~ 12 bit of instruction */
	short funct3;
	/* 11 ~ 7 bit of instruction */
	short rd;
	/* 19 ~ 15 bit of instruction */
	short rs1;
	/* 24 ~ 20 bit of instruction */
	short rs2;
	/* Immediate value in the instruction (if exists) */
	unsigned long imm;
} Instruction;

/*  int readline(FILE *in, unsigned long *target):
 *
 *  Input:
 *      FILE *in: Valid readable filestream.
 *      unsigned long *target: receive return value.
 *
 *  Output:
 *      int:
 *          0: When nothing unusual happened.
 *          1: When some input values are invalid.
 *          2: When failed to read from FILE *in.
 *      unsigned long *target
 *          0: When something unusual happens.
 *          result: In most usual cases.
 */

/*  int writeline(FILE *out, unsigned long target, int length):
 *
 *  Input:
 *      FILE *out: Valid writable filestream.
 *      unsigned long target: An instruction that is ready for writing.
 *      int length: 16 / 32, depending on whether the instruction is compressed.
 *
 *  Output:
 *      int:
 *          0: In most usual cases.
 *          1: When some input values are invalid.
 *          2: Invalid value of length, the value is only possible to be 16 / 32.
 *          3: The function fputc() has encountered unprecedented failure.
 */
int writeline(FILE *out, unsigned long target, int length);

/*  unsigned long stringToBinaryNumber(const char *instruction):
 *
 *  Input:
 *      const char *instruction: Valid, readable with a 32-bit instruction.
 *
 *  Output:
 *      long:
 *          0: When some input values are invalid or target cannot be converted.
 *          result: In most usual cases.
 */

/*  short getOpcode(long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 7-bit number.
 */

/*  short getFunct3(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 3-bit number.
 */

/*  short getFunct7(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 7-bit number.
 */

/*  int inCompressAbleList(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      int:
 *          0: When the instruction cannot be compressed.
 *          1: When the instruction can be compressed.
 */

/*  InsType getType(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      InsType:
 *          result: The type of instruction.
 */

/*  short getRD(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 5-bit number.
 */

/*  short getRS1(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 5-bit number.
 */

/*  short getRS2(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 5-bit number.
 */

/*  unsigned long getImm(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      unsigned long:
 *          result: Imm (if exists) in the instruction.
 */

/*  void parse(unsigned long instruction):
 *
 *  Input:
 *      unsigned long instruction: A 32-bit binary number.
 *
 *  Output:
 *      Instruction* target:
 *          result: Detailed information about the instruction.
 */

/*  Instruction **readFromFile(FILE *in):
 *
 *  Input:
 *      FILE *in: Valid readable filestream.
 *
 *  Output:
 *      Instruction **:
 *          result: An array of original instructions with length 60.
 */
Instruction **readFromFile(FILE *in);

#endif


/*to write a genalized summary of the pro





*/