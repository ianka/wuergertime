/*
 *  Würgertime - An hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/ianka/wuergertime
 *
 * ---------------------------------------------------
 *
 *  utils.h - utility functions and macros
 */


#ifndef UTILS_H
#define UTILS_H


#include <uzebox.h> /* for vram array */
#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h>


/* Common types. */
typedef	struct { uint8_t x, y; } position_t;


/* Max and min macros. */
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


/* Random generator. */
uint8_t fastrandom(void);


/* Initialize RAM from program memory. */
void meminit(void *dst, const void *src, uint8_t count);

/* Blink a code. */
static inline uint8_t blink(uint8_t phase, uint32_t phases) {
	return (phases>>phase) & 0x01;
}


#endif /* UTILS_H */
