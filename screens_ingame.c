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


/*
 *  The prepare screen is showed when a new level is reached.
 *    + Score displays
 *    + Opening animation of floors and burgers 
 *    + Burger positions are being reset
 */
void initInGamePrepareScreen(void) {
	/* Select level description by number. */
	selectLevel(GameScreen & ~GAME_SCREEN_LEVEL_MASK);

	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);
}

void updateInGamePrepareScreen(void) {
	/* Prepare level by description. */
	prepareLevel();
}

void cleanupInGamePrepareScreen(void) {
}


/*
 *  The start screen is showed after prepare screen or after a life is lost.
 *     + Cook and Monsters reset
 *     + Bonus reset
 */
void initInGameStartScreen(void) {
}

void updateInGameStartScreen(void) {
}

void cleanupInGameStartScreen(void) {
}


/*
 *  The play screen is showed when the actual game is happening.
 */
void initInGamePlayScreen(void) {
}

void updateInGamePlayScreen(void) {
}

void cleanupInGamePlayScreen(void) {
}



/*
 *  The lose screen is showed when a life is lost.
 */
void initInGameLoseScreen(void) {
}

void updateInGameLoseScreen(void) {
}

void cleanupInGameLoseScreen(void) {
}


/*
 *  The hurry screen is showed when bonus counter
 *  reaches zero before all burgers are done.
 */
void initInGameHurryScreen(void) {
}

void updateInGameHurryScreen(void) {
}

void cleanupInGameHurryScreen(void) {
}


/*
 *  The bonus screen is showed when a bonus item was collected.
 */
void initInGameBonusScreen(void) {
}

void updateInGameBonusScreen(void) {
}

void cleanupInGameBonusScreen(void) {
}


/*
 * The win screen is showed when a level is completed.
 */
void initInGameWinScreen(void) {
}

void updateInGameWinScreen(void) {
}

void cleanupInGameWinScreen(void) {
}


