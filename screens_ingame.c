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
#define PLAYER_DIRECTION_UP    3
#define PLAYER_DIRECTION_DOWN  4
uint8_t PlayerDirection;

void initInGamePlayScreen(void) {
	PlayerDirection=PLAYER_DIRECTION_RIGHT;
	placeSprite(PlayerSprite,100,64,SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT);
}


void updateInGamePlayScreen(void) {
	SetTile(0,0,getSpriteFloorTile(PlayerSprite));
	SetTile(1,0,getSpriteFloorDirectionTile(PlayerSprite));
	SetTile(2,0,getSpriteLadderTile(PlayerSprite));

	/* Stomp tile under player sprite. */
	stompUnderSprite(PlayerSprite);

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Check pressed buttons. */
	switch (checkControllerButtonsPressed(0,BTN_LEFT|BTN_RIGHT)) {
		case BTN_LEFT:
			/* Change direction on floor if player direction is currently right. */
			if (PlayerDirection == PLAYER_DIRECTION_RIGHT) {
				/* Remember new direction. */
				PlayerDirection=PLAYER_DIRECTION_LEFT;
				
				/* Change sprite direction. */
				changeSpriteDirection(PlayerSprite,SPRITE_FLAGS_DIRECTION_LEFT);
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
			break;
	}

	/* Walk into current direction, if any button but the opposite is held. */
	switch (PlayerDirection) {
		case PLAYER_DIRECTION_LEFT:
			if (checkControllerButtonsHeld(0,BTN_LEFT|BTN_UP|BTN_DOWN)) {
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
				}
			}	
			break;
		case PLAYER_DIRECTION_RIGHT:
			if (checkControllerButtonsHeld(0,BTN_RIGHT|BTN_UP|BTN_DOWN)) {
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

