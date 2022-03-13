#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "utils.h"

/* 1. Compress but not change address */
Compressed **primaryCompression(const Instruction **source);

/* 2. Change addresses */
void confirmAddress(Instruction **origin, Compressed **compressed);

#endif
