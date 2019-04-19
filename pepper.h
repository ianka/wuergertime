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
 *  pepper.h - pepper functions.
 */


#ifndef PEPPER_H
#define PEPPER_H


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"


/* Pepper flags: direction and speed. */
#define PEPPER_FLAGS_DIRECTION_SHIFT 0
#define PEPPER_FLAGS_DIRECTION_MASK  ((0x07<<PEPPER_FLAGS_DIRECTION_SHIFT))
#define PEPPER_FLAGS_DIRECTION_NONE  ((0<<PEPPER_FLAGS_DIRECTION_SHIFT))
#define PEPPER_FLAGS_DIRECTION_LEFT  ((1<<PEPPER_FLAGS_DIRECTION_SHIFT))
#define PEPPER_FLAGS_DIRECTION_RIGHT ((2<<PEPPER_FLAGS_DIRECTION_SHIFT))
#define PEPPER_FLAGS_SPEED_SHIFT     3
#define PEPPER_FLAGS_SPEED_MASK      ((0x03<<PEPPER_FLAGS_SPEED_SHIFT))
#define PEPPER_FLAGS_SPEED_SLOW      ((0<<PEPPER_FLAGS_SPEED_SHIFT))
#define PEPPER_FLAGS_SPEED_NORMAL    ((1<<PEPPER_FLAGS_SPEED_SHIFT))
#define PEPPER_FLAGS_SPEED_FAST      ((2<<PEPPER_FLAGS_SPEED_SHIFT))
#define PEPPER_FLAGS_SPEED_FLASH     ((3<<PEPPER_FLAGS_SPEED_SHIFT))


typedef struct {
	uint8_t flags;
	uint8_t sprite;
} pepper_t;

extern pepper_t Pepper;


uint8_t throwPepper(void);
void movePepper(void);
void removePepper(void);


#endif /* PEPPER_H */
