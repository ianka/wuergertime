/*
 *  Würgertime - An hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/ianka/wuergertime
 *
 * ---------------------------------------------------
 *
 *  screens.c - screen definitions
 */


#include <avr/io.h> /* for uint8_t */


/* Local Includes */
#include "draw.h"
#include "utils.h"
#include "player.h"
#include "opponents.h"
#include "tiles.h"
#include "sprites.h"
#include "patches.h"


/* Maximum number of burger components per screen. */
#define SCREEN_BURGER_MAX 4
#define SCREEN_BURGER_COMPONENT_MAX 5
#define SCREEN_BURGER_PLACE_MAX ((SCREEN_BURGER_COMPONENT_MAX+2))
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


/* Game screen objects. */
uint8_t Scratchpad;
uint32_t Score;
uint32_t DisplayedScore;
uint16_t Bonus;
uint8_t Lives;
uint8_t Peppers;
position_t GameScreenSignPosition;
position_t GameScreenScorePosition;
position_t GameScreenLevelPosition;
position_t GameScreenBonusPosition;
position_t GameScreenLivesPosition;
position_t GameScreenPeppersPosition;
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
uint8_t HurryBuffer[3];


/* Lavel number and pointer to current level drawing. */
uint8_t Level;
const uint8_t *LevelDrawing;


/* Select a level. */
void selectLevel(uint8_t level) {
	const uint8_t *p=LevelDrawings;

	/* Remember level number. */
	Level=level;

	/* Start level descriptions with level 1. */
	level--;

	/* Skip level drawings to given level. */
	while (level--) {
		/* Skip to next level description. */
		while (pgm_read_byte(p)) p++;
		p++;

		/* Restart with first level if end of level descriptions reached. */
		if (!pgm_read_byte(p)) p=LevelDrawings;
	}

	/* Remember level drawing pointer. */
	LevelDrawing=p;
}


/* Prepare current level description. */
void prepareLevelDescription(uint8_t sy) {
	const uint8_t *p=LevelDrawing;
	const level_item_t *q;
	uint8_t b, c, x, y, cy;

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
			/* Get coordinate */
			x=pgm_read_byte(&(q->position.x));
			y=pgm_read_byte(&(q->position.y));

			/* Check for type of level item. */
			switch (c) {
				case LEVEL_ITEM_BURGER_BUNTOP:
					/* Draw top of sign and bun top. */
					drawShape(x,sy-9,ShapeSignLevelDescriptionTop,0);
					break;
				case LEVEL_ITEM_BURGER_TOMATO:
					/* Draw tomato part of sign. */
					drawShape(x,sy-7,ShapeSignLevelDescriptionTomato,0);
					break;
				case LEVEL_ITEM_BURGER_PATTY:
					/* Draw patty part of sign. */
					drawShape(x,sy-6,ShapeSignLevelDescriptionPatty,0);
					break;
				case LEVEL_ITEM_BURGER_CHEESESALAD:
					/* Draw cheese salad part of sign. */
					drawShape(x,sy-5,ShapeSignLevelDescriptionCheesesalad,0);
					break;
				case LEVEL_ITEM_BURGER_BUNBOTTOM:
					/* Draw bun bottom bottom of sign and poles. */
					drawShape(x,sy-4,ShapeSignLevelDescriptionPost,0);
					break;
				case LEVEL_ITEM_SCORE:
					/* Draw score. */
					drawScore(x,y,Score);
					break;
				case LEVEL_ITEM_LEVEL:
					/* Draw level. */
					drawLevel(x,y,Level);
					break;
			}

			/* Next element in drawing component. */
			q++;
		}

		/* Next block. */
		p++;
	}

	/* Collapse the columns. */
	for (x=0;x<SCREEN_WIDTH;x++) {
		for (cy=sy-5;cy>sy-10;cy--) {
			for (y=cy;y>cy-5;y--) {
				if ((getTile(x,y)==TILES1_SPACE)) {
					setTile(x,y,getTile(x,y-1));
					setTile(x,y-1,TILES1_SPACE);
				}
			}
		}
	}
}


/* Prepare current level. */
void prepareLevel(void) {
	const uint8_t *p=LevelDrawing;
	const level_item_t *q;
	uint8_t b, c, x, y, burger, place, component;
	uint8_t i, opponent_start_index;

	/* Reset options. */
	GameScreenOptions=LEVEL_ITEM_OPTION_STOMP_ONCE|LEVEL_ITEM_OPTION_OPPONENT_DUO;

	/* Reset burgers. Places in a burger are counted from bottom to top. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		GameScreenBurger[burger].x=SCREEN_BURGER_INVALID;
		for (place=0;place<SCREEN_BURGER_PLACE_MAX;place++)
			GameScreenBurger[burger].place[place].occupied_by=SCREEN_BURGER_PLACE_INVALID;
		for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
			GameScreenBurger[burger].component[component].type=LEVEL_ITEM_INVALID;
	}

	/* Reset opponent start positions. */
	opponent_start_index=0;
	for (i=0;i<OPPONENT_START_POSITION_MAX;i++)
		OpponentStartPosition[i].x=OPPONENT_START_POSITION_INVALID;


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
				case LEVEL_ITEM_PLAYERSTARTPOINT:
					/* Remember player start position. */
					Player.start_position.x=x;
					Player.start_position.y=y;
					break;
				case LEVEL_ITEM_OPPONENTSTARTPOINT:
					/* Remember opponent start position. */
					if (opponent_start_index<OPPONENT_START_POSITION_MAX) {
						OpponentStartPosition[opponent_start_index].x=x;
						OpponentStartPosition[opponent_start_index].y=y;
						opponent_start_index++;
					}
					break;
				case LEVEL_ITEM_ATTACKWAVES:
					/* Remember attack waves. */
					OpponentAttackWaves=pgm_read_word(&(q->options));
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
						/* Component doesn't need to be tested, it's always <= place */
						if (place != SCREEN_BURGER_PLACE_MAX) {
							/* Yes.	Remember burger place parameters. */
							GameScreenBurger[burger].place[place].occupied_by=SCREEN_BURGER_PLACE_FREE;

							/* Set y coordinate. Special handling for plate. */
							if (c == LEVEL_ITEM_PLATE)
									GameScreenBurger[burger].place[place].half_y = y*2-2;
								else
									GameScreenBurger[burger].place[place].half_y = y*2+((GameScreenOptions & LEVEL_ITEM_OPTION_STOMP_MASK)>>LEVEL_ITEM_OPTION_STOMP_SHIFT);

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
					/* Remember sign position. */
					GameScreenSignPosition.x=x;
					GameScreenSignPosition.y=y;
					break;
				case LEVEL_ITEM_SCORE:
					/* Remember score position. */
					GameScreenScorePosition.x=x;
					GameScreenScorePosition.y=y;
					break;
				case LEVEL_ITEM_LEVEL:
					/* Remember level position. */
					GameScreenLevelPosition.x=x;
					GameScreenLevelPosition.y=y;
					break;
				case LEVEL_ITEM_BONUS:
					/* Remember bonus position. */
					GameScreenBonusPosition.x=x;
					GameScreenBonusPosition.y=y;
					break;
				case LEVEL_ITEM_LIVES:
					/* Remember lives position. */
					GameScreenLivesPosition.x=x;
					GameScreenLivesPosition.y=y;
					break;
				case LEVEL_ITEM_PEPPERS:
					/* Remember peppers position. */
					GameScreenPeppersPosition.x=x;
					GameScreenPeppersPosition.y=y;
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
				case LEVEL_ITEM_PLAYERSTARTPOINT:
				case LEVEL_ITEM_OPPONENTSTARTPOINT:
				case LEVEL_ITEM_ATTACKWAVES:
				case LEVEL_ITEM_OPTIONS:
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
								if (blink((GameScreenAnimationPhase-LEVEL_START_ANIMATION_SIGNFRAME_ENDED)>>LEVEL_START_ANIMATION_SIGN_SHIFT,LEVEL_START_ANIMATION_SIGN_BLINKCODE))
										drawShape(x,y,ShapeSignInGame,0);
									else
										drawShape(x,y,ShapeSignLevelStart,0);
							}
						}
					}
					break;
				case LEVEL_ITEM_SCORE:
					/* Draw score. */
					DisplayedScore=Score;
					drawScore(x,y,DisplayedScore);
					break;
				case LEVEL_ITEM_LEVEL:
					/* Draw level. */
					drawLevel(x,y,Level);
					break;
				case LEVEL_ITEM_BONUS:
					/* Draw bonus. */
					drawBonus(x,y,Bonus);
					break;
				case LEVEL_ITEM_LIVES:
					/* Draw lives. */
					drawLives(x,y,Lives);
					break;
				case LEVEL_ITEM_PEPPERS:
					/* Draw peppers. */
					drawPeppers(x,y,Peppers);
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
								drawLadder(x,pos,length,(length==1)?c & (LEVEL_ITEM_LADDER_UPONLY):(c & LEVEL_ITEM_LADDER_UPONLY)|DRAW_OPTION_LADDER_CONTINUED);
						}
					} else {
						/* Floor. Animate width. */
						if (GameScreenAnimationPhase <= LEVEL_START_ANIMATION_FLOORS_ENDED) {
							length=min(c & LEVEL_ITEM_FLOOR_LENGTH,GameScreenAnimationPhase);
							pos=x+((c & LEVEL_ITEM_FLOOR_LENGTH)-length)/2;

							/* Draw it. */
							drawFloor(pos,y,length,(GameScreenOptions & LEVEL_ITEM_OPTION_WRAP_MASK)?0:DRAW_OPTION_FLOOR_FORCE_NOWRAP);
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
	burger_component_t *p;

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

			/* Set target coordinate. */
			p->half_target_y=GameScreenBurger[burger].place[place-1].half_y;

			/* Get component on place below. */
			component_below=GameScreenBurger[burger].place[place-1].occupied_by & SCREEN_BURGER_OCCUPIED_MASK;
			if (component_below == SCREEN_BURGER_PLACE_FREE_BODY) {
				/* Place below is free. */
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
 *  Search and move components which have other components as a "hat".
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

			/* Score for falling component. */
			Score+=SCORE_COMPONENT_FALLING_CASCADE;

			/* Trigger topple sound. */
			TriggerFx(PATCH_BURGER_TOPPLE,127,true);
		}
	}
}


/* Stomp onto a tile. */
uint8_t stomp(uint8_t x, uint8_t y) {
	uint8_t burger_x, burger, component, stomped, oldstomped;
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
					oldstomped=p->stomped;
					p->stomped|=1<<(x-burger_x);

					/* Check if this tile has been stomped right now. */
					if (p->stomped != oldstomped) {
						/* Yes. Score. */
						Score+=SCORE_STOMPED_TILE;

						/* Trigger stomp sound. */
						TriggerFx(PATCH_STOMP,127,true);
					}

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

									/* Score for falling component. */
									Score+=SCORE_COMPONENT_FALLING;

									/* Trigger fall sound. */
									TriggerFx(PATCH_BURGER_FALL,127,true);
							}
						}
					}
				}
		}
	}

	/* Return number of stomped tiles. */
	return stomped;
}


/* Get the position of a random burger component. Used for patrolling opponents. */
position_t getRandomBurgerComponentPosition() {
	uint8_t burger, component;

	/* Try endless, until we have the required component. */
	for(;;) {
		/* Choose a random burger. */
		burger=fastrandom()%4;

		/* Retry if the burger is invalid. */
		if (GameScreenBurger[burger].x == SCREEN_BURGER_INVALID) continue;

		/* Try endless, until we have the required component. */
		for(;;) {
			/* Choose a random component. */
			component=fastrandom()%5;

			/* Retry if the component is invalid. */
			if (GameScreenBurger[burger].component[component].type == LEVEL_ITEM_INVALID) continue;

			/* Return component position. */
			return (position_t){ x: (GameScreenBurger[burger].x+2)<<3, y: (GameScreenBurger[burger].component[component].half_y+4)<<2 };
		}

		/* The burger doesn't have this component. Try another. */
	}
}


/* Check if at any falling burger component position. */
uint8_t checkFallingBurgerComponentPosition(uint8_t x, uint8_t y) {
	uint8_t burger, component;

	/* Check all burgers. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		/* Check x coordinates of burgers. */
		if ((x>(GameScreenBurger[burger].x<<3)) && (x<((GameScreenBurger[burger].x+5)<<3))) {
			/* Matching burger found. Check y coordinates of all components. */
			for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++)
				if ((y>(((GameScreenBurger[burger].component[component].half_y>>1))<<3))
						&& (y<(((GameScreenBurger[burger].component[component].half_y>>1)+2)<<3)))
					/* Hit by burger component. */
					return 1;

				/* Not hit. Check next. */
		}
	}

	/* No match. */
	return 0;
}


/* Update statistics on screen (score, level, bonus, lives, peppers etc.) */
void updateGameScreenStatistics(void) {
	/* Crawl to actual score. */
	if (DisplayedScore+13 < Score)
		DisplayedScore+=13;
	else if (DisplayedScore+3 < Score)
		DisplayedScore+=3;
	else if (DisplayedScore < Score)
		DisplayedScore++;

	/* Finally update the displayed score, lives, and peppers. */
	drawScore(GameScreenScorePosition.x,GameScreenScorePosition.y,DisplayedScore);
	drawLives(GameScreenLivesPosition.x,GameScreenLivesPosition.y,Lives);
	drawPeppers(GameScreenPeppersPosition.x,GameScreenPeppersPosition.y,Peppers);
}


/* Decrement bonus. */
uint8_t decrementBonus(void) {
	/* Return with flag set if bonus is already zero. */
	if (Bonus == 0) return 1;

	/* Decrement bonus, honour speed set by level option. */
	if (!(GameScreenAnimationPhase & ~(0xffff<<(((GameScreenOptions & LEVEL_ITEM_OPTION_BONUS_MASK)>>LEVEL_ITEM_OPTION_BONUS_SHIFT)+BONUS_DEFAULT_SHIFT))))
		Bonus--;

	/* Update bonus counter on screen. */
	drawBonus(GameScreenBonusPosition.x,GameScreenBonusPosition.y,Bonus);

	/* Return without flag set. */
	return 0;
}

/* Decrement bonus fast. */
uint8_t decrementBonusFast(void) {
	/* Return with flag set if bonus is already zero. */
	if (Bonus == 0) return 1;

	/* High speed bonus decrement. */
	if (Bonus >= BONUS_FAST_DECREMENT)
		Bonus-=BONUS_FAST_DECREMENT;
	else
		Bonus=0;

	/* Update bonus counter on screen. */
	drawBonus(GameScreenBonusPosition.x,GameScreenBonusPosition.y,Bonus);

	/* Return without flag set. */
	return 0;
}


/* Animate "Hurry!" sign. */
uint8_t animateHurry(void) {
	uint8_t x,y;

	/* Get tile coordinate of the cook. */
	x=getSpriteTileX(Player.sprite,0);
	y=getSpriteTileY(Player.sprite,0);

	/* Decide what to do by animation phase. */
	switch (GameScreenAnimationPhase) {
		case HURRY_ANIMATION_DRAW_UPPER_LEFT:
			/* Draw "Hurry!" animation left above cook. */
			drawHurry(x-5,y-3,HurryBuffer);

			/* Trigger hurry sound. */
			TriggerFx(PATCH_HURRY,255,true);
			break;
		case HURRY_ANIMATION_REMOVE_UPPER_LEFT:
			/* Restore background left above cook. */
			restoreHurryBackground(x-5,y-3,HurryBuffer);
			break;
		case HURRY_ANIMATION_DRAW_LOWER_RIGHT:
			/* Draw "Hurry!" animation right below cook. */
			drawHurry(x+2,y+2,HurryBuffer);

			/* Trigger hurry sound. */
			TriggerFx(PATCH_HURRY,255,true);
			break;
		case HURRY_ANIMATION_REMOVE_LOWER_RIGHT:
			/* Restore background right below cook. */
			restoreHurryBackground(x+2,y+2,HurryBuffer);
			break;
		case HURRY_ANIMATION_DRAW_UPPER_RIGHT:
			/* Draw "Hurry!" animation right above cook. */
			drawHurry(x+2,y-3,HurryBuffer);

			/* Trigger hurry sound. */
			TriggerFx(PATCH_HURRY,255,true);
			break;
		case HURRY_ANIMATION_REMOVE_UPPER_RIGHT:
			/* Restore background right above cook. */
			restoreHurryBackground(x+2,y-3,HurryBuffer);
			break;
		case HURRY_ANIMATION_DRAW_LOWER_LEFT:
			/* Draw "Hurry!" animation left below cook. */
			drawHurry(x-5,y+2,HurryBuffer);

			/* Trigger hurry sound. */
			TriggerFx(PATCH_HURRY,255,true);
			break;
		case HURRY_ANIMATION_REMOVE_LOWER_LEFT:
			/* Restore background left below cook. */
			restoreHurryBackground(x-5,y+2,HurryBuffer);

			/* Animation complete. */
			return 1;
	}

	/* Animation not complete. */
	return 0;
}


/* Drop all burger components off-screen. */
void dropAllBurgersOffScreen(void) {
	uint8_t i, burger, component;
	burger_component_t *p;

	/* Check all burgers. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++)
		for (component=0;component<SCREEN_BURGER_COMPONENT_MAX;component++) {
			p=&(GameScreenBurger[burger].component[component]);

			/* Break at first invalid component. */
			if (p->type == LEVEL_ITEM_INVALID) break;

			/* Stomp the burger completely. */
			p->stomped=0x1f;

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

			/* Reset stomping. */
			p->stomped=0;
			p->half_y++;

			/* Clear background behind component. */
			for(i=0;i<5;i++)
				p->background[((p->half_y)>>1) & 0x01][i]=TILES0_SPACE;

			/* Set new target position. */
			p->half_target_y=SCREEN_HEIGHT*2;
		}
}


/* Award any served burger to the score. */
uint8_t awardServedBurgers(void) {
	uint8_t burger;

	/* Check all burgers. */
	for (burger=0;burger<SCREEN_BURGER_MAX;burger++) {
		/* Skip unused and already counted plates. */
		if (GameScreenBurger[burger].component[0].type == LEVEL_ITEM_INVALID
			|| GameScreenBurger[burger].component[0].type == LEVEL_ITEM_COUNTED) continue;

		/* Check buntop position for small burger. */
		if (GameScreenBurger[burger].component[2].type == LEVEL_ITEM_BURGER_BUNTOP
			&& GameScreenBurger[burger].component[2].half_y == (GameScreenBurger[burger].component[0].half_y-4)) {
			Score+=SCORE_BURGER_SMALL;
			GameScreenBurger[burger].component[0].type = LEVEL_ITEM_COUNTED;
			continue;
		}

		/* Check buntop position for medium burger. */
		if (GameScreenBurger[burger].component[3].type == LEVEL_ITEM_BURGER_BUNTOP
			&& GameScreenBurger[burger].component[3].half_y == (GameScreenBurger[burger].component[0].half_y-6)) {
			Score+=SCORE_BURGER_MEDIUM;
			GameScreenBurger[burger].component[0].type = LEVEL_ITEM_COUNTED;
			continue;
		}

		/* Check buntop position for large burger. */
		if (GameScreenBurger[burger].component[4].type == LEVEL_ITEM_BURGER_BUNTOP
			&& GameScreenBurger[burger].component[4].half_y == (GameScreenBurger[burger].component[0].half_y-8)) {
			Score+=SCORE_BURGER_LARGE;
			GameScreenBurger[burger].component[0].type = LEVEL_ITEM_COUNTED;
			continue;
		}

		/* At least one burger not served. */
		return 0;
	}

	/* All burgers are served. */
	return 1;
}

