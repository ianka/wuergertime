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


/* Actual level data */
#include "data/levels.inc"
#ifndef START_AT_LEVEL
#define START_AT_LEVEL 1
#endif

/* Local includes. */
#include "screens.h"
#include "draw.h"
#include "tiles.h"
#include "controllers.h"
#include "highscores.h"
#include "player.h"
#include "sprites.h"


/* Fixed strings. */
const char TextProgrammingAndArtwork[] PROGMEM = "PROGRAMMING AND ARTWORK";
const char TextByJanKandziora[] PROGMEM = "BY JAN KANDZIORA";
const char TextLevelDesign[] PROGMEM = "LEVEL DESIGN \"" LEVEL_DESIGN "\"";
const char TextLevelAuthor[] PROGMEM = "BY " LEVEL_AUTHOR;
const char TextCopyleft[] PROGMEM = ";2012 JAN KANDZIORA";
const char TextLicense[] PROGMEM = "USE AND DISTRIBUTE UNDER THE";
const char TextGPL[] PROGMEM = "TERMS OF GNU GPL V3";
const char TextGameOver[] PROGMEM = "GAME OVER";
const char TextGameOverOff[] PROGMEM = "         ";


/*
 *  The start screen is showed immediately after the game starts
 *  and in rotation with credits, demo and highscores.
 */
void initStartScreen(void) {
	/* Reset game stats. */
	Score=0;
	Bonus=0;
	Lives=DEFAULT_LIVES;
	Peppers=DEFAULT_PEPPERS;

	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);

	/* Draw picture */
	drawFloor(0,20,SCREEN_WIDTH,DRAW_OPTION_FLOOR_FORCE_WRAP);
	drawShape(9,1,ShapeSignOutOfGame,0);
	drawShape(7,6,ShapeFoodTruck,0);
	drawShape(3,0,ShapeHighscoreSignPoleRightLong,DRAW_OPTION_SHAPE_TILTED);
	drawShape(26,0,ShapeHighscoreSignPoleLeftLong,DRAW_OPTION_SHAPE_TILTED);
	drawLadder(4,-1,21,0);

	/* Draw credits and licensing strings. */
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
		drawShape(14,10,ShapePressStart,0);
	} else {
		drawShape(14,10,ShapePressStartBlink,0);
	}

	/* Animate credits. */
	switch (GameScreenAnimationPhase & 511) {
		case 0:
			drawStringCentered(21,TextProgrammingAndArtwork);
			drawStringCentered(22,TextByJanKandziora);
			break;
		case 240:
		case 496:
			clearLine(21);
			clearLine(22);
			break;
		case 256:
			drawStringCentered(21,TextLevelDesign);
			drawStringCentered(22,TextLevelAuthor);
			break;
	}

	/* Check buttons. */
	switch (checkControllerButtonsPressed(0,BTN_NONDIRECTION)) {
		case BTN_A:
		case BTN_B:
		case BTN_X:
		case BTN_Y:
		case BTN_SR:
		case BTN_SL:
		case BTN_START:
		case BTN_SELECT:
			/* Change to level 1 as soon start is pressed. */
			selectLevel(START_AT_LEVEL);
			pushentropy(GameScreenAnimationPhase);
			ChangeGameScreen(GAME_SCREEN_LEVEL_DESCRIPTION);
			break;
		default:
			/* Switch to highscore screen after a while. */
			if (GameScreenAnimationPhase>1024)
				ChangeGameScreen(GAME_SCREEN_HIGHSCORES);
	}
}

void cleanupStartScreen(void) {
	/* Fade out and wait to complete */
	FadeOut(1,1);
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
	drawLadder(4,26,2,0);

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
	switch (checkControllerButtonsPressed(0,BTN_NONDIRECTION)) {
		case BTN_A:
		case BTN_B:
		case BTN_X:
		case BTN_Y:
		case BTN_SR:
		case BTN_SL:
		case BTN_START:
		case BTN_SELECT:
			/* Change to level 1 as soon start is pressed. */
			selectLevel(START_AT_LEVEL);
			pushentropy(GameScreenAnimationPhase);
			ChangeGameScreen(GAME_SCREEN_LEVEL_DESCRIPTION);
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
 *  The game over screen is shown after the in-game over screen.
 */
void initGameOverScreen(void) {
	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);
}


void scrollTileDown(uint8_t x, uint8_t y) {
	if (x>=9 && x<20 && y==14)
		setTile(x,y,getTile(x,y-3));
	else
		setTile(x,y,getTile(x,y-1));
}

void updateGameOverScreen(void) {
	uint8_t y;

	/* Add a random tiny burger, fries or soda. */
	if (fastrandom()%5)
		setTile((GameScreenAnimationPhase*7)%SCREEN_WIDTH,2,TILES1_SPACE);
	else
		drawRandomTinyBurger((GameScreenAnimationPhase*7)%SCREEN_WIDTH,2);

	/* Scroll a column down. */
	for (y=SCREEN_HEIGHT-1;y>2;y--) {
		scrollTileDown((GameScreenAnimationPhase*3)%SCREEN_WIDTH,y);
		scrollTileDown((GameScreenAnimationPhase*7)%SCREEN_WIDTH,y);
		scrollTileDown((GameScreenAnimationPhase*11)%SCREEN_WIDTH,y);
		scrollTileDown((GameScreenAnimationPhase*13)%SCREEN_WIDTH,y);
		scrollTileDown((GameScreenAnimationPhase*17)%SCREEN_WIDTH,y);
	}

	/* Draw score and reached level. */
	drawScore(GameScreenScorePosition.x,GameScreenScorePosition.y,DisplayedScore);
	drawLevel(GameScreenLevelPosition.x,GameScreenLevelPosition.y,Level);

	/* Draw "Game Over" text. */
	/* Animate "Ready for Heat Up". */
	if (GameScreenAnimationPhase & 32) {
		drawStringCentered(13,TextGameOver);
	} else {
		drawStringCentered(13,TextGameOverOff);
	}

	/* Check buttons. */
	switch (checkControllerButtonsPressed(0,BTN_NONDIRECTION)) {
		case BTN_A:
		case BTN_B:
		case BTN_X:
		case BTN_Y:
		case BTN_SR:
		case BTN_SL:
		case BTN_START:
		case BTN_SELECT:
			/* Switch to new highscore screen as soon a button is pressed. */
			ChangeGameScreen(GAME_SCREEN_NEW_HIGHSCORE);
			break;
	}
}

void cleanupGameOverScreen(void) {
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
		drawFloor(2,HIGHSCORE_TOPMOST+2*Scratchpad+1,GameScreenAnimationPhase,0);

	/* Animate ladder. */
	if (GameScreenAnimationPhase >= HIGHSCORE_FLOOR_WIDTH && GameScreenAnimationPhase < SCREEN_HEIGHT+HIGHSCORE_FLOOR_WIDTH-HIGHSCORE_TOPMOST-2*Scratchpad) {
		drawLadder(4,SCREEN_HEIGHT+HIGHSCORE_FLOOR_WIDTH-GameScreenAnimationPhase,GameScreenAnimationPhase-HIGHSCORE_FLOOR_WIDTH,DRAW_OPTION_LADDER_CONTINUED);
	}

	/* Animate cook. */
	if (GameScreenAnimationPhase == HIGHSCORE_COOK_ANIMATION_PHASE) {
		/* Setup cook start position and direction. */
		Player.flags=PLAYER_FLAGS_DIRECTION_UP|PLAYER_FLAGS_SPEED_NORMAL;
		placeSprite(Player.sprite,
			HIGHSCORE_COOK_START_POSITION_X*8+SPRITE_BORDER_WIDTH,
			HIGHSCORE_COOK_START_POSITION_Y*8,
			SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LADDER);
	}

	if (GameScreenAnimationPhase > HIGHSCORE_COOK_ANIMATION_PHASE) {
		/* Top of ladder reached? */
		if (getSpriteTileY(Player.sprite,-1) > (HIGHSCORE_TOPMOST+2*Scratchpad)) {
			/* No. Move cook upwards. */
			moveSprite(Player.sprite,0,-1);
		} else {
			/* Yes. Go right to first letter of name. */
			if (getSpriteTileX(Player.sprite,0) == HIGHSCORE_COOK_START_POSITION_X)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			/* Enter highscore if some existing highscore was topped. */
			if (Scratchpad != HIGHSCORE_ENTRY_MAX) {
				/* Move cook until end position reached. */
				if (getSpriteTileX(Player.sprite,0) < HIGHSCORE_COOK_END_POSITION_X)
					moveSprite(Player.sprite,1,0);
				else
					ChangeGameScreen(GAME_SCREEN_ENTER_HIGHSCORE);
			} else {
				/* Move cook until floor end reached. */
				if (getSpriteTileX(Player.sprite,0) < 14)
					moveSprite(Player.sprite,1,0);
				else
					ChangeGameScreen(GAME_SCREEN_ENTERED_HIGHSCORE);
			}
		}
	}
}

void cleanupNewHighscoreScreen(void) {
}


/*
 *  The enter highscore screen is showed after the new highscore screen.
 */
void initEnterHighscoreScreen(void) {
}

void updateEnterHighscoreScreen(void) {
	uint8_t x=getSpriteTileX(Player.sprite,8);
	uint8_t y=HIGHSCORE_TOPMOST+2*Scratchpad;

	/* Move cook to next half-tile position. */
	if ((getSpriteX(Player.sprite) % 8) != 2)
		moveSprite(Player.sprite,1,0);

	/* Check buttons. */
	switch (checkControllerButtonsPressed(0,BTN_ALL)) {
		case BTN_UP:
			/* Roll through alphabet backwards. */
			setTile(x,y,(((getTile(x,y)-1)-16) % 32)+FONT_ALPHA_TILE);
			break;
		case BTN_DOWN:
			/* Roll through alphabet forward. */
			setTile(x,y,(((getTile(x,y)+1)-16) % 32)+FONT_ALPHA_TILE);
			break;
		case BTN_RIGHT:
			/* Move to next position. */
			if (x < HIGHSCORE_COOK_END_POSITION_X+5)
				moveSprite(Player.sprite,1,0);
			else
				ChangeGameScreen(GAME_SCREEN_ENTERED_HIGHSCORE);
			break;
		case BTN_LEFT:
			break;
		case BTN_A:
		case BTN_B:
		case BTN_X:
		case BTN_Y:
		case BTN_SR:
		case BTN_SL:
		case BTN_START:
		case BTN_SELECT:
			/* Switch to next screen when start button is pressed. */
			ChangeGameScreen(GAME_SCREEN_ENTERED_HIGHSCORE);
			break;
		default:
			/* Switch to next screen after a while. */
			if (GameScreenAnimationPhase>5000)
				ChangeGameScreen(GAME_SCREEN_ENTERED_HIGHSCORE);
	}
}

void cleanupEnterHighscoreScreen(void) {
	uint8_t y=HIGHSCORE_TOPMOST+2*Scratchpad;
	uint8_t i;
	uint32_t name=0;

	/* Calculate name key from tiles. */
	for(i=0;i<5;i++) {
		name<<=5;
		name|=getTile(11-i,y)-SHARED_TILES_COUNT;
	}

	/* Replace highscore in EEPROM. */
	replaceHighscoreEntry(Scratchpad,name,Score);
}


/*
 *  The entered highscore screen is showed after the enter highscore screen.
 */
void initEnteredHighscoreScreen(void) {
	/* Let the cook go back. */
	changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);
}

void updateEnteredHighscoreScreen(void) {
	/* Go left to ladder. */
	if (getSpriteTileX(Player.sprite,7) > HIGHSCORE_COOK_START_POSITION_X)
		moveSprite(Player.sprite,-1,0);

	/* When on ladder, go downwards. */
	if (getSpriteTileX(Player.sprite,7) == HIGHSCORE_COOK_START_POSITION_X) {
		/* Change movement direction. */
		if (getSpriteTileY(Player.sprite,0) == (HIGHSCORE_TOPMOST+2*Scratchpad+1))
			changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);

		/* Move cook until end position reached. */
		/* Then switch to new highscore entry screen. */
		if (getSpriteTileY(Player.sprite,0) < (HIGHSCORE_COOK_START_POSITION_Y-1))
			moveSprite(Player.sprite,0,1);
		else
			ChangeGameScreen(GAME_SCREEN_START);
	}
}

void cleanupEnteredHighscoreScreen(void) {
	/* Fade out and wait to complete */
	FadeOut(1,1);

	/* Unmap the cook. */
	unmapSprite(Player.sprite);
}
