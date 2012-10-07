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
 *  screen_outofgame.c - out of game screen init, update, cleanup functions.
 */


#include <uzebox.h>


/* Local includes. */
#include "screens.h"
#include "draw.h"
#include "tiles.h"
#include "controllers.h"


/* Fixed strings. */
const char TextDebug[] PROGMEM = "DEBUG";
const char TextPress[] PROGMEM = "PRESS";
const char TextStart[] PROGMEM = "START";
const char TextPressStartBlink[] PROGMEM = "     ";
const char TextProgrammingAndArtwork[] PROGMEM = "PROGRAMMING AND ARTWORK";
const char TextByJanKandziora[] PROGMEM = "BY JAN KANDZIORA";
const char TextCopyleft[] PROGMEM = "@2012 JAN KANDZIORA";
const char TextLicense[] PROGMEM = "USE AND DISTRIBUTE UNDER THE";
const char TextGPL[] PROGMEM = "TERMS OF GNU GPL V3";


/*
 *  The debug screen is showed when all else fails.
 */
void initDebugScreen(void) {
	uint8_t x, y;

	FadeIn(1,0);
	clearScreen();
	Print(0,0,TextDebug);
	for(y=0;y<16;y++) {
		for(x=0;x<16;x++)
			SetTile(x+10,y,y*16+x);
	}
}

void updateDebugScreen(void) {
	if (GameScreenAnimationPhase>100)
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
	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);

	/* Draw picture */
	drawFloor(0,20,SCREEN_WIDTH,0);
	drawShape(7,1,ShapeSignOutOfGame);
	drawShape(5,6,ShapeFoodTruck);

	/* Draw credits and licensing strings. */
	drawStringCentered(21,TextProgrammingAndArtwork);
	drawStringCentered(22,TextByJanKandziora);
	drawStringCentered(24,TextCopyleft);
	drawStringCentered(25,TextLicense);
	drawStringCentered(26,TextGPL);
}

void updateStartScreen(void) {
	/* Animate open sign. */
	if (GameScreenAnimationPhase & 32) {
		SetTile(13,1,TILES1_OPEN_ON_LEFT);
		SetTile(14,1,TILES1_OPEN_ON_RIGHT);
	} else {
		SetTile(13,1,TILES1_OPEN_OFF_LEFT);
		SetTile(14,1,TILES1_OPEN_OFF_RIGHT);
	}

	/* Animate "Press Start". */
	if (GameScreenAnimationPhase & 4) {
		Print(23,10,TextPress);
		Print(23,12,TextStart);
	} else {
		Print(23,10,TextPressStartBlink);
		Print(23,12,TextPressStartBlink);
	}

	/* Check buttons. */
	switch (checkControllerButtonsPressed(0,BTN_START)) {
		case BTN_START:
			/* Change to level 0 as soon start is pressed. */
			selectLevel(0);
			ChangeGameScreen(GAME_SCREEN_LEVEL_PREPARE);
			break;
		default:
			/* Switch to highscore screen after a while. */
			if (GameScreenAnimationPhase>100)
				ChangeGameScreen(GAME_SCREEN_DEBUG);
	}
}

void cleanupStartScreen(void) {
	/* Fade out and wait to complete */
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


