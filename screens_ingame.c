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
 *  screens_ingame.c - in game screen init, update, cleanup functions.
 */


#include <uzebox.h>


/* Local includes. */
#include "utils.h"
#include "screens.h"
#include "draw.h"
#include "tiles.h"
#include "sprites.h"
#include "controllers.h"
#include "player.h"


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
	Player.sprite=occupySpriteSlot();
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
void initInGamePlayScreen(void) {
	/* Reset player to start position. */
	resetPlayer();
}


void updateInGamePlayScreen(void) {
	uint8_t directional_buttons_held;

SetTile(27,0,getSpriteFloorTile(Player.sprite));
SetTile(28,0,getSpriteFloorDirectionTile(Player.sprite));
SetTile(29,0,getSpriteLadderTile(Player.sprite));
PrintInt(10,0,getSpriteX(Player.sprite),1);

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Check held buttons. */
	directional_buttons_held=checkControllerButtonsHeld(0,BTN_DIRECTIONS);
	switch (directional_buttons_held) {
		case BTN_LEFT:
			/* Skip if we already are on our way left. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_LEFT)
				break;

			/* Change direction on floor if player direction is currently right. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_RIGHT)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);

			/* Change direction from ladder to floor if player is at a ladder exit. */
			if (checkSpriteAtLadderExit(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);

			break;	
		case BTN_RIGHT:
			/* Skip if we already are on our way right. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_RIGHT)
				break;

			/* Change direction on floor if player direction is currently left. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_LEFT)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			/* Change direction from ladder to floor if player is at a ladder exit. */
			if (checkSpriteAtLadderExit(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			break;
		case BTN_DOWN:
			/* Skip if we already are on our way down. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_DOWN)
				break;

			/* Change direction on ladder if player direction is currently up. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_UP)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);

			/* Change direction from floor to ladder if player is onto a ladder top. */
			if (checkSpriteAtLadderEntryDown(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);

			break;
		case BTN_UP:
			/* Skip if we already are on our way up. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_UP)
				break;
			
			/* Change direction on ladder if player direction is currently down. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_DOWN)
				changePlayerDirectionWithoutAnimationReset(PLAYER_FLAGS_DIRECTION_UP);

			/* Change direction from floor to ladder if player is at a ladder. */
			if (checkSpriteAtLadderEntryUp(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_UP);

			break;
	}

	/* Walk into current direction, if any button but the opposite is held. */
	switch (Player.flags & PLAYER_FLAGS_DIRECTION_MASK) {
		case PLAYER_FLAGS_DIRECTION_LEFT:
			/* Move sprite if nothing should stop us. */
			if ((directional_buttons_held & (BTN_LEFT|BTN_UP|BTN_DOWN)) && (!checkSpriteAtLeftFloorEnd(Player.sprite))) {
				/* Move! */
				moveSprite(Player.sprite,-(1<<((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)),0);

				/* Stomp tile under player sprite. */
				stompUnderSprite(Player.sprite);
			}

			break;
		case PLAYER_FLAGS_DIRECTION_RIGHT:
			if ((directional_buttons_held & (BTN_RIGHT|BTN_UP|BTN_DOWN)) && (!checkSpriteAtRightFloorEnd(Player.sprite))) {
				/* Move! */
				moveSprite(Player.sprite,(1<<((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)),0);

				/* Stomp tile under player sprite. */
				stompUnderSprite(Player.sprite);
			}

			break;
		case PLAYER_FLAGS_DIRECTION_DOWN:
			if ((directional_buttons_held & (BTN_RIGHT|BTN_LEFT|BTN_DOWN)) && (!checkSpriteAtLadderBottom(Player.sprite)))
				/* Move! */
				moveSprite(Player.sprite,0,(1<<min(0,((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)-1)));

			break;
		case PLAYER_FLAGS_DIRECTION_UP:
			if ((directional_buttons_held & (BTN_RIGHT|BTN_LEFT|BTN_UP)) && (!checkSpriteAtLadderTop(Player.sprite)))
				/* Move! */
				moveSprite(Player.sprite,0,-(1<<min(0,((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)-1)));
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

