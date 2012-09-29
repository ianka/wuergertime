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
 *  tiles.h - tile organisation and aggregates.
 */


#ifndef TILES_H
#define TILES_H


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */
#include <uzebox.h> /* for RAM_TILES_COUNT */


/* Local includes */
#include "data/tiles.inc" /* for tile semantics */


/*
 * Tile basics.
 */
extern const char Tiles[] PROGMEM;

#define PIXELS_PER_TILE 64


/*  Tile offsets. */
#define TILEMAP_WIDTH 16
#define SHARED_TILES_COUNT 128
#define UNIQUE_TILES_COUNT 80 /* 256-SHARED_TILES_COUNT-RAM_TILES_COUNT */
#define FONT_BEFORE_BORDER_TILES_COUNT 32


/* Tilesets */
#define TILESET0 0
#define TILESET1 1


/* Tileset switch */
extern uint8_t Tileset;


/* Change tileset. */
static inline void SetTileset(uint8_t tileset) {
	Tileset=tileset;
	SetTileTable(Tiles+(tileset?PIXELS_PER_TILE*UNIQUE_TILES_COUNT:0));
	SetFontTilesIndex(SHARED_TILES_COUNT-FONT_BEFORE_BORDER_TILES_COUNT-RAM_TILES_COUNT+(tileset?0:UNIQUE_TILES_COUNT));
	//SetFontTilesIndex(SHARED_TILES_COUNT-FONT_BEFORE_BORDER_TILES_COUNT+(tileset?0:UNIQUE_TILES_COUNT));
}


/* Tile index by unique/shared tile in tileset 0/1. */
#define utile(tileset,row,column) (((tileset?SHARED_TILES_COUNT:0)+TILEMAP_WIDTH*row+column))
#define stile(tileset,row,column) (((tileset?0:UNIQUE_TILES_COUNT)+TILEMAP_WIDTH*row+column))


/* Tile initializers by page. */
#define TILES(name) { TILES0_ ## name, TILES1_ ## name }
#define TILES_COMPOUND(compound,name) { compound(TILES0_ ## name), compound(TILES1_ ## name) }


/* Compounds */
typedef struct { uint8_t left, right; } tiles_duo_t;
typedef struct { uint8_t top, bottom; } tiles_tallduo_t;
typedef struct { uint8_t left, middle, right; } tiles_trio_t;
typedef struct { uint8_t upperleft, upperright, lowerleft, lowerright; } tiles_block_t;
typedef struct { uint8_t left, middleleft, middle, middleright, right; } tiles_burger_t;

#define TILES_DUO(c) { c ## _LEFT, c ## _RIGHT }
#define TILES_TALLDUO(c) { c ## _TOP, c ## _BOTTOM }
#define TILES_TRIO(c) { c ## _LEFT, c ## _MIDDLE, c ## _RIGHT }
#define TILES_BLOCK(c) { c ## _UPPER_LEFT, c ## _UPPER_RIGHT, c ## _LOWER_LEFT, c ## _LOWER_RIGHT }
#define TILES_BURGER(c) { c ## _LEFT, c ## _MIDDLELEFT, c ## _MIDDLE, c ## _MIDDLERIGHT, c ## _RIGHT }


#endif /* TILES_H */
