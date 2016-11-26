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
#include "highscores.h"


/* Fixed strings. */
const char TextDebug[] PROGMEM = "DEBUG";
const char TextProgrammingAndArtwork[] PROGMEM = "PROGRAMMING AND ARTWORK";
const char TextByJanKandziora[] PROGMEM = "BY JAN KANDZIORA";
const char TextCopyleft[] PROGMEM = ";2012 JAN KANDZIORA";
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
	/* Reset game stats. */
	Score=0;
	Bonus=0;
	Lives=DEFAULT_LIVES;

	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);

	/* Draw picture */
	drawFloor(0,20,SCREEN_WIDTH,0);
	drawShape(9,1,ShapeSignOutOfGame,0);
	drawShape(7,6,ShapeFoodTruck,0);
	drawShape(3,0,ShapeHighscoreSignPoleRightLong,DRAW_OPTION_SHAPE_TILTED);
	drawShape(26,0,ShapeHighscoreSignPoleLeftLong,DRAW_OPTION_SHAPE_TILTED);
	drawLadder(4,-1,21,0);

	/* Draw credits and licensing strings. */
	drawStringCentered(21,TextProgrammingAndArtwork);
	drawStringCentered(22,TextByJanKandziora);
	drawStringCentered(24,TextCopyleft);
	drawStringCentered(25,TextLicense);
	drawStringCentered(26,TextGPL);
}

void updateStartScreen(void) {
	/* Animate open sign. */
	if (GameScreenAnimationPhase & 64) {
		SetTile(15,1,TILES1_OPEN_ON_LEFT);
		SetTile(16,1,TILES1_OPEN_ON_RIGHT);
	} else {
		SetTile(15,1,TILES1_OPEN_OFF_LEFT);
		SetTile(16,1,TILES1_OPEN_OFF_RIGHT);
	}

	/* Animate "Press Start". */
	if (GameScreenAnimationPhase & 8) {
		drawShape(15,10,ShapePressStart,0);
	} else {
		drawShape(15,10,ShapePressStartBlink,0);
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
			if (GameScreenAnimationPhase>500)
				ChangeGameScreen(GAME_SCREEN_HIGHSCORES);
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
	uint8_t i, y;
	uint32_t name, score;

	/* Draw highscores table. */
	clearScreen();
	drawHighscoreBillboard();
	drawLadder(4,24,4,0);

	/* Draw all entries. */
	for (i=0,y=HIGHSCORE_TOPMOST;i<HIGHSCORE_ENTRY_MAX;i++,y+=2) {
		readHighscoreEntry(i,&name,&score);
		drawHighscore(7,y,name,score);
	}

	/* Fade in and wait to complete */
	FadeIn(1,1);
}

void updateHighscoresScreen(void) {
	/* Check buttons. */
	switch (checkControllerButtonsPressed(0,BTN_START)) {
		case BTN_START:
			/* Change to level 0 as soon start is pressed. */
			selectLevel(0);
			ChangeGameScreen(GAME_SCREEN_LEVEL_PREPARE);
			break;
		default:
			/* Switch to start screen after a while. */
			if (GameScreenAnimationPhase>500)
				ChangeGameScreen(GAME_SCREEN_START);
	}
}

void cleanupHighscoresScreen(void) {
	/* Fade out and wait to complete */
	FadeOut(1,1);
}


/*
 *  The new highscore screen is showed after game over.
 */
void initNewHighscoreScreen(void) {
	uint8_t i, y, topped;
	uint32_t name, score;

	/* Draw highscores billboard. */
	clearScreen();
	drawHighscoreBillboard();

	/* Find out highscore topped. */
	topped=findToppedHighscoreEntry(Score);

	/* Any highscore topped? */
	if (topped != HIGHSCORE_ENTRY_MAX) {
		/* Yes. Draw all entries, move topped entries. */
		for (i=0,y=HIGHSCORE_TOPMOST;i<topped;i++,y+=2) {
			readHighscoreEntry(i,&name,&score);
			drawHighscore(7,y,name,score);
		}
		drawHighscore(7,y,0,Score);
		for (y+=2;i<HIGHSCORE_ENTRY_MAX-1;i++,y+=2) {
			readHighscoreEntry(i,&name,&score);
			drawHighscore(7,y,name,score);
		}
	} else {
		/* No. Draw all entries. */
		for (i=0,y=HIGHSCORE_TOPMOST;i<HIGHSCORE_ENTRY_MAX;i++,y+=2) {
			readHighscoreEntry(i,&name,&score);
			drawHighscore(7,y,name,score);
		}
	}

	/* Save topped value to scratchpad. */
	Scratchpad=topped;

	/* Fade in and wait to complete */
	FadeIn(1,1);
}

void updateNewHighscoreScreen(void) {
	/* Animate floor. */
	if (GameScreenAnimationPhase < HIGHSCORE_FLOOR_WIDTH)
		drawFloor(2,HIGHSCORE_TOPMOST+2*Scratchpad+1,GameScreenAnimationPhase,DRAW_OPTION_FLOOR_CAP_LEFT|DRAW_OPTION_FLOOR_CAP_RIGHT);

	/* Animate ladder. */
	if (GameScreenAnimationPhase >= HIGHSCORE_FLOOR_WIDTH && GameScreenAnimationPhase < SCREEN_HEIGHT+HIGHSCORE_FLOOR_WIDTH-HIGHSCORE_TOPMOST-2*Scratchpad) {
		drawLadder(4,SCREEN_HEIGHT+HIGHSCORE_FLOOR_WIDTH-GameScreenAnimationPhase,GameScreenAnimationPhase-HIGHSCORE_FLOOR_WIDTH,DRAW_OPTION_LADDER_CONTINUED);
	}

	/* Check buttons. */
	switch (checkControllerButtonsPressed(0,BTN_START)) {
		case BTN_START:
			/* Switch to start screen when button is pressed. */
			ChangeGameScreen(GAME_SCREEN_START);
			break;
		default:
			/* Switch to start screen after a while. */
			if (GameScreenAnimationPhase>5000)
				ChangeGameScreen(GAME_SCREEN_START);
	}
}

void cleanupNewHighscoreScreen(void) {
	/* Fade out and wait to complete */
	FadeOut(1,1);
}
