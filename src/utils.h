#ifndef UTILS_H
#define UTILS_H

/*  unsigned long readline(FILE *in):
 *
 *  Input:
 *      FILE *in: Valid readable filestream.
 *      unsigned long *target: receive return value.
 *
 *  Output:
 *      int:
 *          0: When something unusual happened.
 *          1: When some input values are invalid.
 *          2: When failed to read from FILE *in.
 *      char *returnValue:
 *          0: In most usual cases.
 *          1: When something usual happens.
 */
int readline(FILE *in, unsigned long *target);

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

/*  int stringToBinaryInt(const char *instruction):
 *
 *  Input:
 *      const char *instruction: Valid, readable with a 32-bit instruction.
 *
 *  Output:
 *      long:
 *          0: When some input values are invalid or target cannot be converted.
 *          result: In most usual cases.
 */
unsigned long stringToBinaryNumber(const char *instruction);

/*  short getOpcode(long instruction):
 *
 *  Input:
 *      long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 7-bit number.
 */
short getOpcode(unsigned long instruction);

/*  short getFunct3(unsigned long instruction):
 *
 *  Input:
 *      long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 3-bit number.
 */
short getFunct3(unsigned long instruction);

/*  short getFunct7(unsigned long instruction):
 *
 *  Input:
 *      long instruction: A 32-bit binary number.
 *
 *  Output:
 *      short:
 *          result: An 7-bit number.
 */
short getFunct7(unsigned long instruction);

/* Wondering if I should return all details in this function.
 * Many helper function will be needed.
 * */
int inCompressAbleList(unsigned long instruction);

int isCompressAble(unsigned long instruction);

#endif
