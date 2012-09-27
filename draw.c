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


#include <avr/io.h> /* for uint8_t */
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
	uint8_t width, height, sx, sy, t;

	/* Get width and height of shape. */
	width=pgm_read_byte(p);
	p++;
	height=pgm_read_byte(p);
	p++;

	/* Draw lines. */
	for (sy=0;sy<height;sy++)
		for (sx=0;sx<width;sx++) {
			t=pgm_read_byte(p);
			p++;

			/* Honor skip tiles. */
			if ((Tileset==0 && t!=TILES0_SKIP) || (Tileset==1 && t!=TILES1_SKIP))
				SetTile(x+sx,y+sy,t);
		}
}


/* Draw an arbitrary shape with some animation on screen */
/* TODO: So far only skipping the upper part is implemented. */
void drawShapeAnimated(uint8_t x, uint8_t y, const uint8_t *p, uint8_t phase) {
	uint8_t width, height, sx, sy, t;

	/* Get width and height of shape. */
	width=pgm_read_byte(p);
	p++;
	height=pgm_read_byte(p);
	p++;

	/* Skip if full height was reached. */
	if (phase>height) return;

	/* Skip lines. */
	p+=(height-phase)*width;

	/* Draw lines. */
	for (sy=(height-phase);sy<height;sy++)
		for (sx=0;sx<width;sx++) {
			t=pgm_read_byte(p);
			p++;

			/* Honor skip tiles. */
			if ((Tileset==0 && t!=TILES0_SKIP) || (Tileset==1 && t!=TILES1_SKIP))
				SetTile(x+sx,y+sy,t);
		}
}


/* Draw a floor. */
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps) {
	tiles_trio_t floor[2]=TILES_COMPOUND(TILES_TRIO,FLOOR);

	/* Skip zero length floor. */
	if (!length) return;

	/* Draw left cap if desired. */
	if (caps & DRAW_OPTION_FLOOR_CAP_LEFT) {
		SetTile(x,y,floor[Tileset].left);
		x++;
		length--;
	}

	/* End if already reached max length. */
	if (!length) return;

	/* Draw right cap if desired. */
	if (caps & DRAW_OPTION_FLOOR_CAP_RIGHT) {
		length--;
		SetTile(x+length,y,floor[Tileset].right);
	}

	/* Draw a floor inbetween. */
	Fill(x,y,length,1,floor[Tileset].middle);
}


/* Draw a ladder */
void drawLadder(uint8_t x, uint8_t y, uint8_t length, uint8_t options) {
	tiles_duo_t ladder[2]=TILES_COMPOUND(TILES_DUO,LADDER);
	tiles_duo_t ladder_top[2]=TILES_COMPOUND(TILES_DUO,LADDER_TOP);
	tiles_duo_t ladder_middle[2]=TILES_COMPOUND(TILES_DUO,LADDER_MIDDLE);
	tiles_duo_t ladder_bottom[2]=TILES_COMPOUND(TILES_DUO,LADDER_BOTTOM);

	/* Skip zero length ladder. */
	if (!length) return;

	/* Ladder to bottom? */
	if (!(options & DRAW_OPTION_LADDER_TOBOTTOM)) {
		/* No. Draw upper exit. */
		if (options & DRAW_OPTION_LADDER_CONTINUED) {
			/* Middle exit for a continued ladder. */
			SetTile(x,y,ladder_middle[Tileset].left);
			SetTile(x+1,y,ladder_middle[Tileset].right);
		} else {
			/* Top exit for a starting ladder. */
			SetTile(x,y,ladder_top[Tileset].left);
			SetTile(x+1,y,ladder_top[Tileset].right);
		}

		/* Advance one row. */
		y++;
		length--;

		/* Draw lower exit. */
		SetTile(x,y+length,ladder_bottom[Tileset].left);
		SetTile(x+1,y+length,ladder_bottom[Tileset].right);
	}

	/* Draw a ladder inbetween. */
	Fill(x,y,1,length,ladder[Tileset].left);
	Fill(x+1,y,1,length,ladder[Tileset].right);
}


/* Draw a burger component. */
void drawBurgerComponent(uint8_t x, uint8_t yhalf, uint8_t component, uint8_t stomped) {
	uint8_t i, tiley, existing_component=SHAPE_BURGER_HALFTILE_AIR, upper_combination, lower_combination;

	/* Check current VRAM for lower tile. Has to be air or air combo. */
	if (Tileset == TILESET_INGAME) {
		/* Check in-game tile indices. */
		switch (getTile(x,(yhalf>>1)+1)) {
			case TILES0_SPACE:
				existing_component=SHAPE_BURGER_HALFTILE_AIR;
				break;
			case TILES0_BURGER_AIR_BUNTOP_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_BUNTOP;
				break;
			case TILES0_BURGER_AIR_TOMATO_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_TOMATO;
				break;
			case TILES0_BURGER_AIR_PATTY_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_PATTY;
				break;
			case TILES0_BURGER_AIR_CHEESESALAD_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_CHEESESALAD;
				break;
			case TILES0_BURGER_AIR_BUNBOTTOM_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_BUNBOTTOM;
				break;
			case TILES0_FLOOR_MIDDLE:
				existing_component=SHAPE_BURGER_HALFTILE_FLOOR;
				break;
		}
	} else {
		/* Check out-of-game tile indices. */
		switch (getTile(x,(yhalf>>1)+1)) {
			case TILES1_SPACE:
				existing_component=SHAPE_BURGER_HALFTILE_AIR;
				break;
			case TILES1_BURGER_AIR_BUNTOP_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_BUNTOP;
				break;
			case TILES1_BURGER_AIR_TOMATO_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_TOMATO;
				break;
			case TILES1_BURGER_AIR_PATTY_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_PATTY;
				break;
			case TILES1_BURGER_AIR_CHEESESALAD_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_CHEESESALAD;
				break;
			case TILES1_BURGER_AIR_BUNBOTTOM_LEFT:
				existing_component=SHAPE_BURGER_HALFTILE_BUNBOTTOM;
				break;
			case TILES1_FLOOR_MIDDLE:
				existing_component=SHAPE_BURGER_HALFTILE_FLOOR;
				break;
		}
	}

	/* Get index of new combination. */
	upper_combination=pgm_read_byte(&ShapeBurgersHalftiles[SHAPE_BURGER_HALFTILE_AIR][component+SHAPE_BURGER_HALFTILE_BUNTOP]);
	lower_combination=pgm_read_byte(&ShapeBurgersHalftiles[component+SHAPE_BURGER_HALFTILE_BUNTOP][existing_component]);

	/* Go through all burger component tiles in a row. */
	for (i=0;i<5;i++) {
		/* Add tile stomped value to tile y position. */
		tiley=yhalf+(stomped & 0x01);
		stomped>>=1;

		/* Now check for "half tile" yhalf value. */
		if (tiley & 0x01) {
			/* Two half tiles. Special handling for bun ends inside floor. */
			if (component == SHAPE_BURGER_BUNTOP) {
				if ((i==0) && (getTile(x,tiley>>1) == TILES0_FLOOR_MIDDLE)) {
					SetTile(x,tiley>>1,TILES0_BURGER_BUNTOP_INFLOOR_LEFT);
					SetTile(x,(tiley>>1)+1,pgm_read_byte(&ShapeBurgers[lower_combination][Tileset].left+i));

					/* Next tile. */
					continue;
				}
				if ((i==4) && (getTile(x+4,tiley>>1) == TILES0_FLOOR_MIDDLE)) {
					SetTile(x+i,tiley>>1,TILES0_BURGER_BUNTOP_INFLOOR_RIGHT);
					SetTile(x+i,(tiley>>1)+1,pgm_read_byte(&ShapeBurgers[lower_combination][Tileset].left+i));

					/* Next tile. */
					continue;
				}
			}	

			/* Usual half-tiles. */
			SetTile(x+i,(tiley>>1),pgm_read_byte(&ShapeBurgers[upper_combination][Tileset].left+i));
			SetTile(x+i,(tiley>>1)+1,pgm_read_byte(&ShapeBurgers[lower_combination][Tileset].left+i));
		} else {
			/* Full tile. Special handling for bun ends inside floor. */
			if (component == SHAPE_BURGER_BUNBOTTOM) {
				if ((i==0) && (getTile(x,tiley>>1) == TILES0_FLOOR_MIDDLE)) {
					SetTile(x,tiley>>1,TILES0_BURGER_BUNBOTTOM_INFLOOR_LEFT);

					/* Next tile. */
					continue;
				}
				if ((i==4) && (getTile(x+4,tiley>>1) == TILES0_FLOOR_MIDDLE)) {
					SetTile(x+i,tiley>>1,TILES0_BURGER_BUNBOTTOM_INFLOOR_RIGHT);

					/* Next tile. */
					continue;
				}
			}
			
			/* Not a special case. */
			SetTile(x+i,tiley>>1,pgm_read_byte(&ShapeBurgers[component][Tileset].left+i));
		}	
	}
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


/* Draw plate. */
void drawPlate(uint8_t x, uint8_t y) {
	tiles_burger_t plate[2]=TILES_COMPOUND(TILES_BURGER,PLATE);

	SetTile(x,y,plate[Tileset].left);
	SetTile(x+1,y,plate[Tileset].middleleft);
	SetTile(x+2,y,plate[Tileset].middle);
	SetTile(x+3,y,plate[Tileset].middleright);
	SetTile(x+4,y,plate[Tileset].right);
}

