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
#include "tiles.h" /* for TILES0_SPACE */


/* Maximum number of burger components per screen. */
#define SCREEN_BURGER_COMPONENT_MAX 20


/* Game screen switch, animation phase and update function pointer. */ 
uint8_t GameScreenPrevious;
uint8_t GameScreen;
uint16_t GameScreenAnimationPhase;
void (*GameScreenUpdateFunction)(void);
uint16_t GameScreenOptions;
struct {
	uint8_t component, x, stomped, half_target_y;
	int8_t  half_y;
	uint8_t background[2][5];
}	GameScreenBurgerComponents[SCREEN_BURGER_COMPONENT_MAX];


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
void prepareLevel(void) {
	const level_item_t *p=LevelDescription;
	uint8_t c, x, y, component_counter;
	uint8_t i;

	/* Reset options. */
	GameScreenOptions=LEVEL_ITEM_OPTION_STOMP_ONCE;

	/* Reset burger components. */
	for (component_counter=0;component_counter<SCREEN_BURGER_COMPONENT_MAX;component_counter++)
		GameScreenBurgerComponents[component_counter].component=LEVEL_ITEM_INVALID;

	/* Go through level specific screen list. */
	component_counter=0;
	while ((c=pgm_read_byte(&(p->c))) != 0) {
		/* Get coordinates */
		x=pgm_read_byte(&(p->x));
		y=pgm_read_byte(&(p->y));

		/* Check for type of level item. */
		switch (c) {
			case LEVEL_ITEM_OPTIONS:
				/* Use x and y values as option field. */
				GameScreenOptions=(y<<8)+x;
				break;
			case LEVEL_ITEM_BURGER_PLACEHOLDER:
				/* Skip if too much components in screen description */
				/* TODO: This should trigger a debug screen instead. */
				if (component_counter >= SCREEN_BURGER_COMPONENT_MAX) break;

				/* Remember burger component parameters. */
				GameScreenBurgerComponents[component_counter].component=c;
				GameScreenBurgerComponents[component_counter].x=x;
				GameScreenBurgerComponents[component_counter].stomped=0;
				GameScreenBurgerComponents[component_counter].half_target_y = y*2-1+GameScreenOptions;
				GameScreenBurgerComponents[component_counter].half_y = y*2-1+GameScreenOptions;

				/* Next component. */
				component_counter++;
				break;
			case LEVEL_ITEM_BURGER_BUNTOP:
			case LEVEL_ITEM_BURGER_TOMATO:
			case LEVEL_ITEM_BURGER_PATTY:
			case LEVEL_ITEM_BURGER_CHEESESALAD:
			case LEVEL_ITEM_BURGER_BUNBOTTOM:
				/* Skip if too much components in screen description */
				/* TODO: This should trigger a debug screen instead. */
				if (component_counter >= SCREEN_BURGER_COMPONENT_MAX) break;

				/* Remember burger component parameters. */
				GameScreenBurgerComponents[component_counter].component=c;
				GameScreenBurgerComponents[component_counter].x=x;
				GameScreenBurgerComponents[component_counter].stomped=0;
				GameScreenBurgerComponents[component_counter].half_target_y = y*2-1+GameScreenOptions;

				/* Set current y to a negative value for start animation. */
				GameScreenBurgerComponents[component_counter].half_y =
					-(((x & 0x03)+1)*4*((LEVEL_ITEM_BURGER_BUNBOTTOM-c)+1));

				/* Clear background buffer. */
				for (i=0;i<5;i++) {
					GameScreenBurgerComponents[component_counter].background[0][i]=TILES0_SPACE;
					GameScreenBurgerComponents[component_counter].background[1][i]=TILES0_SPACE;
				}	

				/* Next component. */
				component_counter++;
				break;
			case LEVEL_ITEM_SIGN:
				break;
			case LEVEL_ITEM_PLATE:
				break;
			default:
				/* Ladders and floors. */
				if ((c & LEVEL_ITEM_LADDER) == LEVEL_ITEM_LADDER) {
				} else {
				}	
		}	

		/* Next element in screen list. */
		p++;
	}	
}


/* Level start animation. */
void animateLevelStart(void) {
	const level_item_t *p=LevelDescription;
	uint8_t c, x, y, length, pos, component_counter;

	/* Draw level specific screen list. */
	while ((c=pgm_read_byte(&(p->c))) != 0) {
		/* Get coordinates */
		x=pgm_read_byte(&(p->x));
		y=pgm_read_byte(&(p->y));

		/* Check for type of level item. */
		switch (c) {
			case LEVEL_ITEM_OPTIONS:
				/* Do not draw anything. */
				break;
			case LEVEL_ITEM_BURGER_PLACEHOLDER:
			case LEVEL_ITEM_BURGER_BUNTOP:
			case LEVEL_ITEM_BURGER_TOMATO:
			case LEVEL_ITEM_BURGER_PATTY:
			case LEVEL_ITEM_BURGER_CHEESESALAD:
			case LEVEL_ITEM_BURGER_BUNBOTTOM:
				/* Do not draw anything. */
				break;
			case LEVEL_ITEM_SIGN:
				/* Draw sign shape when ladder animation is done */
				if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_LADDERS_ENDED) {
					/* Draw animated sign. */
					if (GameScreenAnimationPhase < LEVEL_START_ANIMATION_SIGNFRAME_ENDED) {
						/* Signframe animation. */
						drawShapeAnimated(x,y,ShapeSignLevelStart,(GameScreenAnimationPhase-LEVEL_START_ANIMATION_LADDERS_ENDED));
					}	else {
						if (GameScreenAnimationPhase < LEVEL_START_ANIMATION_SIGN_ENDED) {
							/* Blinking sign animation */
							if (blink((GameScreenAnimationPhase-LEVEL_START_ANIMATION_SIGNFRAME_ENDED)>>1,LEVEL_START_ANIMATION_SIGN_BLINKCODE))
									drawShape(x,y,ShapeSignTilesInGame);
								else
									drawShape(x,y,ShapeSignLevelStart);
						}
					}
				}	
				break;
			case LEVEL_ITEM_PLATE:
				/* Draw sign shape */
				drawPlate(x,y);
				break;
			default:
				/* Ladders and floors. */
				if ((c & LEVEL_ITEM_LADDER) == LEVEL_ITEM_LADDER) {
					/* Ladder. Animate drawing it when floor animation is done. */
					if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_FLOORS_ENDED) {
						length=min(c & LEVEL_ITEM_LADDER_LENGTH,
							(GameScreenAnimationPhase-LEVEL_START_ANIMATION_FLOORS_ENDED)>>1);
						pos=y+(c & LEVEL_ITEM_LADDER_LENGTH)-length;
						drawLadder(x,pos,length,
							(length==(c & LEVEL_ITEM_LADDER_LENGTH))?c & (LEVEL_ITEM_LADDER_CONTINUED|LEVEL_ITEM_LADDER_TOBOTTOM):c & LEVEL_ITEM_LADDER_TOBOTTOM);
					}	
				} else {
					/* Floor. Animate width. */
					if (GameScreenAnimationPhase <= LEVEL_START_ANIMATION_FLOORS_ENDED) {
						length=min(c & LEVEL_ITEM_FLOOR_LENGTH,GameScreenAnimationPhase);
						pos=x+((c & LEVEL_ITEM_FLOOR_LENGTH)-length)/2;

						/* Draw it. */
						drawFloor(pos,y,length,((pos==0)||(pos+length == SCREEN_WIDTH))?c & LEVEL_ITEM_FLOOR_CAP_BOTH:LEVEL_ITEM_FLOOR_CAP_BOTH);
					}
				}
		}	

		/* Next element in screen list. */
		p++;
	}

	/* Animate burgers when sign animation is done. */
	if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_SIGN_ENDED) {
		/* Go through all prepared burger components. */
		for (component_counter=0;component_counter<SCREEN_BURGER_COMPONENT_MAX;component_counter++) {
			/* Break at first invalid component. */
			if (GameScreenBurgerComponents[component_counter].component == LEVEL_ITEM_INVALID) break;

			/* Skip if animation is completed for this component. */
			if (GameScreenBurgerComponents[component_counter].half_y
				== GameScreenBurgerComponents[component_counter].half_target_y) continue;

			/* Animate component. Move it down. */
			GameScreenBurgerComponents[component_counter].half_y++;

			/* Restore screen at old position. */
			handleBurgerBackground(
				GameScreenBurgerComponents[component_counter].x,
				GameScreenBurgerComponents[component_counter].half_y,
				GameScreenBurgerComponents[component_counter].stomped,
				GameScreenBurgerComponents[component_counter].background);

			/* Skip if half_y position is still negative. */
			if (GameScreenBurgerComponents[component_counter].half_y<0) continue;

			/* Draw burger component at new position. */
			drawBurgerComponent(
				GameScreenBurgerComponents[component_counter].x,
				GameScreenBurgerComponents[component_counter].half_y,
				GameScreenBurgerComponents[component_counter].component
					-LEVEL_ITEM_BURGER_BUNTOP+SHAPE_BURGER_BUNTOP,
				GameScreenBurgerComponents[component_counter].stomped,
				GameScreenBurgerComponents[component_counter].background);
		}
	}
}


