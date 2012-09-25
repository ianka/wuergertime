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


/* Screen size. */
#define SCREEN_WIDTH  30
#define SCREEN_HEIGHT 28


/* Center strings. */
#define drawStringCentered(y,string) Print(SCREEN_WIDTH/2-sizeof(string)/2,y,string);



/* Drawables in screen lists. */
#define DRAWABLE_LADDER           0xc0
#define DRAWABLE_LADDER_CONTINUED 0x20
#define DRAWABLE_LADDER_LENGTH    0x1f
#define DRAWABLE_FLOOR            0x00
#define DRAWABLE_FLOOR_CAP_LEFT   0x40 
#define DRAWABLE_FLOOR_CAP_RIGHT  0x20 
#define DRAWABLE_FLOOR_CAP_BOTH   ((DRAWABLE_FLOOR_CAP_LEFT|DRAWABLE_FLOOR_CAP_RIGHT))
#define DRAWABLE_FLOOR_LENGTH     0x1f 

typedef struct { uint8_t c, x, y; } PROGMEM drawable_t;

void clearScreen(void);
void drawShape(uint8_t x, uint8_t y, uint8_t *p);
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps);
void drawLevel(uint8_t level, uint8_t length_tweak);



#endif /* DRAW_H */
