#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "utils.h"

Compressed **primaryCompression(const Instruction **source);

void confirmAddress(Instruction **origin, Compressed **compressed);

#endif
