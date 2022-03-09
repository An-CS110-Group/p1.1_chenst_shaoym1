#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "utils.h"

Compressed **primaryCompression(const Instruction **source) {
	Compressed **target;
	/* 1. Check validation */
	if (source == NULL) { return NULL; }
	/* 2. Allocate spce for pointers */
	target = malloc(sizeof(Compressed *) * 60);
	return target;
}
