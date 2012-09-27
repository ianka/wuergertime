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
 *  utils.h - utility functions and macros
 */


#ifndef UTILS_H
#define UTILS_H


#include <uzebox.h> /* for vram array */
#include <avr/io.h> /* for uint8_t */


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


/* Get tile address in vram. */
static inline uint8_t* GetTileAddress (uint8_t x, uint8_t y) {
	return &vram[32*y+x];
}


#endif /* UTILS_H */
