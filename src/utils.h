<<<<<<< HEAD
#ifndef UTILS_H
#define UTILS_H

/* Your code here... */

#endif
=======
#ifndef UTILS_H
#define UTILS_H

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

#endif
>>>>>>> bc4bf53266e0a67fe1386d9a5d53f73fd811f33c
