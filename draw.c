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
 * AIR        | (*)   *      *      *        *          *      (*)
 * BUNTOP     |  *    .      .      .        .          .       *
 * TOMATO     |  *    *      .      .        .          .       *
 * PATTY      |  *    *      *      .        .          .       *
 * CHEESESALAD|  *    .      .      *        .          .       *
 * BUNBOTTOM  |  *    .      .      *        *          .       *
 * FLOOR      |  .    .      .      .        .          .       .
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
const uint8_t ShapeBurgersHalftiles[8][8] PROGMEM={
	{ 0xfe,    5,    7,    9,   11,   13, 0xff, 0xff },
	{    6, 0xff,   15,   19, 0xff, 0xff, 0xff, 0xff },
	{    8, 0xff, 0xff,   16, 0xff, 0xff, 0xff, 0xff },
	{   10, 0xff, 0xff, 0xff,   17,   20, 0xff, 0xff },
	{   12, 0xff, 0xff, 0xff, 0xff,   18, 0xff, 0xff },
	{   14, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
	{ 0xfd,   21,   22,   23,   24,   25, 0xff, 0xff },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
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
	TILES_COMPOUND(TILES_BURGER,BURGER_FLOOR_BUNTOP),
	TILES_COMPOUND(TILES_BURGER,BURGER_FLOOR_TOMATO),
	TILES_COMPOUND(TILES_BURGER,BURGER_FLOOR_PATTY),
	TILES_COMPOUND(TILES_BURGER,BURGER_FLOOR_CHEESESALAD),

	TILES_COMPOUND(TILES_BURGER,BURGER_FLOOR_BUNBOTTOM),
};


/* Other shape data. */
#include "data/shapes.inc"


/* Clear screen. */
void clearScreen(void) {
	uint8_t space[2]=TILES(SPACE);
	Fill(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,space[Tileset]);
}


/* Draw an arbitrary shape on screen */
void drawShape(uint8_t x, uint8_t y, const uint8_t *p, uint8_t options) {
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
			if ((Tileset==0 && t!=TILES0_SKIP) || (Tileset==1 && t!=TILES1_SKIP)) {
				if (options & DRAW_OPTION_SHAPE_TILTED)
						setTile(x+sy,y+sx,t);
					else
						setTile(x+sx,y+sy,t);
			}
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
				setTile(x+sx,y+sy,t);
		}
}


/* Draw a floor. */
void drawFloor(uint8_t x, uint8_t y, uint8_t length, uint8_t caps) {
	tiles_trio_t floor[2]=TILES_COMPOUND(TILES_TRIO,FLOOR);

	/* Skip zero length floor. */
	if (!length) return;

	/* Draw left cap if desired. */
	if (caps & DRAW_OPTION_FLOOR_CAP_LEFT) {
		setTile(x,y,floor[Tileset].left);
		x++;
		length--;
	}

	/* End if already reached max length. */
	if (!length) return;

	/* Draw right cap if desired. */
	if (caps & DRAW_OPTION_FLOOR_CAP_RIGHT) {
		length--;
		setTile(x+length,y,floor[Tileset].right);
	}

	/* Draw a floor inbetween. */
	Fill(x,y,length,1,floor[Tileset].middle);
}


/* Draw a ladder */
void drawLadder(uint8_t x, uint8_t y, uint8_t length, uint8_t options) {
	uint8_t floor[2]=TILES(FLOOR_MIDDLE);
	tiles_duo_t ladder[2]=TILES_COMPOUND(TILES_DUO,LADDER);
	tiles_duo_t ladder_top[2]=TILES_COMPOUND(TILES_DUO,LADDER_TOP);
	tiles_duo_t ladder_top_floorend[2]=TILES_COMPOUND(TILES_DUO,LADDER_TOP_FLOOREND);
	tiles_duo_t ladder_top_uponly[2]=TILES_COMPOUND(TILES_DUO,LADDER_TOP_UPONLY);
	tiles_duo_t ladder_bottom[2]=TILES_COMPOUND(TILES_DUO,LADDER_BOTTOM);
	tiles_duo_t ladder_bottom_floorend[2]=TILES_COMPOUND(TILES_DUO,LADDER_BOTTOM_FLOOREND);

	/* Skip zero length ladder. */
	if (!length) return;

	/* Draw upper exit. */
	if (options & DRAW_OPTION_LADDER_UPONLY) {
		/* One-way ladder top. */
		setTile(x,y,ladder_top_uponly[Tileset].left);
		setTile(x+1,y,ladder_top_uponly[Tileset].right);
	} else {
		/* Normal ladder top. */
		/* Check if floor end where top is going to be placed. */
		if (getTile(x,y) == floor[Tileset])
				setTile(x,y,ladder_top[Tileset].left);
			else
				setTile(x,y,ladder_top_floorend[Tileset].left);

		if (getTile(x+1,y) == floor[Tileset])
				setTile(x+1,y,ladder_top[Tileset].right);
			else
				setTile(x+1,y,ladder_top_floorend[Tileset].right);
	}

	/* Advance one row. */
	y++;
	length--;

	/* Continued ladder? */
	if (!(options & DRAW_OPTION_LADDER_CONTINUED)) {
		/* No. Draw lower exit. */
		/* Check if floor end where bottom is going to be placed. */
		if (getTile(x,y+length) == floor[Tileset])
				setTile(x,y+length,ladder_bottom[Tileset].left);
			else
				setTile(x,y+length,ladder_bottom_floorend[Tileset].left);

		if (getTile(x+1,y+length) == floor[Tileset])
				setTile(x+1,y+length,ladder_bottom[Tileset].right);
			else
				setTile(x+1,y+length,ladder_bottom_floorend[Tileset].right);
	}

	/* Draw a ladder inbetween. */
	Fill(x,y,1,length,ladder[Tileset].left);
	Fill(x+1,y,1,length,ladder[Tileset].right);
}


/* Draw a burger component. */
void drawBurgerComponentTile(uint8_t index, uint8_t x, uint8_t half_y, uint8_t component, uint8_t stomped, uint8_t buffer[2][5]) {
	uint8_t tile_y, existing_component=SHAPE_BURGER_HALFTILE_AIR, upper_combination, lower_combination;

	/* Add tile stomped value to component y position. */
	tile_y=half_y+((stomped>>index) & 0x01);

	/* Check current VRAM for floor tile. */
	if (getTile(x+index,half_y>>1)==TILES0_FLOOR_MIDDLE) {
		/* Upper tile is combined with floor. */
		upper_combination=pgm_read_byte(&ShapeBurgersHalftiles[SHAPE_BURGER_HALFTILE_FLOOR][component+SHAPE_BURGER_HALFTILE_BUNTOP]);
	} else {
		/* Upper tile is combined with air. */
		upper_combination=pgm_read_byte(&ShapeBurgersHalftiles[SHAPE_BURGER_HALFTILE_AIR][component+SHAPE_BURGER_HALFTILE_BUNTOP]);
	}

	/* Check current VRAM for lower tile. Has to be air or air combo. */
	if (Tileset == TILESET0) {
		/* Check in-game tile indices. */
		switch (getTile(x+index,(half_y>>1)+1)) {
			case TILES0_SPACE:
				existing_component=SHAPE_BURGER_HALFTILE_AIR;
				break;
			case TILES0_BURGER_AIR_BUNTOP_LEFT:
			case TILES0_BURGER_AIR_BUNTOP_MIDDLELEFT:
			/* case TILES0_BURGER_AIR_BUNTOP_MIDDLE: same as TILES0_BURGER_AIR_BUNTOP_MIDDLELEFT */
			/* case TILES0_BURGER_AIR_BUNTOP_MIDDLERIGHT: same as TILES0_BURGER_AIR_BUNTOP_MIDDLELEFT */
			case TILES0_BURGER_AIR_BUNTOP_RIGHT:
				existing_component=SHAPE_BURGER_HALFTILE_BUNTOP;
				break;
			case TILES0_BURGER_AIR_TOMATO_LEFT:
			case TILES0_BURGER_AIR_TOMATO_MIDDLELEFT:
			case TILES0_BURGER_AIR_TOMATO_MIDDLE:
			/* case TILES0_BURGER_AIR_TOMATO_MIDDLERIGHT: same as TILES0_BURGER_AIR_TOMATO_MIDDLELEFT */
			case TILES0_BURGER_AIR_TOMATO_RIGHT:
				existing_component=SHAPE_BURGER_HALFTILE_TOMATO;
				break;
			case TILES0_BURGER_AIR_PATTY_LEFT:
			case TILES0_BURGER_AIR_PATTY_MIDDLELEFT:
			case TILES0_BURGER_AIR_PATTY_MIDDLE:
			case TILES0_BURGER_AIR_PATTY_MIDDLERIGHT:
			case TILES0_BURGER_AIR_PATTY_RIGHT:
				existing_component=SHAPE_BURGER_HALFTILE_PATTY;
				break;
			case TILES0_BURGER_AIR_CHEESESALAD_LEFT:
			case TILES0_BURGER_AIR_CHEESESALAD_MIDDLELEFT:
			/* case TILES0_BURGER_AIR_CHEESESALAD_MIDDLE: same as TILES0_BURGER_AIR_CHEESESALAD_MIDDLELEFT */
			/* case TILES0_BURGER_AIR_CHEESESALAD_MIDDLERIGHT: same as TILES0_BURGER_AIR_CHEESESALAD_MIDDLELEFT */
			case TILES0_BURGER_AIR_CHEESESALAD_RIGHT:
				existing_component=SHAPE_BURGER_HALFTILE_CHEESESALAD;
				break;
			case TILES0_BURGER_AIR_BUNBOTTOM_LEFT:
			case TILES0_BURGER_AIR_BUNBOTTOM_MIDDLELEFT:
			/* case TILES0_BURGER_AIR_BUNBOTTOM_MIDDLE: same as TILES0_BURGER_AIR_BUNBOTTOM_MIDDLELEFT */
			/* case TILES0_BURGER_AIR_BUNBOTTOM_MIDDLERIGHT: same as TILES0_BURGER_AIR_BUNBOTTOM_MIDDLELEFT */
			case TILES0_BURGER_AIR_BUNBOTTOM_RIGHT:
				existing_component=SHAPE_BURGER_HALFTILE_BUNBOTTOM;
				break;
		}
	} else {
	}

	/* Get index of new lower combination. */
	lower_combination=pgm_read_byte(&ShapeBurgersHalftiles[component+SHAPE_BURGER_HALFTILE_BUNTOP][existing_component]);

	/* Now check for "half tile" half_y value. */
	if (tile_y & 0x01) {
		/* Two half tiles. Special handling for bun ends inside floor. */
		if (component == SHAPE_BURGER_BUNBOTTOM) {
			if ((index==0) && (buffer[(tile_y>>1) & 0x01][index] == TILES0_FLOOR_MIDDLE)) {
				setTile(x+index,(tile_y>>1),pgm_read_byte(&ShapeBurgers[upper_combination][Tileset].left+index));
				setTile(x+index,(tile_y>>1)+1,TILES0_BURGER_BUNBOTTOM_INFLOOR_LEFT);

				/* Next tile. */
				return;
			}
			if ((index==4) && (buffer[(tile_y>>1) & 0x01][index] == TILES0_FLOOR_MIDDLE)) {
				setTile(x+index,(tile_y>>1),pgm_read_byte(&ShapeBurgers[upper_combination][Tileset].left+index));
				setTile(x+index,(tile_y>>1)+1,TILES0_BURGER_BUNBOTTOM_INFLOOR_RIGHT);

				/* Next tile. */
				return;
			}
		}

		/* Usual half-tiles. */
		setTile(x+index,(tile_y>>1),pgm_read_byte(&ShapeBurgers[upper_combination][Tileset].left+index));
		setTile(x+index,(tile_y>>1)+1,pgm_read_byte(&ShapeBurgers[lower_combination][Tileset].left+index));
	} else {
		/* Full tile. Special handling for bun ends inside floor. */
		if (component == SHAPE_BURGER_BUNTOP) {
			if ((index==0) && (buffer[(~(tile_y>>1)) & 0x01][index] == TILES0_FLOOR_MIDDLE)) {
				setTile(x+index,tile_y>>1,TILES0_BURGER_BUNTOP_INFLOOR_LEFT);

				/* Next tile. */
				return;
			}
			if ((index==4) && (buffer[(~(tile_y>>1)) & 0x01][index] == TILES0_FLOOR_MIDDLE)) {
				setTile(x+index,tile_y>>1,TILES0_BURGER_BUNTOP_INFLOOR_RIGHT);

				/* Next tile. */
				return;
			}
		}

		/* Not a special case. */
		setTile(x+index,tile_y>>1,pgm_read_byte(&ShapeBurgers[component][Tileset].left+index));
	}
}


void drawBurgerComponent(uint8_t x, uint8_t half_y, uint8_t component, uint8_t stomped, uint8_t buffer[2][5]) {
	uint8_t i;

	/* Go through all burger component tiles in a row. */
	for (i=0;i<5;i++) {
		drawBurgerComponentTile(i, x, half_y, component, stomped, buffer);
	}
}


/*
 * Save/Restore VRAM contents under burger component into buffer.
 * Burger components are always moving down, so save/restore can be done in a very
 * straightforward fashion even with half-tiles.
 *
 *   tile_y|   0     1       2      3       4
 *         |
 *  tile_y |
 * --------------------------------------------
 *    0    | Full   Upper
 *    2    |        Lower  Full   Upper
 *    4    |                      Lower    Full
 *
 * When tile_y is odd, lower background at tile_y+1 has to be saved.
 * When tile_y is even, background at tile_y-2 has to be restored.
 * Save buffer has to be alternated, which can be done by using bit 1 of tile_y as an index.
 */
void handleBurgerBackgroundTile(uint8_t index, uint8_t x, int8_t half_y, uint8_t stomped, uint8_t buffer[2][5]) {
	int8_t y, tile_y;

	/* Add tile stomped value to component y position. */
	tile_y=half_y+((stomped>>index) & 0x01);

	/* Save/restore is selected by full/half tile. */
	if (tile_y & 0x01) {
		/* Half tile. Save. */
		y=(tile_y>>1)+1; if (y>=SCREEN_HEIGHT) y=0;
		buffer[(tile_y>>1) & 0x01][index]=getTile(x+index,y);

		/* Restore floor half tiles immediately, as we have special handling in the draw function for them. */
		if (buffer[(~(tile_y>>1)) & 0x01][index] == TILES0_FLOOR_MIDDLE) {
			setTile(x+index,(tile_y>>1),TILES0_FLOOR_MIDDLE);
		}
	} else {
		/* Full tile. Restore. */
		setTile(x+index,(tile_y>>1)-1,buffer[(tile_y>>1) & 0x01][index]);
	}

}


void handleBurgerBackground(uint8_t x, int8_t half_y, uint8_t stomped, uint8_t buffer[2][5]) {
	uint8_t i;

	/* Go through all burger component tiles in a row. */
	for (i=0;i<5;i++) {
		handleBurgerBackgroundTile(i, x, half_y, stomped, buffer);
	}
}


/* Draw a soda can. */
void drawSoda(uint8_t x, uint8_t y) {
	tiles_block_t soda[2]=TILES_COMPOUND(TILES_BLOCK,SODA);

	setTile(x,y,soda[Tileset].upperleft);
	setTile(x+1,y,soda[Tileset].upperright);
	setTile(x,y+1,soda[Tileset].lowerleft);
	setTile(x+1,y+1,soda[Tileset].lowerright);
}


/* Draw fries packet. */
void drawFries(uint8_t x, uint8_t y) {
	tiles_block_t fries[2]=TILES_COMPOUND(TILES_BLOCK,FRIES);

	setTile(x,y,fries[Tileset].upperleft);
	setTile(x+1,y,fries[Tileset].upperright);
	setTile(x,y+1,fries[Tileset].lowerleft);
	setTile(x+1,y+1,fries[Tileset].lowerright);
}


/* Draw crown. */
void drawCrown(uint8_t x, uint8_t y) {
	tiles_block_t crown[2]=TILES_COMPOUND(TILES_BLOCK,CROWN);

	setTile(x,y,crown[Tileset].upperleft);
	setTile(x+1,y,crown[Tileset].upperright);
	setTile(x,y+1,crown[Tileset].lowerleft);
	setTile(x+1,y+1,crown[Tileset].lowerright);
}


/* Draw plate. */
void drawPlate(uint8_t x, uint8_t y) {
	tiles_burger_t plate[2]=TILES_COMPOUND(TILES_BURGER,PLATE);

	setTile(x,y,plate[Tileset].left);
	setTile(x+1,y,plate[Tileset].middleleft);
	setTile(x+2,y,plate[Tileset].middle);
	setTile(x+3,y,plate[Tileset].middleright);
	setTile(x+4,y,plate[Tileset].right);
}


/* Draw score. */
void drawScore(uint8_t x, uint8_t y, uint32_t value) {
  uint8_t c,i;

	/* Print header. */
	setTile(x,y,TILES0_SCORE_LEFT);
	setTile(x+1,y,TILES0_SCORE_MIDDLE);
	setTile(x+2,y,TILES0_SCORE_RIGHT);

	/* Print value. */
	x+=7;
  for(i=0;i<7;i++) {
		c=value%10;
		setTile(x--,y+1,0xf0-FONT_BEFORE_BORDER_TILES_COUNT+c);
    value=value/10;
  }
}


/* Draw bonus. */
void drawBonus(uint8_t x, uint8_t y, uint16_t value) {
  uint8_t c,i;

	/* Print header. */
	setTile(x,y,TILES0_BONUS_LEFT);
	setTile(x+1,y,TILES0_BONUS_MIDDLE);
	setTile(x+2,y,TILES0_BONUS_RIGHT);

	/* Print value. */
	x+=3;
  for(i=0;i<3;i++) {
		c=value%10;
		setTile(x--,y+1,0xf0-FONT_BEFORE_BORDER_TILES_COUNT+c);
    value=value/10;
  }
}


/* Draw lives. */
void drawLives(uint8_t x, uint8_t y, uint8_t value) {
	uint8_t i;

	/* Limit number of drawn cooks. */
	value=min(value,LIVES_DRAW_MAX);

	/* Draw a vertical line of cooks. */
	for(i=0;i<LIVES_DRAW_MAX;i++) {
		if (i<(value-1))
				setTile(x,y-i,TILES0_COOK_SMALL);
			else
				setTile(x,y-i,TILES0_SPACE);
	}
}


/* Draw highscore entry. */
void drawHighscore(uint8_t x, uint8_t y, uint32_t name, uint32_t score) {
	uint8_t i;

	/* Print name. */
	for (i=0;i<5;i++) {
		SetTile(x+i,y,SHARED_TILES_COUNT+(name & 0x1f));
		name>>=5;
	}

	/* Print connecting line. */
	for (i=5;i<15;i++) {
		SetTile(x+i,y,TILES1_DOT);
	}

	/* Print score. */
 	for(i=0;i<8;i++) {
		/* Draw char. */
		setTile(x+15-i,y,SHARED_TILES_COUNT-FONT_BEFORE_BORDER_TILES_COUNT+0x10+(score%10));

		/* Next char, break if only zeroes are left. */
 	  score=score/10;
		if (score == 0) break;
  }
}


/* Draw hurry and save background. */
void drawHurry(uint8_t x, uint8_t y, uint8_t buffer[3]) {
	int8_t xh, yh;

	/* Make sure coordinates are on-screen. */
	xh=min(max(x,0),SCREEN_WIDTH-3);
	yh=min(max(y,0),SCREEN_HEIGHT-1);

	/* Save background. */
	buffer[0]=getTile(xh,yh);
	buffer[1]=getTile(xh+1,yh);
	buffer[2]=getTile(xh+2,yh);

	/* Draw. */
	setTile(xh,yh,TILES0_HURRY_LEFT);
	setTile(xh+1,yh,TILES0_HURRY_MIDDLE);
	setTile(xh+2,yh,TILES0_HURRY_RIGHT);
}


/* Restore hurry background. */
void restoreHurryBackground(uint8_t x, uint8_t y, uint8_t buffer[3]) {
	int8_t xh, yh;

	/* Make sure coordinates are on-screen. */
	xh=min(max(x,0),SCREEN_WIDTH-3);
	yh=min(max(y,0),SCREEN_HEIGHT-1);

	/* Restore background. */
	setTile(xh,yh,buffer[0]);
	setTile(xh+1,yh,buffer[1]);
	setTile(xh+2,yh,buffer[2]);
}

