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


/* Screen size. */
#define SCREEN_WIDTH  30
#define SCREEN_WRAP   30
#define SCREEN_HEIGHT 28


/* Center strings. */
#define drawStringCentered(y,string) Print(SCREEN_WIDTH/2-sizeof(string)/2,y,string);


void clearScreen(void);
void drawShape(uint8_t x, uint8_t y, const uint8_t *p);
void drawShapeAnimated(uint8_t x, uint8_t y, const uint8_t *p, uint8_t phase);
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps);
void drawLadder(uint8_t x, uint8_t y, uint8_t length, uint8_t options);
void drawBurgerComponent(uint8_t x, uint8_t yhalf, uint8_t component, uint8_t stomped, uint8_t buffer[2][5]);
void drawBurgerComponentTile(uint8_t index, uint8_t x, uint8_t yhalf, uint8_t component, uint8_t stomped, uint8_t buffer[2][5]);
void handleBurgerBackground(uint8_t x, int8_t half_y, uint8_t stomped, uint8_t buffer[2][5]);
void handleBurgerBackgroundTile(uint8_t index, uint8_t x, int8_t half_y, uint8_t stomped, uint8_t buffer[2][5]);
void drawSoda(uint8_t x, uint8_t y);
void drawFries(uint8_t x, uint8_t y);
void drawCrown(uint8_t x, uint8_t y);
void drawPlate(uint8_t x, uint8_t y);


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
