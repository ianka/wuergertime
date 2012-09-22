/*
 * Würgertime - hommage to Burger Time
 *
 * Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 * licensed under the GNU GPL v3 or later
 *
 * see https://github.com/janka/wuergertime
 *
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/tiles.inc"



const uint8_t myburger[]=TILES_BURGER(TILES0_BURGER_PATTY_CHEESESALAD);

int main(void) {
	return myburger[0];
}
