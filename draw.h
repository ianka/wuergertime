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


/* Local includes. */
#include "screens.h" /* for draw options */


/* Draw options. */
#define DRAW_OPTION_FLOOR_CAP_LEFT LEVEL_ITEM_FLOOR_CAP_LEFT
#define DRAW_OPTION_FLOOR_CAP_RIGHT LEVEL_ITEM_FLOOR_CAP_RIGHT


/* Shapes. */
extern const uint8_t ShapeSignTilesOutOfGame[] PROGMEM;
extern const uint8_t ShapeSignTilesInGame[] PROGMEM;
extern const uint8_t ShapeFoodTruck[] PROGMEM;


/* Screen size. */
#define SCREEN_WIDTH  30
#define SCREEN_HEIGHT 28


/* Center strings. */
#define drawStringCentered(y,string) Print(SCREEN_WIDTH/2-sizeof(string)/2,y,string);


void clearScreen(void);
void drawShape(uint8_t x, uint8_t y, const uint8_t *p);
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps);
void drawLadder(uint8_t x, uint8_t y, uint8_t length, uint8_t continued);
void drawSoda(uint8_t x, uint8_t y);
void drawFries(uint8_t x, uint8_t y);
void drawCrown(uint8_t x, uint8_t y);


#endif /* DRAW_H */
