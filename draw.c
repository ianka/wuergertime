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
 *  draw.c - draw screen elements and screens
 */


#include <avr/io.h> /* for uint8_t and uint16_t */
#include <avr/pgmspace.h> /* for PROGMEM */


/* Local includes. */
#include "draw.h"
#include "tiles.h"
#include "screens.h"


/* 
 * Burger shapes.
 *
 * Available half-tile combinations in tileset.
 *
 *        THIS|
 * OVER       | AIR BUNTOP TOMATO PATTY CHEESESALAD BUNBOTTOM FLOOR
 * -----------+-----------------------------------------------------
 * AIR        | (*)   *      *      *        *          *       .
 * BUNTOP     |  *    .      .      .        .          .       .    
 * TOMATO     |  *    *      .      .        .          .       .
 * PATTY      |  *    *      *      .        .          .       .
 * CHEESESALAD|  *    .      .      *        .          .       .
 * BUNBOTTOM  |  *    .      .      *        *          .       .
 * FLOOR      | (*)   *      *      *        *          *       .
 *
 */

/* Burger shapes are selected by number. */
#define SHAPE_BURGER_BUNTOP 0
#define SHAPE_BURGER_TOMATO 1
#define SHAPE_BURGER_PATTY 2
#define SHAPE_BURGER_CHEESESALAD 3
#define SHAPE_BURGER_BUNBOTTOM 4




/* Half-tiles. */
#define SHAPE_BURGER_HALFTILE_AIR         0
#define SHAPE_BURGER_HALFTILE_BUNTOP      1
#define SHAPE_BURGER_HALFTILE_TOMATO      2
#define SHAPE_BURGER_HALFTILE_PATTY       3
#define SHAPE_BURGER_HALFTILE_CHEESESALAD 4
#define SHAPE_BURGER_HALFTILE_BUNBOTTOM   5
#define SHAPE_BURGER_HALFTILE_FLOOR       6
#define SHAPE_BURGER_HALFTILE_DUMMY       7


/* Index table for half-tile combinations. */
const uint8_t ShapeBurgersHalftiles[6][8] PROGMEM={
	{ 0xfe,    5,    7,    9,   11,   13, 0xfd, 0xff },
	{    6, 0xff,   15,   19, 0xff, 0xff,   21, 0xff },
	{    8, 0xff, 0xff,   16, 0xff, 0xff,   22, 0xff },
	{   10, 0xff, 0xff, 0xff,   17,   20,   23, 0xff },
	{   12, 0xff, 0xff, 0xff, 0xff,   18,   24, 0xff },
	{   14, 0xff, 0xff, 0xff, 0xff, 0xff,   25, 0xff },
};


/* Burger shapes. */
const tiles_burger_t ShapeBurgers[26][2] PROGMEM={
	TILES_COMPOUND(TILES_BURGER,BURGER_BUNTOP),
	TILES_COMPOUND(TILES_BURGER,BURGER_TOMATO),
	TILES_COMPOUND(TILES_BURGER,BURGER_PATTY),
	TILES_COMPOUND(TILES_BURGER,BURGER_CHEESESALAD),
	TILES_COMPOUND(TILES_BURGER,BURGER_BUNBOTTOM),

	TILES_COMPOUND(TILES_BURGER,BURGER_AIR_BUNTOP),
	TILES_COMPOUND(TILES_BURGER,BURGER_BUNTOP_AIR),
	TILES_COMPOUND(TILES_BURGER,BURGER_AIR_TOMATO),
	TILES_COMPOUND(TILES_BURGER,BURGER_TOMATO_AIR),
	TILES_COMPOUND(TILES_BURGER,BURGER_AIR_PATTY),

	TILES_COMPOUND(TILES_BURGER,BURGER_PATTY_AIR),
	TILES_COMPOUND(TILES_BURGER,BURGER_AIR_CHEESESALAD),
	TILES_COMPOUND(TILES_BURGER,BURGER_CHEESESALAD_AIR),
	TILES_COMPOUND(TILES_BURGER,BURGER_AIR_BUNBOTTOM),
	TILES_COMPOUND(TILES_BURGER,BURGER_BUNBOTTOM_AIR),

	TILES_COMPOUND(TILES_BURGER,BURGER_BUNTOP_TOMATO),
	TILES_COMPOUND(TILES_BURGER,BURGER_TOMATO_PATTY),
	TILES_COMPOUND(TILES_BURGER,BURGER_PATTY_CHEESESALAD),
	TILES_COMPOUND(TILES_BURGER,BURGER_CHEESESALAD_BUNBOTTOM),
	TILES_COMPOUND(TILES_BURGER,BURGER_BUNTOP_PATTY),

	TILES_COMPOUND(TILES_BURGER,BURGER_PATTY_BUNBOTTOM),
	TILES_COMPOUND(TILES_BURGER,BURGER_BUNTOP_FLOOR),
	TILES_COMPOUND(TILES_BURGER,BURGER_TOMATO_FLOOR),
	TILES_COMPOUND(TILES_BURGER,BURGER_PATTY_FLOOR),
	TILES_COMPOUND(TILES_BURGER,BURGER_CHEESESALAD_FLOOR),

	TILES_COMPOUND(TILES_BURGER,BURGER_BUNBOTTOM_FLOOR),
};


/* Other shape data. */
#include "data/shapes.inc"


/* Clear screen. */
void clearScreen(void) {
	uint8_t space[2]=TILES(SPACE);
	Fill(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,space[Tileset]);
}


/* Draw an arbitrary shape on screen */
void drawShape(uint8_t x, uint8_t y, const uint8_t *p) {
	uint8_t width, height, sx, sy;

	/* Get width and height of shape. */
	width=pgm_read_byte(p);
	p++;
	height=pgm_read_byte(p);
	p++;

	/* Draw lines. */
	for (sy=0;sy<height;sy++)
		for (sx=0;sx<width;sx++) {
			SetTile(x+sx,y+sy,pgm_read_byte(p));
			p++;
		}
}


/* Draw a floor. */
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps) {
	tiles_trio_t floor[2]=TILES_COMPOUND(TILES_TRIO,FLOOR);

	/* Draw left cap if desired. */
	if (caps & DRAW_OPTION_FLOOR_CAP_LEFT) {
		SetTile(x,y,floor[Tileset].left);
		x++;
		length--;
	}

	/* End if already reache max length. */
	if (!length) return;

	/* Draw right cap if desired. */
	if (caps & DRAW_OPTION_FLOOR_CAP_LEFT) {
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


/* Draw a burger component. */
void drawBurgerComponent(uint8_t x, uint8_t yhalf, uint8_t component, uint16_t stomped) {
	uint8_t i, tiley;

	/* Go through all burger component tiles in a row. */
	for (i=0;i<5;i++) {
		/* Add tile stomped value to tile y position. */
		tiley=yhalf+(stomped & 0x03);
		stomped>>=2;

		/* Now check for "half tile" yhalf value. */
		if (tiley & 0x01) {
			/* Half tile. Must arrange overlay. */


		} else {
			/* Full tile. Easy! */
			SetTile(x+i,tiley>>1,pgm_read_byte(&ShapeBurgers[component][Tileset].left+i));
		}	
	}



//	const uint8_t *p = &ShapeBurgers[component][Tileset].left;


	/* Shortcut, actually this is left, middleleft, middle, middleright, right. */
/*	SetTile(x,y,pgm_read_byte(p)); x++; p++;
	SetTile(x,y,pgm_read_byte(p)); x++; p++;
	SetTile(x,y,pgm_read_byte(p)); x++; p++;
	SetTile(x,y,pgm_read_byte(p)); x++; p++;
	SetTile(x,y,pgm_read_byte(p)); x++; p++;*/
}


/* Draw a soda can. */
void drawSoda(uint8_t x, uint8_t y) {
	tiles_block_t soda[2]=TILES_COMPOUND(TILES_BLOCK,SODA);

	SetTile(x,y,soda[Tileset].upperleft);
	SetTile(x+1,y,soda[Tileset].upperright);
	SetTile(x,y+1,soda[Tileset].lowerleft);
	SetTile(x+1,y+1,soda[Tileset].lowerright);
}	


/* Draw fries packet. */
void drawFries(uint8_t x, uint8_t y) {
	tiles_block_t fries[2]=TILES_COMPOUND(TILES_BLOCK,FRIES);

	SetTile(x,y,fries[Tileset].upperleft);
	SetTile(x+1,y,fries[Tileset].upperright);
	SetTile(x,y+1,fries[Tileset].lowerleft);
	SetTile(x+1,y+1,fries[Tileset].lowerright);
}


/* Draw crown. */
void drawCrown(uint8_t x, uint8_t y) {
	tiles_block_t crown[2]=TILES_COMPOUND(TILES_BLOCK,CROWN);

	SetTile(x,y,crown[Tileset].upperleft);
	SetTile(x+1,y,crown[Tileset].upperright);
	SetTile(x,y+1,crown[Tileset].lowerleft);
	SetTile(x+1,y+1,crown[Tileset].lowerright);
}

