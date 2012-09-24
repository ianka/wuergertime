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



#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })



/*
 *  Tilesets
 */
#define TILESET_INGAME 0
#define TILESET_OUTOFGAME 1

/* Tileset switch */
uint8_t Tileset;

/* Change tileset. */
static inline void SetTileset(uint8_t tileset) {
	Tileset=tileset;
	SetTileTable(Tiles+(tileset?PIXELS_PER_TILE*UNIQUE_TILES_COUNT:0));
}


/*
 *  Draw game elements.
 */
void clearScreen(void) {
	uint8_t space[2]=TILES(SPACE);
	Fill(0,0,30,28,space[Tileset]);
}


#define DRAWABLE_LADDER           0xc0
#define DRAWABLE_LADDER_CONTINUED 0x20
#define DRAWABLE_LADDER_LENGTH    0x1f
#define DRAWABLE_FLOOR            0x00
#define DRAWABLE_FLOOR_CAP_LEFT   0x40 
#define DRAWABLE_FLOOR_CAP_RIGHT  0x20 
#define DRAWABLE_FLOOR_CAP_BOTH   ((DRAWABLE_FLOOR_CAP_LEFT|DRAWABLE_FLOOR_CAP_RIGHT))
#define DRAWABLE_FLOOR_LENGTH     0x1f 

typedef struct { uint8_t c, x, y; } PROGMEM drawable_t;

const drawable_t Levels[] PROGMEM={
	/* Level 0 */
	{ c: DRAWABLE_FLOOR|1, x:0, y:3 },
	{ c: DRAWABLE_FLOOR|DRAWABLE_FLOOR_CAP_LEFT|2, x:0, y:6 },
	{ c: DRAWABLE_FLOOR|3, x:0, y:9 },
	{ c: 0 },

	/* Level 1 */
	{ c: DRAWABLE_FLOOR|3, x:0, y:3 },
	{ c: DRAWABLE_FLOOR|DRAWABLE_FLOOR_CAP_BOTH|22, x:0, y:13 },
	{ c: DRAWABLE_FLOOR|10, x:5, y:9 },
	{ c: DRAWABLE_LADDER|5, x:10, y:9 },
	{ c: DRAWABLE_LADDER|DRAWABLE_LADDER_CONTINUED|10, x:10, y:13 },
	{ c: 0 },

	/* Level 2 */
	{ c: DRAWABLE_FLOOR|3, x:0, y:3 },
	{ c: DRAWABLE_FLOOR|1, x:0, y:6 },
	{ c: DRAWABLE_FLOOR|5, x:0, y:9 },
	{ c: 0 },
};





/* Draw a floor. */
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps) {
	tiles_trio_t floor[2]=TILES_COMPOUND(TILES_TRIO,FLOOR);

	/* Draw left cap if desired. */
	if (caps & DRAWABLE_FLOOR_CAP_LEFT) {
		SetTile(x,y,floor[Tileset].left);
		x++;
		length--;
	}

	/* End if already reache max length. */
	if (!length) return;

	/* Draw right cap if desired. */
	if (caps & DRAWABLE_FLOOR_CAP_LEFT) {
		length--;
		SetTile(x+length,y,floor[Tileset].right);
	}

	/* Draw a floor inbetween. */
	Fill(x,y,length,1,floor[Tileset].middle);
}


/* Draw a ladder */
void drawLadder(uint8_t x, uint8_t y, uint8_t length, uint8_t continued) {
	tiles_duo_t ladder[2]=TILES_COMPOUND(TILES_DUO,LADDER);
	tiles_duo_t ladder_top[2]=TILES_COMPOUND(TILES_DUO,LADDER_TOP);
	tiles_duo_t ladder_middle[2]=TILES_COMPOUND(TILES_DUO,LADDER_MIDDLE);
	tiles_duo_t ladder_bottom[2]=TILES_COMPOUND(TILES_DUO,LADDER_BOTTOM);

	/* Draw upper exit. */
	if (continued) {
		/* Middle exit for a continued ladder. */
		SetTile(x,y,ladder_middle[Tileset].left);
		SetTile(x+1,y,ladder_middle[Tileset].right);
	} else {
		/* Top exit for a starting ladder. */
		SetTile(x,y,ladder_top[Tileset].left);
		SetTile(x+1,y,ladder_top[Tileset].right);
	}	
	y++;
	length--;

	/* Draw lower exit. */
	length--;
	SetTile(x,y+length,ladder_bottom[Tileset].left);
	SetTile(x+1,y+length,ladder_bottom[Tileset].right);

	/* Draw a ladder inbetween. */
	Fill(x,y,1,length,ladder[Tileset].left);
	Fill(x+1,y,1,length,ladder[Tileset].right);
}



void drawSoda(uint8_t x, uint8_t y) {
	tiles_block_t soda[2]=TILES_COMPOUND(TILES_BLOCK,SODA);

	SetTile(x,y,soda[Tileset].upperleft);
	SetTile(x+1,y,soda[Tileset].upperright);
	SetTile(x,y+1,soda[Tileset].lowerleft);
	SetTile(x+1,y+1,soda[Tileset].lowerright);
}	


/* Draw a level. */
void drawLevel(uint8_t level, uint8_t length_tweak) {
	const drawable_t *p=Levels;
	uint8_t c, x, y, length;

	/* Skip to given level */
	while (level--) {
		while (pgm_read_byte(&(p->c))) p++;
		p++;
	}

	/* Draw level specific screen list. */
	while ((c=pgm_read_byte(&(p->c))) != 0) {
		/* Get coordinates */
		x=pgm_read_byte(&(p->x));
		y=pgm_read_byte(&(p->y));

		/* Check for type of drawable. */
		if ((c & DRAWABLE_LADDER) == DRAWABLE_LADDER) {
			/* Ladder. Draw it. */
			drawLadder(x,y,c & DRAWABLE_LADDER_LENGTH,c & DRAWABLE_LADDER_CONTINUED);
		} else {
			/* Floor. Honor the length tweak, e.g. for the level start animation. */
			length=min(c & DRAWABLE_FLOOR_LENGTH,length_tweak);

			/* Draw it. */
			drawFloor(x+((c & DRAWABLE_FLOOR_LENGTH)-length)/2,y,length,c & DRAWABLE_FLOOR_CAP_BOTH);
		}
		/* Next element in screen list. */
		p++;
	}	
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
uint8_t length_animation;
void initStartScreen(void) {
	/* Draw elements which are the same for each level. */
	clearScreen();
	length_animation=1;
}

void updateStartScreen(void) {
	drawLevel(1,length_animation);
	if (length_animation<31) length_animation++;
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
void initLevelScreen(uint8_t level) {
}

void updateLevelScreen(uint8_t level) {
}

void cleanupLevelScreen(uint8_t level) {
}


/*
 *  Main funcion
 */
int main(void) {
	/* Game-wide initialisation */

	/* Setup game-wide global variables. */
	Tileset=0;
	GameScreenPrevious=GAME_SCREEN_INVALID;
	GameScreen=GAME_SCREEN_START;

	/* Setup audio. */
//	InitMusicPlayer(patches);
//	StartSong(song_drmario_main);

	/* Setup video. */

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
			SetTileset(TILESET_OUTOFGAME);
			switch (GameScreen) {
				case GAME_SCREEN_DEBUG:         initDebugScreen(); break;
				case GAME_SCREEN_START:         initStartScreen(); break;
				case GAME_SCREEN_CREDITS:       initCreditsScreen(); break;
				case GAME_SCREEN_DEMO:          initDemoScreen(); break;
				case GAME_SCREEN_HIGHSCORES:    initHighscoresScreen(); break;
				case GAME_SCREEN_NEW_HIGHSCORE: initNewHighscoreScreen(); break;
				default:
					/* Initialize valid level only. */
					if (GameScreen<GAME_LEVELS) {
						/* Valid. In-game tileset is different. */
						SetTileset(TILESET_INGAME);
					
						/* Initialize level screen. */
						initLevelScreen(GameScreen);
					}
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
