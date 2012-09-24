/*
 *  Würgertime - An Hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/janka/wuergertime
 *
 */

#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <math.h>
#include <stdio.h>


/* Sound effects */
#include "data/drmario_music.inc"
#include "data/patches.inc"

/* Tiles */
#include "data/tiles.inc"
#include "data/tiles0.inc"





/*
 *
 */




/*
 *  Drawing game elements.
 */
void clearScreen(void) {
	uint8_t space[2]=TILES(SPACE);
	Fill(0, 0, 29, 27, space[0]);
}

void drawFloor(uint8_t x, uint8_t y, uint8_t length) {
	uint8_t floor[2]=TILES(FLOOR);
	Fill(x,y,length,1,floor[0]);
}

void drawSoda(uint8_t x, uint8_t y) {
	tiles_block_t soda[2]=TILES_COMPOUND(TILES_BLOCK,SODA);
	SetTile(x,y,soda[0].ul);
	SetTile(x+1,y,soda[0].ur);
	SetTile(x,y+1,soda[0].ll);
	SetTile(x+1,y+1,soda[0].lr);
}	


/*
 *  Game screens.
 *
 *  All screen numbers below level count are levels,
 *  all others not listed here are invalid, too.
 */
#define GAME_LEVELS 100

#define GAME_SCREEN_INVALID       0xff 
#define GAME_SCREEN_DEBUG         0xf0 
#define GAME_SCREEN_START         0xf1 
#define GAME_SCREEN_CREDITS       0xf2
#define GAME_SCREEN_DEMO          0xf3
#define GAME_SCREEN_HIGHSCORES    0xf4
#define GAME_SCREEN_NEW_HIGHSCORE 0xf5


/* Game screen switch. */ 
uint8_t GameScreenPrevious;
uint8_t GameScreen;



/* Switch to new game screen. */
static inline void ChangeGameScreen(uint8_t screen) {
	GameScreen=screen;
}



/* Debug screen */
void initDebugScreen(void) {
}

void updateDebugScreen(void) {
}

void cleanupDebugScreen(void) {
}


/* Start screen */
void initStartScreen(void) {
	clearScreen();
	drawFloor(5,5,5);
	drawFloor(5,10,15);
	drawSoda(7,8);
}

void updateStartScreen(void) {
}

void cleanupStartScreen(void) {
}


/* Credits screen */
void initCreditsScreen(void) {
}

void updateCreditsScreen(void) {
}

void cleanupCreditsScreen(void) {
}


/* Demo screen */
void initDemoScreen(void) {
}

void updateDemoScreen(void) {
}

void cleanupDemoScreen(void) {
}


/* Highscores screen */
void initHighscoresScreen(void) {
}

void updateHighscoresScreen(void) {
}

void cleanupHighscoresScreen(void) {
}


/* New highscore screen */
void initNewHighscoreScreen() {
}

void updateNewHighscoreScreen() {
}

void cleanupNewHighscoreScreen() {
}


/* Level screens */
void initLevelScreen(int level) {
}

void updateLevelScreen(int level) {
}

void cleanupLevelScreen(int level) {
}


/*
 *  Main funcion
 */
int main(void) {
	/* Game-wide initialisation */

	/* Setup game-wide global variables. */
	GameScreenPrevious=GAME_SCREEN_INVALID;
	GameScreen=GAME_SCREEN_START;

	/* Setup audio. */
//	InitMusicPlayer(patches);
//	StartSong(song_drmario_main);

	/* Setup video. */
	SetTileTable(Tiles);
//	SetTileTable(Tiles+64*UNIQUE_TILES_COUNT);


	/* Main loop */
	for (;;) {
		/* Check for screen change. */
		if (GameScreen != GameScreenPrevious) {
			/* Changed. Cleanup previous screen. */
			switch (GameScreenPrevious) {
				case GAME_SCREEN_DEBUG:         cleanupDebugScreen(); break;
				case GAME_SCREEN_START:         cleanupStartScreen(); break;
				case GAME_SCREEN_CREDITS:       cleanupCreditsScreen(); break;
				case GAME_SCREEN_DEMO:          cleanupDemoScreen(); break;
				case GAME_SCREEN_HIGHSCORES:    cleanupHighscoresScreen(); break;
				case GAME_SCREEN_NEW_HIGHSCORE: cleanupNewHighscoreScreen(); break;
				default:
					/* Cleanup valid level only. */
					if (GameScreenPrevious<GAME_LEVELS)
						/* Valid. */
						cleanupLevelScreen(GameScreenPrevious);
			}

			/* Initialize new screen. */
			switch (GameScreen) {
				case GAME_SCREEN_DEBUG:         initDebugScreen(); break;
				case GAME_SCREEN_START:         initStartScreen(); break;
				case GAME_SCREEN_CREDITS:       initCreditsScreen(); break;
				case GAME_SCREEN_DEMO:          initDemoScreen(); break;
				case GAME_SCREEN_HIGHSCORES:    initHighscoresScreen(); break;
				case GAME_SCREEN_NEW_HIGHSCORE: initNewHighscoreScreen(); break;
				default:
					/* Initialize valid level only. */
					if (GameScreen<GAME_LEVELS)
						/* Valid. */
						initLevelScreen(GameScreen);
			}

			/* Screen change done. */
			GameScreenPrevious=GameScreen;
		}

		/* Select by game screen */
		switch (GameScreen) {
			case GAME_SCREEN_DEBUG:         updateDebugScreen(); break;
			case GAME_SCREEN_START:         updateStartScreen(); break;
			case GAME_SCREEN_CREDITS:       updateCreditsScreen(); break;
			case GAME_SCREEN_DEMO:          updateDemoScreen(); break;
			case GAME_SCREEN_HIGHSCORES:    updateHighscoresScreen(); break;
			case GAME_SCREEN_NEW_HIGHSCORE: updateNewHighscoreScreen(); break;
			default:
				/* Check for valid level. */
				if (GameScreen<GAME_LEVELS)
					/* Valid. */
					updateLevelScreen(GameScreen);
				else
					/* Invalid. Change to debug screen. */
					ChangeGameScreen(GAME_SCREEN_DEBUG);
		}

		/* Wait for next frame (let the interrupt kernel work). */
		WaitVsync(1);
	}

	/* Never reached. */
	return 0;
}
