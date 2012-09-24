/*
 *  Würgertime - An hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/janka/wuergertime
 *
 * ---------------------------------------------------
 *
 *  utils.c - utility functions and macros
 */


#include <avr/io.h> /* for uint8_t and uint32_t */


/*
 *  Random generator.
 */
#define RANDOM_MODULO 0x7FFFFFFF

uint32_t random_seed = 1234;

uint8_t fastrandom(void) {
	random_seed=
		(random_seed >> 16)
		+ ((random_seed << 15) & RANDOM_MODULO)
		- (random_seed >> 21)
		- ((random_seed << 10) & RANDOM_MODULO);
	if (random_seed < 0) random_seed += RANDOM_MODULO;
	return (uint8_t) random_seed;
}
