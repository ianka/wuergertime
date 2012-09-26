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


/* Local includes. */
#include "draw.h"
#include "tiles.h"
#include "screens.h"


/* Shape data. */
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

