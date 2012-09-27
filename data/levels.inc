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
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_LEFT|30, x:0, y:20 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|30, x:0, y:16 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_RIGHT|30, x:0, y:12 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|9, x:0, y:7 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|23, x:7, y:8 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|9, x:0, y:3 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|9, x:21, y:4 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|12, x:7, y:5 },
	{ c: LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_CAP_BOTH|5, x:19, y:5 },

	{ c: LEVEL_ITEM_LADDER|4, x:0, y:3 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|5, x:0, y:7 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:0, y:12 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:0, y:16 },

	{ c: LEVEL_ITEM_LADDER|2, x:7, y:3 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|2, x:7, y:5 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|1, x:7, y:7 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:7, y:8 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:7, y:12 },

	{ c: LEVEL_ITEM_LADDER|4, x:14, y:8 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:14, y:12 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:14, y:16 },

	{ c: LEVEL_ITEM_LADDER|4, x:21, y:5 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:21, y:8 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:21, y:12 },

	{ c: LEVEL_ITEM_LADDER|4, x:28, y:4 },
	{ c: LEVEL_ITEM_LADDER|4, x:28, y:12 },
	{ c: LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_CONTINUED|4, x:28, y:16 },

	{ c: LEVEL_ITEM_SIGN, x:9, y:0 },

	{ c: LEVEL_ITEM_BURGER_BUNBOTTOM, x:2, y:20*2-1 },
	{ c: LEVEL_ITEM_BURGER_CHEESESALAD, x:2, y:16*2-1 },
	{ c: LEVEL_ITEM_BURGER_PATTY, x:2, y:12*2-1 },
	{ c: LEVEL_ITEM_BURGER_TOMATO, x:2, y:7*2-1 },
	{ c: LEVEL_ITEM_BURGER_BUNTOP, x:2, y:4*2-1 },

	{ c: LEVEL_ITEM_BURGER_BUNBOTTOM, x:9, y:20*2 },
	{ c: LEVEL_ITEM_BURGER_CHEESESALAD, x:9, y:16*2 },
	{ c: LEVEL_ITEM_BURGER_PATTY, x:9, y:12*2 },
	{ c: LEVEL_ITEM_BURGER_BUNTOP, x:9, y:8*2 },

	{ c: LEVEL_ITEM_BURGER_BUNBOTTOM, x:16, y:20*2+1 },
	{ c: LEVEL_ITEM_BURGER_PATTY, x:16, y:16*2+1 },
	{ c: LEVEL_ITEM_BURGER_TOMATO, x:16, y:12*2+1 },
	{ c: LEVEL_ITEM_BURGER_BUNTOP, x:16, y:8*2+1 },

	{ c: LEVEL_ITEM_BURGER_BUNBOTTOM, x:23, y:26*2 },
	{ c: LEVEL_ITEM_BURGER_CHEESESALAD, x:23, y:25*2 },
	{ c: LEVEL_ITEM_BURGER_PATTY, x:23, y:24*2 },
	{ c: LEVEL_ITEM_BURGER_TOMATO, x:23, y:23*2 },
	{ c: LEVEL_ITEM_BURGER_BUNTOP, x:23, y:22*2 },

	{ c: LEVEL_ITEM_PLATE, x:2, y:27 },
	{ c: LEVEL_ITEM_PLATE, x:9, y:27 },
	{ c: LEVEL_ITEM_PLATE, x:16, y:27 },
	{ c: LEVEL_ITEM_PLATE, x:23, y:27 },
	{ c: 0 },

	/* Level 1 */
	{ c: LEVEL_ITEM_FLOOR|3, x:0, y:3 },
	{ c: LEVEL_ITEM_FLOOR|1, x:0, y:6 },
	{ c: LEVEL_ITEM_FLOOR|5, x:0, y:9 },
	{ c: 0 },
};


#endif /* SCREENLISTS_INC */
