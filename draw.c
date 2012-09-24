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
#include "utils.h"


/* Clear screen. */
void clearScreen(void) {
	uint8_t space[2]=TILES(SPACE);
	Fill(0,0,30,28,space[Tileset]);
}


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
	const drawable_t *p=ScreenLists;
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

