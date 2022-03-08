#ifndef UTILS_H
#define UTILS_H

/* TODO: It may be better if I return an int indicating the whole line.
 * Call stringToBinaryNumber.
 * */

/*  int readline(FILE *in, char *target):
 *
 *  Input:
 *      FILE *in: Valid readable filestream.
 *      char *target: Valid, writable, with at least 33 bytes of space.
 *
 *  Output:
 *      int:
 *          0: In most usual cases.
 *          1: When some input values are invalid.
 *          2: The file has come to an end.
 */
int readline(FILE *in, char *target);

/* TODO: We shall write from unsigned long instead of char*.
 * There should be different cases (16 / 32).
 * */

/*  int writeline(FILE *out, const char *target):
 *
 *  Input:
 *      FILE *out: Valid writable filestream.
 *      char *target: Valid, readable.
 *
 *  Output:
 *      int:
 *          0: In most usual cases.
 *          1: When some input values are invalid.
 *          2: The function fputs() has encountered unprecedented failure.
 */
int writeline(FILE *out, const char *target);

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
