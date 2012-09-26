/*
 *  Würgertime - An Hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/janka/wuergertime
 *
 * ---------------------------------------------------
 *
 *  data/levels.inc - level drawing lists
 */


#ifndef LEVELS_INC
#define LEVELS_INC


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */


/* Local includes */
#include "../screens.h" /* for level_item_t */


/* Screen lists. */
const level_item_t Levels[] PROGMEM={
	/* Level 0 */
	{ c: LEVEL_ITEM_FLOOR|1, x:0, y:3 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_LEFT|2, x:0, y:6 },
	{ c: LEVEL_ITEM_FLOOR|3, x:0, y:9 },
	{ c: 0 },

	/* Level 1 */
	{ c: LEVEL_ITEM_SIGN, x:3, y:0 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|22, x:0, y:13 },
	{ c: LEVEL_ITEM_FLOOR|10, x:5, y:9 },
	{ c: LEVEL_ITEM_LADDER|5, x:10, y:9 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|10, x:10, y:13 },
	{ c: 0 },

	/* Level 2 */
	{ c: LEVEL_ITEM_FLOOR|3, x:0, y:3 },
	{ c: LEVEL_ITEM_FLOOR|1, x:0, y:6 },
	{ c: LEVEL_ITEM_FLOOR|5, x:0, y:9 },
	{ c: 0 },
};



#endif /* SCREENLISTS_INC */
