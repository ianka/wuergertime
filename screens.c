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
#define SCREEN_BURGER_MAX 4
#define SCREEN_BURGER_COMPONENT_MAX 5
#define SCREEN_BURGER_INVALID 0xff

/* Game screen switch, animation phase and update function pointer. */ 
uint8_t GameScreenPrevious;
uint8_t GameScreen;
uint16_t GameScreenAnimationPhase;
void (*GameScreenUpdateFunction)(void);
uint16_t GameScreenOptions;
typedef struct {
	uint8_t type, stomped, half_place_y, half_target_y;
	int8_t  half_y;
	uint8_t background[2][5];
} burger_component_t;
struct {
	uint8_t x;
	burger_component_t component[SCREEN_BURGER_COMPONENT_MAX];
} GameScreenBurger[SCREEN_BURGER_MAX];


/* Lavel number and pointer to current level drawing. */
uint8_t Level;
const level_item_t *LevelDrawing;


/* Select a level. */
void selectLevel(uint8_t level) {
	const level_item_t *p=LevelDrawings;

	/* Remember level number. */
	Level=level;

	/* Skip level drawings to given level */
	while (level--) {
		while (pgm_read_byte(&(p->c))) p++;
		p++;
	}

	/* Remember level drawing pointer. */
	LevelDrawing=p;
}


/* Prepare current level. */
void prepareLevel(void) {
	const level_item_t *p=LevelDrawing;
	uint8_t c, x, y, burger, component;
	uint8_t i;

	/* Reset options. */
	GameScreenOptions=LEVEL_ITEM_OPTION_STOMP_ONCE;

	/* Reset burger components. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		GameScreenBurger[burger].x=SCREEN_BURGER_INVALID;
		for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
			GameScreenBurger[burger].component[component].type=LEVEL_ITEM_INVALID;
	}

	/* Go through level specific screen list. */
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
			case LEVEL_ITEM_BURGER_BUNTOP:
			case LEVEL_ITEM_BURGER_TOMATO:
			case LEVEL_ITEM_BURGER_PATTY:
			case LEVEL_ITEM_BURGER_CHEESESALAD:
			case LEVEL_ITEM_BURGER_BUNBOTTOM:
				/* Get the number of the burger with that x coordinate. */
				for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
					if (GameScreenBurger[burger].x == x) break;
				}

				/* Check if there was a burger. */
				if (burger==SCREEN_BURGER_MAX) {
					/* No. Get free burger slot. */
					for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
						if (GameScreenBurger[burger].x == SCREEN_BURGER_INVALID) {
							/* Allocate burger slot. */
							GameScreenBurger[burger].x=x;
							break;
						}	
					}
				}

				/* Check if we have a burger slot now. */
				if (burger!=SCREEN_BURGER_MAX) {
					/* Yes. Get free component slot. */
					for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
						if (GameScreenBurger[burger].component[component].type == LEVEL_ITEM_INVALID) break;

					/* Check if we have a component slot now. */
					if (component!=SCREEN_BURGER_COMPONENT_MAX) {
						/* Yes.	Remember burger component parameters. */
						GameScreenBurger[burger].component[component].type=c;
						GameScreenBurger[burger].component[component].stomped=0;
						GameScreenBurger[burger].component[component].half_place_y = y*2+GameScreenOptions;
						GameScreenBurger[burger].component[component].half_y = GameScreenBurger[burger].component[component].half_place_y;
						GameScreenBurger[burger].component[component].half_target_y = GameScreenBurger[burger].component[component].half_place_y;

						/* If not placeholder, do some more initialisations. */
						if (c != LEVEL_ITEM_BURGER_PLACEHOLDER) {
							/* Clear background buffer. */
							for (i=0;i<5;i++) {
								GameScreenBurger[burger].component[component].background[0][i]=TILES0_SPACE;
								GameScreenBurger[burger].component[component].background[1][i]=TILES0_SPACE;
							}	

							/* Set current y to a negative value for start animation. */
							GameScreenBurger[burger].component[component].half_y =
								-(((x & 0x03)+1)*4*((LEVEL_ITEM_BURGER_BUNBOTTOM-c)+1));
						}
					}
				}
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


/* Burger animation. */
void animateBurgers(void) {
	uint8_t burger, component;
	burger_component_t *p;

	/* Go through all prepared burger components. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++)
		for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++) {
			p=&(GameScreenBurger[burger].component[component]);

			/* Break at first invalid component. */
			if (p->type == LEVEL_ITEM_INVALID) break;

			/* Skip if animation is completed for this component. */
			if (p->half_y == p->half_target_y) continue;

			/* Animate component. Move it down. */
			p->half_y++;

			/* Skip if half_y position is still negative. */
			if (p->half_y<-1) continue;

			/* Restore screen at old position. */
			handleBurgerBackground(
				GameScreenBurger[burger].x,
				p->half_y,
				p->stomped,
				p->background);

			/* Draw burger component at new position. */
			drawBurgerComponent(
				GameScreenBurger[burger].x,
				p->half_y,
				p->type-LEVEL_ITEM_BURGER_BUNTOP+SHAPE_BURGER_BUNTOP,
				p->stomped,
				p->background);
		}
}


/* Level start animation. */
void animateLevelStart(void) {
	const level_item_t *p=LevelDrawing;
	uint8_t c, x, y, length, pos;

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
									drawShape(x,y,ShapeSignInGame);
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
						length=(GameScreenAnimationPhase-LEVEL_START_ANIMATION_FLOORS_ENDED)>>1;
						pos=y+(c & LEVEL_ITEM_LADDER_LENGTH)-length;
						if ((length <= (c & LEVEL_ITEM_LADDER_LENGTH)) && ((GameScreenAnimationPhase-LEVEL_START_ANIMATION_FLOORS_ENDED) & 0x01))
							drawLadder(x,pos,length,c & (LEVEL_ITEM_LADDER_CONTINUED|LEVEL_ITEM_LADDER_UPONLY));
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
	if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_SIGN_ENDED)
		animateBurgers();
}


/* Stomp onto a tile. */
void stomp(uint8_t x, uint8_t y) {
	uint8_t burger_x, burger, component;
	burger_component_t *p, *q;

	/* Check which burger. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		burger_x=GameScreenBurger[burger].x;
		if ((x >= burger_x) && (x < burger_x+5)) {
			/* Burger selected. Check if on a component. */
			for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
				if ((GameScreenBurger[burger].component[component].half_y>>1) == y) {
					/* Component found. */ 
					p=&(GameScreenBurger[burger].component[component]);

					/* Stomp tile.*/
					p->stomped|=1<<(x-burger_x);

					/* Check if completely stomped. */
					if (p->stomped == 0x1f) {
						/* Yes. Reset and increase half_y instead. */
						p->stomped=0;
						p->half_y++;

						/* No falling animation by default. */
						p->half_target_y = p->half_y;

						/* Check if the component should fall now. */
						if (!(p->half_y & 0x01)) {
							/* Full tile. Check tile above leftmost tile. */
							if (getTile(burger_x,(p->half_y>>1)-1) == TILES0_FLOOR_MIDDLE) {
								/* Floor is just above this burger component. Fall! */
								if (component>0) {
									/* There could be a component below. Check. */
									q=&(GameScreenBurger[burger].component[component-1]);
										p->half_target_y=q->half_place_y;
									if (q->type != LEVEL_ITEM_INVALID) {
										/* There is a component below. Fall to its place. */
									} {
										/* No component below. Fall to bottom of screen. */
//										p->half_target_y=SCREEN_HEIGHT*2-2;
									}
								}	
							}
						}
					}

					/* Restore screen at old position. */
					handleBurgerBackgroundTile(x-burger_x,
						GameScreenBurger[burger].x,
						p->half_y,
						p->stomped,
						p->background);

					/* Draw burger component at new position. */
					drawBurgerComponent(
						GameScreenBurger[burger].x,
						p->half_y,
						p->type-LEVEL_ITEM_BURGER_BUNTOP+SHAPE_BURGER_BUNTOP,
						p->stomped,
						p->background);
				}
		}
	}
}
