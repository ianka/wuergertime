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
 *  draw.h - draw screen elements and screens
 */


#ifndef DRAW_H
#define DRAW_H


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */
#include <uzebox.h> /* for vram array. */


/* Local includes. */
#include "screens.h" /* for draw options */


/* Draw options. */
#define DRAW_OPTION_FLOOR_CAP_LEFT LEVEL_ITEM_FLOOR_CAP_LEFT
#define DRAW_OPTION_FLOOR_CAP_RIGHT LEVEL_ITEM_FLOOR_CAP_RIGHT
#define DRAW_OPTION_LADDER_CONTINUED LEVEL_ITEM_LADDER_CONTINUED
#define DRAW_OPTION_LADDER_UPONLY LEVEL_ITEM_LADDER_UPONLY
#define DRAW_OPTION_SHAPE_TILTED ((1<<0))


/* Burger shapes are selected by number. */
#define SHAPE_BURGER_BUNTOP 0
#define SHAPE_BURGER_TOMATO 1
#define SHAPE_BURGER_PATTY 2
#define SHAPE_BURGER_CHEESESALAD 3
#define SHAPE_BURGER_BUNBOTTOM 4


/* Other shapes are selected by pointer. */
extern const uint8_t ShapeSignInGame[] PROGMEM;
extern const uint8_t ShapeSignOutOfGame[] PROGMEM;
extern const uint8_t ShapeSignLevelStart[] PROGMEM;
extern const uint8_t ShapeFoodTruck[] PROGMEM;
extern const uint8_t ShapePressStart[] PROGMEM;
extern const uint8_t ShapePressStartBlink[] PROGMEM;
extern const uint8_t ShapeHighscoreSignTop[] PROGMEM;
extern const uint8_t ShapeHighscoreSignLeft[] PROGMEM;
extern const uint8_t ShapeHighscoreSignRight[] PROGMEM;
extern const uint8_t ShapeHighscoreSignBottom[] PROGMEM;
extern const uint8_t ShapeHighscoreSignBurger[] PROGMEM;
extern const uint8_t ShapeHighscoreSignPoleLeftShort[] PROGMEM;
extern const uint8_t ShapeHighscoreSignPoleRightShort[] PROGMEM;
extern const uint8_t ShapeHighscoreSignPoleLeftLong[] PROGMEM;
extern const uint8_t ShapeHighscoreSignPoleRightLong[] PROGMEM;
extern const uint8_t ShapeSignLevelDescriptionPost[] PROGMEM;
extern const uint8_t ShapeSignLevelDescriptionCheesesalad[] PROGMEM;
extern const uint8_t ShapeSignLevelDescriptionPatty[] PROGMEM;
extern const uint8_t ShapeSignLevelDescriptionTomato[] PROGMEM;
extern const uint8_t ShapeSignLevelDescriptionTop[] PROGMEM;

/* Screen size. */
#define SCREEN_WIDTH  30
#define SCREEN_WRAP   30
#define SCREEN_HEIGHT 28

/* Maximum number of cooks to draw for lives marker. */
#define LIVES_DRAW_MAX 7

/* Highscore screen constants. */
#define HIGHSCORE_TOPMOST 12
#define HIGHSCORE_FLOOR_WIDTH 14
#define HIGHSCORE_COOK_ANIMATION_PHASE 10
#define HIGHSCORE_COOK_START_POSITION_X 5
#define HIGHSCORE_COOK_START_POSITION_Y (SCREEN_HEIGHT+1)
#define HIGHSCORE_COOK_END_POSITION_X 6

/* Description screen constants. */
#define DESCRIPTION_COOK_START_POSITION_X 2
#define DESCRIPTION_COOK_START_POSITION_Y 20
#define DESCRIPTION_COOK_END_POSITION_X 28

/* Center strings. */
#define drawStringCentered(y,string) Print(SCREEN_WIDTH/2-sizeof(string)/2,y,string);


void clearLine(uint8_t y);
void clearQuad(uint8_t x, uint8_t y);
void clearScreen(void);
void drawShape(uint8_t x, uint8_t y, const uint8_t *p, uint8_t options);
void drawShapeAnimated(uint8_t x, uint8_t y, const uint8_t *p, uint8_t phase);
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps);
void drawLadder(uint8_t x, uint8_t y, uint8_t length, uint8_t options);
void drawSquirtedLadderPiece(uint8_t x, uint8_t y);
void drawCleanedLadderPiece(uint8_t x, uint8_t y);
void drawWailedLadderPiece(uint8_t x, uint8_t y);
void drawBurgerComponent(uint8_t x, uint8_t yhalf, uint8_t component, uint8_t stomped, uint8_t buffer[2][5]);
void drawBurgerComponentTile(uint8_t index, uint8_t x, uint8_t yhalf, uint8_t component, uint8_t stomped, uint8_t buffer[2][5]);
void handleBurgerBackground(uint8_t x, int8_t half_y, uint8_t stomped, uint8_t buffer[2][5]);
void handleBurgerBackgroundTile(uint8_t index, uint8_t x, int8_t half_y, uint8_t stomped, uint8_t buffer[2][5]);
void drawSoda(uint8_t x, uint8_t y);
void drawFries(uint8_t x, uint8_t y);
void drawCrown(uint8_t x, uint8_t y);
void drawPlate(uint8_t x, uint8_t y);
void drawValue(uint8_t x, uint8_t y, uint8_t width, uint32_t value);
void drawScore(uint8_t x, uint8_t y, uint32_t value);
void drawLevel(uint8_t x, uint8_t y, uint16_t value);
void drawBonus(uint8_t x, uint8_t y, uint16_t value);
void drawLives(uint8_t x, uint8_t y, uint8_t value);
void drawHighscoreBillboard();
void drawHighscore(uint8_t x, uint8_t y, uint32_t name, uint32_t score);
void drawHurry(uint8_t x, uint8_t y, uint8_t buffer[3]);
void restoreHurryBackground(uint8_t x, uint8_t y, uint8_t buffer[3]);
void drawRandomTinyBurger(uint8_t x, uint8_t y);


/* Read VRAM and honor RAM tile offset. */
static inline uint8_t getTile(uint8_t x, uint8_t y) {
	return vram[x+y*SCREEN_WRAP]-RAM_TILES_COUNT;
}


/* Check for valid indices and set tile in VRAM. */
static inline void setTile(uint8_t x, uint8_t y, uint8_t tile) {
	if ((x>=SCREEN_WIDTH) || (y>=SCREEN_HEIGHT)) return;
	SetTile(x,y,tile);
}


#endif /* DRAW_H */
