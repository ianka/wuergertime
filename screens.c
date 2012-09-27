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
 *  screens.c - screen definitions
 */


#include <avr/io.h> /* for uint8_t */


/* Actual level data */
#include "data/levels.inc"


/* Local Includes */
#include "draw.h"
#include "utils.h"


/* Game screen switch, animation phase and update function pointer. */ 
uint8_t GameScreenPrevious;
uint8_t GameScreen;
uint16_t GameScreenAnimationPhase;
void (*GameScreenUpdateFunction)(void);


/* Pointer to current level description. */
const level_item_t *LevelDescription;


/* Select a level description. */
void selectLevel(uint8_t level) {
	const level_item_t *p=LevelDescriptions;

	/* Skip level descriptions to given level */
	while (level--) {
		while (pgm_read_byte(&(p->c))) p++;
		p++;
	}

	/* Remember level description pointer. */
	LevelDescription=p;
}


/* Prepare current level. */
void prepareLevel(uint8_t length_tweak) {
	const level_item_t *p=LevelDescription;
	uint8_t c, x, y, length;

	/* Draw level specific screen list. */
	while ((c=pgm_read_byte(&(p->c))) != 0) {
		/* Get coordinates */
		x=pgm_read_byte(&(p->x));
		y=pgm_read_byte(&(p->y));

		/* Check for type of drawable. */
		switch (c) {
			case LEVEL_ITEM_SIGN:
				/* Draw sign shape */
				drawShape(x,y,ShapeSignTilesInGame);
				break;
			case LEVEL_ITEM_BURGER:
			case LEVEL_ITEM_BURGER+1:
			case LEVEL_ITEM_BURGER+2:
			case LEVEL_ITEM_BURGER+3:
			case LEVEL_ITEM_BURGER+4:
				/* Draw burger shape */
				drawBurgerComponent(x,y,c&7,0x1f);
				break;
			default:
				/* Ladders and floors. */
				if ((c & LEVEL_ITEM_LADDER) == LEVEL_ITEM_LADDER) {
					/* Ladder. Draw it. */
					drawLadder(x,y,c & LEVEL_ITEM_LADDER_LENGTH,c & LEVEL_ITEM_LADDER_CONTINUED);
				} else {
					/* Floor. Honor the length tweak, e.g. for the level start animation. */
					length=min(c & LEVEL_ITEM_FLOOR_LENGTH,length_tweak);

					/* Draw it. */
					drawFloor(x+((c & LEVEL_ITEM_FLOOR_LENGTH)-length)/2,y,length,c & LEVEL_ITEM_FLOOR_CAP_BOTH);
				}
		}	

		/* Next element in screen list. */
		p++;
	}	
}


