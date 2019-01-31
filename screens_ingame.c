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
#include "opponents.h"


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
 *    + Burger and opponent positions are being reset
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
	/* Reset sprites. */
	resetSpriteSlots();
	Player.sprite=occupySpriteSlot();
	Opponent[0].sprite=occupySpriteSlot();
	Opponent[1].sprite=occupySpriteSlot();
	Opponent[2].sprite=occupySpriteSlot();
	Opponent[3].sprite=occupySpriteSlot();

	/* Reset opponent positions. */
	resetOpponents();
}


/*
 *  The start screen is showed after prepare screen or after a life is lost.
 *     + Cook reset
 *     + Bonus reset
 */
void initInGameStartScreen(void) {
	/* Reset controllers. */
	resetControllers();

	/* Reset bonus. */
	Bonus=DEFAULT_BONUS;

	/* Reset player to start position. */
	resetPlayer();
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
}


void updateInGamePlayScreen(void) {
	uint8_t directional_buttons_held;
	uint8_t i;

	/* Animate open sign. */
	if (GameScreenAnimationPhase & 64) {
		SetTile(15,0,TILES0_OPEN_OFF_LEFT);
		SetTile(16,0,TILES0_OPEN_OFF_RIGHT);
	} else {
		SetTile(15,0,TILES0_OPEN_ON_LEFT);
		SetTile(16,0,TILES0_OPEN_ON_RIGHT);
	}

	/* Player start blink animation. */
	if (GameScreenAnimationPhase < PLAYER_START_BLINKING_ENDED) {
		if (blink((GameScreenAnimationPhase)>>PLAYER_START_BLINKING_SHIFT,PLAYER_START_BLINKCODE)) {
			/* Normal player. */
			setSpriteTransparency(Player.sprite,0);
			updateSprite(Player.sprite);
		}	else {
			/* Transparent player. */
			setSpriteTransparency(Player.sprite,SPRITE_FLAGS_TRANSPARENT);
			updateSprite(Player.sprite);
		}
	}

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Check if player and opponents should move. */
	if ((Bonus < HURRY_BONUS) || (GameScreenAnimationPhase & 1)) {
		/* Yes. Get held buttons. */
		directional_buttons_held=checkControllerButtonsHeld(0,BTN_DIRECTIONS);

		/* Select direction to move player. */
		selectPlayerDirection(directional_buttons_held);

		/* Move player into selected direction, if possible. */
		movePlayer(directional_buttons_held);

		/* Handle all opponents. */
		for (i=0;i<OPPONENT_MAX;i++) {
			/* Remove opponent if it is hit by a burger component. */
			removeOpponentIfHit(i);

			/* Select direction and move all active opponents. */
			selectOpponentDirection(i);
			moveOpponent(i);

			/* Change to lose screen when an opponent caught a player. */
			if ((GameScreenAnimationPhase >= PLAYER_START_BLINKING_ENDED) && checkOpponentCaughtPlayer(i))
				ChangeGameScreen(GAME_SCREEN_LEVEL_LOSE);
		}
	}

	/* Start next attack wave on matching animation phase. */
	nextAttackWave();

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Decrement bonus, lose a life when bonus is zero. */
	if (decrementBonus())
		ChangeGameScreen(GAME_SCREEN_LEVEL_LOSE);

	/* Check if we are at hurry moment. */
	if (Bonus == HURRY_BONUS)
		ChangeGameScreen(GAME_SCREEN_LEVEL_HURRY);

	/* Switch to bonus screen if all burgers are served. */
	if (allBurgersServed())
		ChangeGameScreen(GAME_SCREEN_LEVEL_BONUS);
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
	/* Animate "Hurry!" sign, leave screen when animation is done. */
	if (animateHurry())
		ChangeGameScreen(GAME_SCREEN_LEVEL_PLAY);
}

void cleanupInGameHurryScreen(void) {
}


/*
 *  The bonus screen is showed when all burgers have been served.
 *  It shows the bonus animation on the level screen.
 */
void initInGameBonusScreen(void) {
	uint8_t i;

	/* Kick all opponents off-screen. */
	for (i=0;i<OPPONENT_MAX;i++)
		kickOpponent(i);

	/* Fast bonus counting. */
	GameScreenOptions&=~LEVEL_ITEM_OPTION_BONUS_MASK;

	/* Draw bonus items at all valid opponent start positions. */
	for (i=0;i<OPPONENT_START_POSITION_MAX;i++) {
		if (OpponentStartPosition[i].x != OPPONENT_START_POSITION_INVALID) {
			/* Draw a random bonus item. */
			if (fastrandom()%2)
				drawSoda(OpponentStartPosition[i].x-1,OpponentStartPosition[i].y-2);
			else
				drawFries(OpponentStartPosition[i].x-1,OpponentStartPosition[i].y-2);
		}
	}
}

void updateInGameBonusScreen(void) {
	uint8_t i;
	uint8_t directional_buttons_held;

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Check if player should move. */
	if ((GameScreenAnimationPhase & 1)) {
		/* Yes. Get held buttons. */
		directional_buttons_held=checkControllerButtonsHeld(0,BTN_DIRECTIONS);

		/* Select direction to move player. */
		selectPlayerDirection(directional_buttons_held);

		/* Move player into selected direction, if possible. */
		movePlayer(directional_buttons_held);

		/* Move all active opponents. */
		for (i=0;i<OPPONENT_MAX;i++)
			moveOpponent(i);
	}

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Decrement bonus, win level when bonus is zero. */
	if (decrementBonus())
		ChangeGameScreen(GAME_SCREEN_LEVEL_WIN);
}

void cleanupInGameBonusScreen(void) {
}


/*
 *  The lose screen is showed when a life is lost.
 *  It shows the lose animation on the level screen.
 */
void initInGameLoseScreen(void) {
	/* Kick player from screen. */
	changePlayerDirection(PLAYER_FLAGS_DIRECTION_CAUGHT);
}

void updateInGameLoseScreen(void) {
	uint8_t i;

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Handle all opponents. */
	for (i=0;i<OPPONENT_MAX;i++) {
		/* Remove opponent if it is hit by a burger component. */
		removeOpponentIfHit(i);

		/* Select direction and move all active opponents. */
		selectOpponentDirection(i);
		moveOpponent(i);
	}

	/* Animate caught player. */
	if (animateCaughtPlayer()) {
		/* Animations done. Notice lost life. */
		Lives--;

		/* Re-initialize level or game over. */
		if (Lives)
				ChangeGameScreen(GAME_SCREEN_LEVEL_START);
			else
				ChangeGameScreen(GAME_SCREEN_LEVEL_GAME_OVER);
	}
}

void cleanupInGameLoseScreen(void) {
}


/*
 *  The in game game over screen is showed when all lives are lost.
 *  It shows an animation on the level screen.
 */
void initInGameOverScreen(void) {
	/* Let all burgers drop off-screen. */
	dropAllBurgersOffScreen();
}

void updateInGameOverScreen(void) {
	uint8_t o, b, i;

	/* Handle all opponents. */
	for (i=0;i<OPPONENT_MAX;i++) {
		/* Select direction and move all active opponents. */
		selectOpponentDirection(i);
		moveOpponent(i);
	}

	/* Burger drop animation. */
	b=animateBurgers();

	/* Decrement Bonus */
	o=decrementBonusFast();

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Change screen when burger drop animation is done, no bonus left and displayed score updated. */
	if (o && b == 0 && DisplayedScore == Score) {
		ChangeGameScreen(GAME_SCREEN_NEW_HIGHSCORE);
	}
}

void cleanupInGameOverScreen(void) {
	/* Unmap all opponents. */
	unmapOpponents();

	/* Fade out. */
	FadeOut(1,1);
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

