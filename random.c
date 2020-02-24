#include "random.h"

void rand_byts(uint32_t size, unsigned char *p)
{
	randombytes(p, size);
}