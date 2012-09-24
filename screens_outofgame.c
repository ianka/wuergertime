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
 *  screen_outofgame.c - out of game screen init, update, cleanup functions.
 */


/* Local includes. */
#include "screens.h"
#include "draw.h"


/*
 *  The debug screen is showed when all else fails.
 */
void initDebugScreen(void) {
}

void updateDebugScreen(void) {
}

void cleanupDebugScreen(void) {
}


/*
 *  The start screen is showed immediately after the game starts
 *  and in rotation with credits, demo and highscores.
 */
void initStartScreen(void) {
	clearScreen();
	drawLevel(0,32);
}

void updateStartScreen(void) {
}

void cleanupStartScreen(void) {
}


/*
 *  The credits screen is showed in rotation with start, highscore and demo.
 */
void initCreditsScreen(void) {
}

void updateCreditsScreen(void) {
}

void cleanupCreditsScreen(void) {
}


/*
 *  The demo screen is showed in rotation with start, highscore and credits.
 */
void initDemoScreen(void) {
}

void updateDemoScreen(void) {
}

void cleanupDemoScreen(void) {
}


/*
 *  The highscores screen is showed in rotation with start, demo and credits.
 */
void initHighscoresScreen(void) {
}

void updateHighscoresScreen(void) {
}

void cleanupHighscoresScreen(void) {
}


/*
 *  The new highscore screen is showed when a new high score was reached.
 */
void initNewHighscoreScreen(void) {
}

void updateNewHighscoreScreen(void) {
}

void cleanupNewHighscoreScreen(void) {
}


