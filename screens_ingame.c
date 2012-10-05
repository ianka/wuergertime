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
 *  screen_ingame.c - in game screen init, update, cleanup functions.
 */


#include <uzebox.h>


/* Local includes. */
#include "screens.h"
#include "draw.h"
#include "tiles.h"
#include "sprites.h"
#include "controllers.h"


/*
 *  The description screen is the first screen for each level to be showed.
 *  It places a signpost with some description what this level will be like.
 *  Tileset 1 is selected as we need an alphabet.
 */
void initInGameDescriptionScreen(void) {
}

void updateInGameDescriptionScreen(void) {
}

void cleanupInGameDescriptionScreen(void) {
}


/*
 *  The prepare screen is showed when a new level is reached.
 *    + Score displays
 *    + Opening animation of floors and burgers 
 *    + Burger positions are being reset
 */
void initInGamePrepareScreen(void) {
	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);

	/* Prepare level by description. */
	prepareLevel();
}

void updateInGamePrepareScreen(void) {
	/* Level build animation. */
	animateLevelStart();

	/* Change screen when animation is done. */
	if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_ENDED+1)
		ChangeGameScreen(GAME_SCREEN_LEVEL_START);
}

void cleanupInGamePrepareScreen(void) {
}


/*
 *  The start screen is showed after prepare screen or after a life is lost.
 *     + Cook and Monsters reset
 *     + Bonus reset
 */
void initInGameStartScreen(void) {
	/* Reset controllers. */
	resetControllers();

	/* Reset sprites. */
	resetSpriteSlots();
	PlayerSprite=occupySpriteSlot();
#if 0
	stomp(2,20);
	stomp(3,20);
	stomp(4,20);
	stomp(5,20);
#endif	
}

void updateInGameStartScreen(void) {
	/* Burger drop animation. */
	/* Change screen when animation is done. */
	dropHattedComponents();
	if (!animateBurgers())
		ChangeGameScreen(GAME_SCREEN_LEVEL_PLAY);
}

void cleanupInGameStartScreen(void) {
}


/*
 *  The play screen is showed when the actual game is happening.
 */
#define PLAYER_DIRECTION_NONE  0
#define PLAYER_DIRECTION_LEFT  1
#define PLAYER_DIRECTION_RIGHT 2
#define PLAYER_DIRECTION_DOWN  3
#define PLAYER_DIRECTION_UP    4
uint8_t PlayerDirection;

void initInGamePlayScreen(void) {
	PlayerDirection=PLAYER_DIRECTION_RIGHT;
	placeSprite(PlayerSprite,120,64,SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT);
}


void updateInGamePlayScreen(void) {
	uint8_t directional_buttons_held;

	SetTile(27,0,getSpriteFloorTile(PlayerSprite));
	SetTile(28,0,getSpriteFloorDirectionTile(PlayerSprite));
	SetTile(29,0,getSpriteLadderTile(PlayerSprite));

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Check held buttons. */
	directional_buttons_held=checkControllerButtonsHeld(0,BTN_DIRECTIONS);
	switch (directional_buttons_held) {
		case BTN_LEFT:
			/* Change direction on floor if player direction is currently right. */
			if (PlayerDirection == PLAYER_DIRECTION_RIGHT) {
				/* Remember new direction. */
				PlayerDirection=PLAYER_DIRECTION_LEFT;
				
				/* Change sprite direction. */
				changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_LEFT);
			}

			/* Change direction from ladder to floor if player is onto a ladder top or bottom. */
			if (!(getSpriteY(PlayerSprite) & 0x07)) {
				/* On an exact tile coordinate. Check floor. */
				switch (getSpriteFloorTile(PlayerSprite)) {
					case TILES0_LADDER_TOP_RIGHT:
					case TILES0_LADDER_TOP_FLOOREND_RIGHT:
					case TILES0_LADDER_BOTTOM_RIGHT:
					case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
						/* Remember new direction. */
						PlayerDirection=PLAYER_DIRECTION_LEFT;

						/* Change sprite direction. */
						changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_LEFT);
						break;
					default:
						/* Not on a top/bottom. */
						;
				}
			}
			break;	
		case BTN_RIGHT:
			/* Change direction on floor if player direction is currently left. */
			if (PlayerDirection == PLAYER_DIRECTION_LEFT) {
				/* Remember new direction. */
				PlayerDirection=PLAYER_DIRECTION_RIGHT;
				
				/* Change sprite direction. */
				changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_RIGHT);
			}	

			/* Change direction from ladder to floor if player is onto a ladder top or bottom. */
			if (!(getSpriteY(PlayerSprite) & 0x07)) {
				/* On an exact tile coordinate. Check floor. */
				switch (getSpriteFloorTile(PlayerSprite)) {
					case TILES0_LADDER_TOP_RIGHT:
					case TILES0_LADDER_TOP_FLOOREND_RIGHT:
					case TILES0_LADDER_BOTTOM_RIGHT:
					case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
						/* Remember new direction. */
						PlayerDirection=PLAYER_DIRECTION_RIGHT;

						/* Change sprite direction. */
						changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_RIGHT);
						break;
					default:
						/* Not on a top/bottom. */
						;
				}
			}	
			break;
		case BTN_DOWN:
			/* Change direction on ladder if player direction is currently up. */
			if (PlayerDirection == PLAYER_DIRECTION_UP) {
				/* Remember new direction. */
				PlayerDirection=PLAYER_DIRECTION_DOWN;
			}

			/* Change direction from floor to ladder if player is onto a ladder top. */
			if (!(getSpriteX(PlayerSprite) & 0x07)) {
				/* On an exact tile coordinate. Check floor. */
				switch (getSpriteFloorTile(PlayerSprite)) {
					case TILES0_LADDER_TOP_RIGHT:
					case TILES0_LADDER_TOP_FLOOREND_RIGHT:
						/* Remember new direction. */
						PlayerDirection=PLAYER_DIRECTION_DOWN;

						/* Change sprite direction. */
						changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_LADDER);
						break;
					default:
						/* Not on a top. */
						;
				}
			}
			break;
		case BTN_UP:
			/* Change direction on ladder if player direction is currently down. */
			if (PlayerDirection == PLAYER_DIRECTION_DOWN) {
				/* Remember new direction. */
				PlayerDirection=PLAYER_DIRECTION_UP;
			}

			/* Change direction from floor to ladder if player is at a ladder. */
			if (!(getSpriteX(PlayerSprite) & 0x07)) {
				/* On an exact tile coordinate. Check ladder. */
				switch (getSpriteLadderTile(PlayerSprite)) {
					case TILES0_LADDER_RIGHT:
					case TILES0_LADDER_MUSTARDED_RIGHT:
					case TILES0_LADDER_MUSTARDED_CLEANED_RIGHT:
					case TILES0_LADDER_TOP_RIGHT:
					case TILES0_LADDER_TOP_FLOOREND_RIGHT:
						/* No. Remember new direction. */
						PlayerDirection=PLAYER_DIRECTION_UP;

						/* Change sprite direction. */
						changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_LADDER);
						break;
				}		
			}
			break;
	}

	/* Walk into current direction, if any button but the opposite is held. */
	switch (PlayerDirection) {
		case PLAYER_DIRECTION_LEFT:
			if (directional_buttons_held & (BTN_LEFT|BTN_UP|BTN_DOWN)) {
				/* Check the floor tile for anything that should stop us. */
				switch (getSpriteFloorTile(PlayerSprite)) {
					case TILES0_FLOOR_LEFT:
					case TILES0_LADDER_TOP_FLOOREND_LEFT:
					case TILES0_LADDER_BOTTOM_FLOOREND_LEFT:
						/* No move. */
						break;
					default:
						/* Move! */
						moveSprite(PlayerSprite,-2,0);

						/* Stomp tile under player sprite. */
						stompUnderSprite(PlayerSprite);
				}
			}	
			break;
		case PLAYER_DIRECTION_RIGHT:
			if (directional_buttons_held & (BTN_RIGHT|BTN_UP|BTN_DOWN)) {
				/* Check the floor tile for anything that should stop us. */
				switch (getSpriteFloorTile(PlayerSprite)) {
					case TILES0_FLOOR_RIGHT:
					case TILES0_LADDER_TOP_FLOOREND_RIGHT:
					case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
						/* No move. */
						break;
					default:
						/* Move! */
						moveSprite(PlayerSprite,2,0);

						/* Stomp tile under player sprite. */
						stompUnderSprite(PlayerSprite);
				}
			}	
			break;
		case PLAYER_DIRECTION_DOWN:
			if (directional_buttons_held & (BTN_LEFT|BTN_RIGHT|BTN_DOWN)) {
				/* Check the floor tile for anything that should stop us. */
				switch (getSpriteFloorTile(PlayerSprite)) {
					case TILES0_LADDER_BOTTOM_RIGHT:
					case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
					case TILES0_LADDER_TOP_UPONLY_RIGHT:
						/* No move. */
						break;
					default:
						/* Move! */
						moveSprite(PlayerSprite,0,2);

						/* Stomp tile under player sprite. */
						stompUnderSprite(PlayerSprite);
				}
			}	
			break;
		case PLAYER_DIRECTION_UP:
			if (directional_buttons_held & (BTN_LEFT|BTN_RIGHT|BTN_UP)) {
				/* Check the floor tile for anything that should stop us. */
				switch (getSpriteLadderTopTile(PlayerSprite)) {
					case TILES0_LADDER_TOP_RIGHT:
					case TILES0_LADDER_TOP_FLOOREND_RIGHT:
						/* Ladder top end? */
						switch (getSpriteLadderTile(PlayerSprite)) {
							case TILES0_LADDER_RIGHT:
							case TILES0_LADDER_TOP_RIGHT:
							case TILES0_LADDER_TOP_FLOOREND_RIGHT:
							case TILES0_LADDER_MUSTARDED_RIGHT:
							case TILES0_LADDER_MUSTARDED_CLEANED_RIGHT:
								/* No. Move! */
								moveSprite(PlayerSprite,0,-2);

								/* Stomp tile under player sprite. */
								stompUnderSprite(PlayerSprite);
								break;
						}
						break;
					default:
						/* Move! */
						moveSprite(PlayerSprite,0,-2);

						/* Stomp tile under player sprite. */
						stompUnderSprite(PlayerSprite);
				}
			}	
			break;
	}
}

void cleanupInGamePlayScreen(void) {
}


/*
 *  The hurry screen is showed when bonus counter reaches zero before all burgers are done.
 *  It shows the hurry animation on the level screen.
 */
void initInGameHurryScreen(void) {
}

void updateInGameHurryScreen(void) {
}

void cleanupInGameHurryScreen(void) {
}


/*
 *  The bonus screen is showed when a bonus item was collected.
 *  It shows the bonus animation on the level screen.
 */
void initInGameBonusScreen(void) {
}

void updateInGameBonusScreen(void) {
}

void cleanupInGameBonusScreen(void) {
}


/*
 *  The lose screen is showed when a life is lost.
 *  It shows the lose animation on the level screen.
 */
void initInGameLoseScreen(void) {
}

void updateInGameLoseScreen(void) {
}

void cleanupInGameLoseScreen(void) {
}


/*
 *  The win screen is showed when a level is completed.
 *  It shows the winning animation on the level screen.
 */
void initInGameWinScreen(void) {
}

void updateInGameWinScreen(void) {
}

void cleanupInGameWinScreen(void) {
}


/*
 *  The aftermath screen is showed after the win screen.
 *  It shows individual collected items and bonus added to the score.
 *  Tileset 1 is selected as we need an alphabet.
 */
void initInGameAftermathScreen(void) {
}

void updateInGameAftermathScreen(void) {
}

void cleanupInGameAftermathScreen(void) {
}

