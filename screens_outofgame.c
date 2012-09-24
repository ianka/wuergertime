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


#include <uzebox.h>


/* Local includes. */
#include "screens.h"
#include "draw.h"


/* Fixed strings. */
const char TextDebug[] PROGMEM = "DEBUG";
const char TextWuergertime[] PROGMEM = "WUERGERTIME";
const char TextPressStart[] PROGMEM = "PRESS START";
const char TextPressStartBlink[] PROGMEM = "           ";
const char TextProgrammingAndArtwork[] PROGMEM = "PROGRAMMING AND ARTWORK";
const char TextByJanKandziora[] PROGMEM = "BY JAN KANDZIORA";
const char TextCopyleft[] PROGMEM = "@2012 JAN KANDZIORA";
const char TextLicense[] PROGMEM = "USE AND DISTRIBUTE UNDER THE";
const char TextGPL[] PROGMEM = "TERMS OF GNU GPL V3";


/*
 *  The debug screen is showed when all else fails.
 */
void initDebugScreen(void) {
	uint8_t x,y;

	FadeIn(1,0);
	clearScreen();
	Print(0,0,TextDebug);
	for(y=0;y<16;y++) {
		for(x=0;x<16;x++)
			SetTile(x+10,y,y*16+x);
	}
}

void updateDebugScreen(void) {
	if (GameScreenAnimationPhase>1000)
		ChangeGameScreen(GAME_SCREEN_START);
}

void cleanupDebugScreen(void) {
	FadeOut(1,1);
}


/*
 *  The start screen is showed immediately after the game starts
 *  and in rotation with credits, demo and highscores.
 */
void initStartScreen(void) {
	FadeIn(1,0);
	clearScreen();
	drawStringCentered(10,TextWuergertime)
	drawStringCentered(15,TextPressStart)
	drawFloor(0,19,SCREEN_WIDTH,0);
	drawStringCentered(21,TextProgrammingAndArtwork);
	drawStringCentered(22,TextByJanKandziora);
	drawStringCentered(24,TextCopyleft);
	drawStringCentered(25,TextLicense);
	drawStringCentered(26,TextGPL);
}

void updateStartScreen(void) {
	if (GameScreenAnimationPhase & 2) {
		drawStringCentered(15,TextPressStart)
	} else {
		drawStringCentered(15,TextPressStartBlink)
	}

	if (GameScreenAnimationPhase>100)
		ChangeGameScreen(GAME_SCREEN_DEBUG);
}

void cleanupStartScreen(void) {
	FadeOut(1,1);
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


