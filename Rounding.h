#ifndef ROUNDING_H
#define ROUNDING_H

#include "Parameters.h"

void Power2Round(uint32_t *high, uint32_t *low, const uint32_t value);

uint32_t Decompose(uint32_t *r0_prime, const uint32_t r);
uint32_t HighBits(uint32_t r);

uint32_t Make_hint(uint32_t a, uint32_t b); 
uint32_t Use_hint(uint32_t h, uint32_t r);


#endif