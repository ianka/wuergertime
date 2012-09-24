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
 *  data/screenlists.inc - screen drawing lists
 */


#ifndef SCREENLISTS_INC
#define SCREENLISTS_INC


#include "../draw.h" /* For drawable type */


const drawable_t ScreenLists[] PROGMEM={
	/* Level 0 */
	{ c: DRAWABLE_FLOOR|1, x:0, y:3 },
	{ c: DRAWABLE_FLOOR|DRAWABLE_FLOOR_CAP_LEFT|2, x:0, y:6 },
	{ c: DRAWABLE_FLOOR|3, x:0, y:9 },
	{ c: 0 },

	/* Level 1 */
	{ c: DRAWABLE_FLOOR|3, x:0, y:3 },
	{ c: DRAWABLE_FLOOR|DRAWABLE_FLOOR_CAP_BOTH|22, x:0, y:13 },
	{ c: DRAWABLE_FLOOR|10, x:5, y:9 },
	{ c: DRAWABLE_LADDER|5, x:10, y:9 },
	{ c: DRAWABLE_LADDER|DRAWABLE_LADDER_CONTINUED|10, x:10, y:13 },
	{ c: 0 },

	/* Level 2 */
	{ c: DRAWABLE_FLOOR|3, x:0, y:3 },
	{ c: DRAWABLE_FLOOR|1, x:0, y:6 },
	{ c: DRAWABLE_FLOOR|5, x:0, y:9 },
	{ c: 0 },
};


#endif /* SCREENLISTS_INC */
