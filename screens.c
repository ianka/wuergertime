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
#include "player.h"
#include "tiles.h" /* for TILES0_SPACE */


/* Maximum number of burger components per screen. */
#define SCREEN_BURGER_MAX 4
#define SCREEN_BURGER_COMPONENT_MAX 5
#define SCREEN_BURGER_PLACE_MAX ((SCREEN_BURGER_COMPONENT_MAX+1))
#define SCREEN_BURGER_INVALID 0xff
#define SCREEN_BURGER_OCCUPIED_HAT_SHIFT 4
#define SCREEN_BURGER_OCCUPIED_MASK   0x0f
#define SCREEN_BURGER_PLACE_INVALID   0xee
#define SCREEN_BURGER_PLACE_FREE_BODY 0x0f
#define SCREEN_BURGER_PLACE_FREE_HAT  0xf0
#define SCREEN_BURGER_PLACE_FREE      ((SCREEN_BURGER_PLACE_FREE_BODY|SCREEN_BURGER_PLACE_FREE_HAT))


/* Game screen switch, animation phase and update function pointer. */ 
uint8_t GameScreenPrevious;
uint8_t GameScreen;
uint16_t GameScreenAnimationPhase;
void (*GameScreenUpdateFunction)(void);
uint16_t GameScreenOptions;

typedef struct {
	int8_t  half_y;
	uint8_t occupied_by;
} burger_component_place_t;
typedef struct {
	uint8_t type, stomped, half_target_y;
	int8_t  half_y;
	uint8_t background[2][5];
} burger_component_t;
struct {
	uint8_t x;
	burger_component_place_t place[SCREEN_BURGER_PLACE_MAX];
	burger_component_t component[SCREEN_BURGER_COMPONENT_MAX];
} GameScreenBurger[SCREEN_BURGER_MAX];



/* Lavel number and pointer to current level drawing. */
uint8_t Level;
const uint8_t *LevelDrawing;


/* Select a level. */
void selectLevel(uint8_t level) {
	const uint8_t *p=LevelDrawings;

	/* Remember level number. */
	Level=level;

	/* Skip level drawings to given level */
	while (level--) {
		while (pgm_read_byte(&p)) p++;
		p++;
	}

	/* Remember level drawing pointer. */
	LevelDrawing=p;
}


/* Prepare current level. */
void prepareLevel(void) {
	const uint8_t *p=LevelDrawing;
	const level_item_t *q;
	uint8_t b, c, x, y, burger, place, component;
	uint8_t i;

	/* Reset options. */
	GameScreenOptions=LEVEL_ITEM_OPTION_STOMP_ONCE;

	/* Reset burgers. Places in a burger are counted from bottom to top. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		GameScreenBurger[burger].x=SCREEN_BURGER_INVALID;
		for (place=0;place<SCREEN_BURGER_PLACE_MAX;place++)
			GameScreenBurger[burger].place[place].occupied_by=SCREEN_BURGER_PLACE_INVALID;
		for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
			GameScreenBurger[burger].component[component].type=LEVEL_ITEM_INVALID;
	}

	/* Go through level specific screen list. */
	while ((b=pgm_read_byte(p))) {
		/* Start at first component block. */
		q=LevelComponents;

		/* Skip to selected drawing components block. */
		while (--b) {
			while (pgm_read_byte(&(q->component))) q++;
			q++;
		}

		/* Go through all drawing components in that block. */
		while ((c=pgm_read_byte(&(q->component)))) {
			/* Get coordinates */
			x=pgm_read_byte(&(q->position.x));
			y=pgm_read_byte(&(q->position.y));

			/* Check for type of level item. */
			switch (c) {
				case LEVEL_ITEM_PLAYER:
					Player.start_position.x=x;
					Player.start_position.y=y;
					break;
				case LEVEL_ITEM_OPTIONS:
					/* Use x and y values as option field. */
					GameScreenOptions=pgm_read_word(&(q->options));
					break;
				case LEVEL_ITEM_PLATE:
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
						/* Yes. Get next place slot. */
						for (place=0;place<SCREEN_BURGER_PLACE_MAX;place++)
							if (GameScreenBurger[burger].place[place].occupied_by == SCREEN_BURGER_PLACE_INVALID) break;

						/* Get a free component slot. */
						for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
							if (GameScreenBurger[burger].component[component].type == LEVEL_ITEM_INVALID) break;

						/* Check if we have a place slot now. */
						/* component don't need to be tested, it's always <= place */
						if (place != SCREEN_BURGER_PLACE_MAX) {
							/* Yes.	Remember burger place parameters. */
							GameScreenBurger[burger].place[place].occupied_by=SCREEN_BURGER_PLACE_FREE;

							/* Set y coordinate. Special handling for plate. */
							if (c == LEVEL_ITEM_PLATE)
									GameScreenBurger[burger].place[place].half_y = y*2-2;
								else	
									GameScreenBurger[burger].place[place].half_y = y*2+GameScreenOptions;

							/* If not plate or placeholder, do component initialisations. */
							if ((c != LEVEL_ITEM_PLATE) && (c != LEVEL_ITEM_BURGER_PLACEHOLDER)) {
								GameScreenBurger[burger].place[place].occupied_by=component|SCREEN_BURGER_PLACE_FREE_HAT;
								GameScreenBurger[burger].component[component].type=c;
								GameScreenBurger[burger].component[component].stomped=0;
								GameScreenBurger[burger].component[component].half_y=
									GameScreenBurger[burger].place[place].half_y;
								GameScreenBurger[burger].component[component].half_target_y=
									GameScreenBurger[burger].place[place].half_y;

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
				default:
					/* Ladders and floors. */
					if ((c & LEVEL_ITEM_LADDER) == LEVEL_ITEM_LADDER) {
					} else {
					}	
			}

			/* Next element in drawing component. */
			q++;
		}

		/* Next block. */
		p++;
	}	
}


/* Burger animation. */
uint8_t animateBurgers(void) {
	uint8_t burger, component, i, animated;
	burger_component_t *p;

	/* Go through all prepared burger components. */
	animated=0;
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++)
		for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++) {
			p=&(GameScreenBurger[burger].component[component]);

			/* Break at first invalid component. */
			if (p->type == LEVEL_ITEM_INVALID) break;

			/* Animation completed for this component? */
			if (p->half_y == p->half_target_y) {
				/* Yes. Stop at half tile? */
				if (p->half_y & 0x01) {
					/* Yes. Clear background line to avoid a graphics bug on next drop. */
					for (i=0;i<5;i++) {
						p->background[(~(p->half_y>>1)) & 0x01][i]=TILES0_SPACE;
					}
				}	

				/* Skip other steps. */
				continue;
			}	

			/* Animate component. Remember. */
			animated++;
			
			/* Move it down. */
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

	/* Return numberof animated components. */
	return animated;
}


/* Level start animation. */
void animateLevelStart(void) {
	const uint8_t *p=LevelDrawing;
	const level_item_t *q;
	uint8_t b, c, x, y, length, pos;

	/* Go through level specific screen list. */
	while ((b=pgm_read_byte(p))) {
		/* Start at first component block. */
		q=LevelComponents;

		/* Skip to selected drawing components block. */
		while (--b) {
			while (pgm_read_byte(&(q->component))) q++;
			q++;
		}

		/* Go through all drawing components in that block. */
		while ((c=pgm_read_byte(&(q->component))) != 0) {
			/* Get coordinates */
			x=pgm_read_byte(&(q->position.x));
			y=pgm_read_byte(&(q->position.y));

			/* Check for type of level item. */
			switch (c) {
				case LEVEL_ITEM_PLAYER:
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
								drawLadder(x,pos,length,(length==1)?c & (LEVEL_ITEM_LADDER_CONTINUED|LEVEL_ITEM_LADDER_UPONLY):(c& LEVEL_ITEM_LADDER_UPONLY)|LEVEL_ITEM_LADDER_CONTINUED);
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

			/* Next element in drawing component. */
			q++;
		}

		/* Next block. */
		p++;
	}

	/* Animate burgers when sign animation is done. */
	if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_SIGN_ENDED)
		animateBurgers();
}


/* Drop a component. */
void dropComponent(uint8_t burger, uint8_t component) {
	uint8_t component_below, place, i;
	burger_component_t *p, *q;

	/* Shortcut pointer to current component. */
	p=&(GameScreenBurger[burger].component[component]);

	/* Get place of current component. */
	for (place=1;place<SCREEN_BURGER_PLACE_MAX;place++) {
		if ((GameScreenBurger[burger].place[place].occupied_by & SCREEN_BURGER_OCCUPIED_MASK)
			== component) {
			/* Place found. Mark as free. */
			GameScreenBurger[burger].place[place].occupied_by=SCREEN_BURGER_PLACE_FREE;

			/* Clear background behind component. */
			for(i=0;i<5;i++)
				p->background[((p->half_y)>>1) & 0x01][i]=TILES0_SPACE;
			
			/* Get component on place below. */
			component_below=GameScreenBurger[burger].place[place-1].occupied_by & SCREEN_BURGER_OCCUPIED_MASK;
			if (component_below == SCREEN_BURGER_PLACE_FREE_BODY) {
				/* Place below is free. */
				/* Set target coordinate. */
				p->half_target_y=GameScreenBurger[burger].place[place-1].half_y;

				/* Plate below? */
				if (place == 1) {
					/*
					 *  Yes. The plate is always "free" to circumvent the "hat" mechanism.
					 *  Instead, the place is adjusted for each component dropped to it.
					 */
					GameScreenBurger[burger].place[0].half_y-=2;
				} else {
					/* No. Mark it as being occupied by the current component. */
					GameScreenBurger[burger].place[place-1].occupied_by=component|SCREEN_BURGER_PLACE_FREE_HAT;
				}
			} else {	
				/* Place below is occupied. Get component. */
				q=&(GameScreenBurger[burger].component[component_below]);

				/* Set target coordinate. */
				p->half_target_y=q->half_y-2;

				/* Remember current component soon being a "hat" for the component_below. */
				GameScreenBurger[burger].place[place-1].occupied_by=
					component_below|(component<<SCREEN_BURGER_OCCUPIED_HAT_SHIFT);
			}

			/* Break loop. */
			break;
		}
	}
}


/*
 *  Search a move components which have other components as a "hat".
 *  This introduces chain reaction falling of components.
 */
void dropHattedComponents(void) {
	uint8_t burger, place, body, hat;
	burger_component_t *p, *q;

	/* Go through all places. Places in a burger are counted from bottom to top. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		for (place=0;place<SCREEN_BURGER_PLACE_MAX;place++) {
			/* End burger if no places left. */
			if (place == SCREEN_BURGER_PLACE_INVALID) break;

			/* Skip this place if not "hatted". */
			hat=GameScreenBurger[burger].place[place].occupied_by >> SCREEN_BURGER_OCCUPIED_HAT_SHIFT;
			if (hat == SCREEN_BURGER_PLACE_FREE_BODY) continue;

			/* A hat has been annouced. Skip if still flying. */
			body=GameScreenBurger[burger].place[place].occupied_by & SCREEN_BURGER_OCCUPIED_MASK;
			p=&(GameScreenBurger[burger].component[hat]);
			q=&(GameScreenBurger[burger].component[body]);
			if (p->half_y != (q->half_y-2)) continue;
			
			/* Move hat half a tile up to make it jump. */
			p->half_y--;
			drawBurgerComponent(GameScreenBurger[burger].x,
				p->half_y,
				p->type-LEVEL_ITEM_BURGER_BUNTOP+SHAPE_BURGER_BUNTOP,
				p->stomped,
				p->background);

			/* Set target coordinate for hat to drop it to the place of the body. */
			p->half_target_y=GameScreenBurger[burger].place[place].half_y;

			/* Reset stomping for component under the hat. */
			q->stomped=0;
			q->half_y++;

			/* Drop component under the hat. */
			dropComponent(burger, body);

			/* Mark the place being occupied by the former hat. */
			GameScreenBurger[burger].place[place].occupied_by=hat|SCREEN_BURGER_PLACE_FREE_HAT;
		}
	}
}


/* Stomp onto a tile. */
uint8_t stomp(uint8_t x, uint8_t y) {
	uint8_t burger_x, burger, component, stomped;
	burger_component_t *p;

	/* Check which burger. */
	stomped=0;
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		burger_x=GameScreenBurger[burger].x;
		if ((x >= burger_x) && (x < burger_x+5)) {
			/* Burger selected. Check if on a component. */
			for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
				if ((GameScreenBurger[burger].component[component].half_y>>1) == y) {
					/* Component found. */
					stomped++;
					p=&(GameScreenBurger[burger].component[component]);

					/* Stomp tile.*/
					p->stomped|=1<<(x-burger_x);

					/* Restore screen at old position. */
					handleBurgerBackgroundTile(x-burger_x,
						GameScreenBurger[burger].x,
						p->half_y,
						p->stomped,
						p->background);

					/* Draw burger component at new position. */
					drawBurgerComponentTile(x-burger_x,
						GameScreenBurger[burger].x,
						p->half_y,
						p->type-LEVEL_ITEM_BURGER_BUNTOP+SHAPE_BURGER_BUNTOP,
						p->stomped,
						p->background);

					/* Check if completely stomped. */
					if (p->stomped == 0x1f) {
						/* Yes. Reset and increase half_y instead. */
						p->stomped=0;
						p->half_y++;

						/* No falling animation by default. */
						p->half_target_y = p->half_y;

						/* Check if the component should fall now. */
						if (p->half_y & 0x01) {
							/* Full tile. Check leftmost tile. */
							switch (getTile(burger_x,(p->half_y>>1))) {
								case TILES0_BURGER_FLOOR_BUNTOP_LEFT:
								case TILES0_BURGER_FLOOR_TOMATO_LEFT:
								case TILES0_BURGER_FLOOR_PATTY_LEFT:
								case TILES0_BURGER_FLOOR_CHEESESALAD_LEFT:
								case TILES0_BURGER_FLOOR_BUNBOTTOM_LEFT:
									/* Floor is just above this burger component. Drop it! */
									dropComponent(burger, component);
							}
						}
					}
				}
		}
	}

	/* Return number of stomped tiles. */
	return stomped;
}
