#ifndef _DALLOC_IO_INTERNAL_H_
#define _DALLOC_IO_INTERNAL_H_

#include <stdint.h>

/*
Convert the integer to a string and pad out to N digits.

Return 0 on success, or 1 for invalid inputs (e.g. x conains
more than n digits).

@param x: The integer value.
@param n: The desired number of digits in the string.
@param buf: The output buffer. Must be of size > n.
*/
uint32_t pad(uint32_t x, uint16_t n, char *buf);

#endif // _DALLOC_IO_INTERNAL_H_
