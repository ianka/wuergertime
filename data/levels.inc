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
 *  data/levels.inc - level descriptions
 */


#ifndef LEVELS_INC
#define LEVELS_INC


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */


/* Local includes */
#include "../screens.h" /* for level_item_t */


/* Level descriptions. */
const level_item_t LevelDescriptions[] PROGMEM={
	/* Level 0 */
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|22, x:0, y:13 },
//	{ c: LEVEL_ITEM_LADDER|5, x:10, y:9 },
//	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|10, x:10, y:13 },
	{ c: LEVEL_ITEM_BURGER|4, x:10, y:20 },
	{ c: LEVEL_ITEM_BURGER|3, x:10, y:18 },
	{ c: LEVEL_ITEM_BURGER|2, x:10, y:16 },
	{ c: LEVEL_ITEM_BURGER|1, x:10, y:14 },
	{ c: LEVEL_ITEM_BURGER|0, x:10, y:12 },
	{ c: LEVEL_ITEM_BURGER|4, x:15, y:25 },
	{ c: LEVEL_ITEM_BURGER|3, x:15, y:23 },
	{ c: LEVEL_ITEM_BURGER|2, x:15, y:21 },
	{ c: LEVEL_ITEM_BURGER|1, x:15, y:19 },
	{ c: LEVEL_ITEM_BURGER|0, x:15, y:17 },
	{ c: LEVEL_ITEM_BURGER|4, x:20, y:23 },
	{ c: LEVEL_ITEM_BURGER|2, x:20, y:21 },
	{ c: LEVEL_ITEM_BURGER|0, x:20, y:19 },
	{ c: LEVEL_ITEM_BURGER|0, x:5,  y:26 },
	{ c: LEVEL_ITEM_SIGN, x:3, y:0 },
	{ c: 0 },

	/* Level 1 */
	{ c: LEVEL_ITEM_FLOOR|3, x:0, y:3 },
	{ c: LEVEL_ITEM_FLOOR|1, x:0, y:6 },
	{ c: LEVEL_ITEM_FLOOR|5, x:0, y:9 },
	{ c: 0 },
};


#endif /* SCREENLISTS_INC */
