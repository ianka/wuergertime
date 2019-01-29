/*
 *  Würgertime - An Hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/ianka/wuergertime
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
const uint8_t LevelDrawings[] PROGMEM={
	/* End of level drawings */
	0
};

const level_item_t LevelComponents[] PROGMEM={
	LEVEL_COMPONENT_END,
};


#endif /* SCREENLISTS_INC */
