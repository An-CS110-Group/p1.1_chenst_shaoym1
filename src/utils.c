<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* Your code here... */

 
=======
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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
	/* 1.1 Check validation of input objects */
	if (target == NULL | out == NULL) { return 1; }

	/* 1.2 Write into the file */
	if (fputs(target, out) == EOF) { return 2; }

	/* 1.3 Newline after successfully written */
	if (fputs("\n", out) == EOF) { return 2; }

	/* 1.4 Return 0 when nothing unusual happens */
	return 0;
}
>>>>>>> bc4bf53266e0a67fe1386d9a5d53f73fd811f33c
