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


#define LEVEL_START_ANIMATION_FLOORS_ENDED SCREEN_WIDTH
#define LEVEL_START_ANIMATION_LADDERS_ENDED ((LEVEL_START_ANIMATION_FLOORS_ENDED+SCREEN_HEIGHT))
#define LEVEL_START_ANIMATION_ENDED LEVEL_START_ANIMATION_LADDERS_ENDED

/* Prepare current level. */
void prepareLevel() {
	const level_item_t *p=LevelDescription;
	uint8_t c, x, y, length, pos;

	/* Skip if animation is done. */
	if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_ENDED+1) return;

	/* Draw level specific screen list. */
	while ((c=pgm_read_byte(&(p->c))) != 0) {
		/* Get coordinates */
		x=pgm_read_byte(&(p->x));
		y=pgm_read_byte(&(p->y));

		/* Check for type of drawable. */
		switch (c) {
			case LEVEL_ITEM_SIGN:
				/* Draw sign shape when animation is done */
				if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_ENDED)
					drawShape(x,y,ShapeSignTilesInGame);
				break;
			case LEVEL_ITEM_PLATE:
				/* Draw sign shape */
				drawPlate(x,y);
				break;
			case LEVEL_ITEM_BURGER_BUNTOP:
			case LEVEL_ITEM_BURGER_TOMATO:
			case LEVEL_ITEM_BURGER_PATTY:
			case LEVEL_ITEM_BURGER_CHEESESALAD:
			case LEVEL_ITEM_BURGER_BUNBOTTOM:
				/* Draw burger shape when animation is done. */
				if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_ENDED)
					drawBurgerComponent(x,y,c-LEVEL_ITEM_BURGER_BUNTOP+SHAPE_BURGER_BUNTOP,0);
				break;
			default:
				/* Ladders and floors. */
				if ((c & LEVEL_ITEM_LADDER) == LEVEL_ITEM_LADDER) {
					/* Ladder. Animate drawing it when floor animation is done. */
					if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_FLOORS_ENDED) {
						length=min(c & LEVEL_ITEM_LADDER_LENGTH,
							GameScreenAnimationPhase-LEVEL_START_ANIMATION_FLOORS_ENDED);
						pos=y+(c & LEVEL_ITEM_LADDER_LENGTH)-length;
						drawLadder(x,pos,length,
							(length==(c & LEVEL_ITEM_LADDER_LENGTH))?c & LEVEL_ITEM_LADDER_CONTINUED:0);
					}	
				} else {
					/* Floor. Animate width. */
					length=min(c & LEVEL_ITEM_FLOOR_LENGTH,GameScreenAnimationPhase);
					pos=x+((c & LEVEL_ITEM_FLOOR_LENGTH)-length)/2;

					/* Draw it. */
					drawFloor(pos,y,length,((pos==0)||(pos+length == SCREEN_WIDTH))?c & LEVEL_ITEM_FLOOR_CAP_BOTH:LEVEL_ITEM_FLOOR_CAP_BOTH);
				}
		}	

		/* Next element in screen list. */
		p++;
	}	
}


