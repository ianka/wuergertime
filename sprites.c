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
 *  sprites.c - sprite tile organisation and aggregates.
 */


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */
#include <uzebox.h>


/* Local includes. */
#include "draw.h" /* for getTile */
#include "tiles.h"
#include "player.h"
#include "screens.h" /* for stomp() */
#include "data/sprites.inc"


/* Actual sprite tile data, generated by pcxtotiles.tcl tool */
#include "data/sprites0.inc"


/* Megasprite animations. */
#define SPRITE_ANIMATION_COOK_SIDE_MAX 4
const uint8_t SpriteAnimationCookSide[SPRITE_ANIMATION_COOK_SIDE_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_COOK_SIDE_ANIMATE0),
	TILES_BLOCK(TILES2_COOK_SIDE_ANIMATE1),
	TILES_BLOCK(TILES2_COOK_SIDE_ANIMATE2),
	TILES_BLOCK(TILES2_COOK_SIDE_ANIMATE3),
};


#define SPRITE_ANIMATION_COOK_LADDER_MAX 8
const uint8_t SpriteAnimationCookLadder[SPRITE_ANIMATION_COOK_LADDER_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE0),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE1),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE2),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE3),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE4),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE5),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE6),
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE7),
};


/* Megasprite slots. */
struct {
	uint8_t x, y;
	uint16_t flags;
} GameSpriteSlots[SPRITE_SLOTS_MAX];


/* Reset sprite slots. */
void resetSpriteSlots(void) {
	uint8_t i;

	for(i=0;i<SPRITE_SLOTS_MAX;i++)
		GameSpriteSlots[i].flags=SPRITE_FLAGS_FREE_SLOT;
}


/* Occupy a sprite slot. */
uint8_t occupySpriteSlot(void) {
	uint8_t i;
	
	/* Get a free sprite slot. */
	for(i=0;i<SPRITE_SLOTS_MAX;i++)
		if (GameSpriteSlots[i].flags == SPRITE_FLAGS_FREE_SLOT) break;
	
	/* Return free slot number. */
	return i;
}


/* Free a sprite slot. */
void freeSpriteSlot(uint8_t slot) {
	uint8_t i;

	/* Skip if the slot is free already. */ 
	if (GameSpriteSlots[slot].flags == SPRITE_FLAGS_FREE_SLOT) return;

	/* Unmap kernel sprites for that slot. */
	for(i=0;i<4;i++)
		sprites[slot*4+i].y=0xf0; /* invalid coordinate */

	/* Free the slot. */
	GameSpriteSlots[slot].flags=SPRITE_FLAGS_FREE_SLOT;
}


/* Update sprite on screen. */
void updateSprite(uint8_t slot) {
	uint8_t tile, i, view_right;
	const uint8_t *p;

	/* Setup kernel sprites according to flags. */
	switch (GameSpriteSlots[slot].flags & (SPRITE_FLAGS_TYPE_MASK|SPRITE_FLAGS_DIRECTION_MASK)) {
		case SPRITE_FLAGS_TYPE_SCORE:
			break;
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT:
			/* Get address of first tile number for given animation step. */
			p=&SpriteAnimationCookSide[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_COOK_SIDE_MAX][0];
			i=slot*4;
			view_right=((GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) == SPRITE_FLAGS_DIRECTION_RIGHT);

			/* Place tiles, honor mirroring. */
			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-(view_right?0:8);
			sprites[i].y=GameSpriteSlots[slot].y-16;
			p++;
			i++;

			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-(view_right?8:0);
			sprites[i].y=GameSpriteSlots[slot].y-16;
			p++;
			i++;

			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-(view_right?0:8);
			sprites[i].y=GameSpriteSlots[slot].y-8;
			p++;
			i++;

			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-(view_right?8:0);
			sprites[i].y=GameSpriteSlots[slot].y-8;
			break;
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LADDER:
			/* Get address of first tile number for given animation step. */
			p=&SpriteAnimationCookLadder[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_COOK_LADDER_MAX][0];
			i=slot*4;

			/* Place tiles, honor mirroring. */
			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=(tile & SPRITE_MIRROR)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-8;
			sprites[i].y=GameSpriteSlots[slot].y-16;
			p++;
			i++;

			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=(tile & SPRITE_MIRROR)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x;
			sprites[i].y=GameSpriteSlots[slot].y-16;
			p++;
			i++;

			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=(tile & SPRITE_MIRROR)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-8;
			sprites[i].y=GameSpriteSlots[slot].y-8;
			p++;
			i++;

			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=(tile & SPRITE_MIRROR)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x;
			sprites[i].y=GameSpriteSlots[slot].y-8;
			break;
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_FACING:
			break;
		case SPRITE_FLAGS_TYPE_EGGHEAD:
			break;
		case SPRITE_FLAGS_TYPE_SAUSAGEMAN:
			break;
		case SPRITE_FLAGS_TYPE_MRMUSTARD:
			break;
	}
}


/* Place a sprite somewhere. */
void placeSprite(uint8_t slot, uint8_t x, uint8_t y, uint16_t flags) {
	/* Remember position and flags. */
	GameSpriteSlots[slot].x=x;
	GameSpriteSlots[slot].y=y;
	GameSpriteSlots[slot].flags=flags;

	/* Update sprite. */
	updateSprite(slot);
}


/* Move a sprite. */
void moveSprite(uint8_t slot, int8_t x, int8_t y) {
	/* Skip if horizontal position is not within boundaries. */
	if ((GameSpriteSlots[slot].x+x<8) || (GameSpriteSlots[slot].x+x>(SCREEN_WIDTH<<3)-8)) return;

	/* Remember new position. */
	GameSpriteSlots[slot].x+=x;
	GameSpriteSlots[slot].y+=y;

	/* Next animation step. */
	GameSpriteSlots[slot].flags=(GameSpriteSlots[slot].flags & ~SPRITE_FLAGS_ANIMATION_MASK) | ((GameSpriteSlots[slot].flags+1) & SPRITE_FLAGS_ANIMATION_MASK);

	/* Update sprite. */
	updateSprite(slot);
}


/* Change sprite direction. */
void changeSpriteDirection(uint8_t slot, uint8_t direction) {
	/* Remember updated direction, reset animation. */
	GameSpriteSlots[slot].flags=(GameSpriteSlots[slot].flags & ~(SPRITE_FLAGS_DIRECTION_MASK|SPRITE_FLAGS_ANIMATION_MASK)) | (direction & SPRITE_FLAGS_DIRECTION_MASK);

	/* Update sprite. */
	updateSprite(slot);
}


/* Get coordinates of sprite. */
uint8_t getSpriteX(uint8_t slot) {
	return GameSpriteSlots[slot].x;
}

uint8_t getSpriteY(uint8_t slot) {
	return GameSpriteSlots[slot].y;
}


/* Get tile index for tile under sprite foot. */
uint8_t getSpriteFloorTile(uint8_t slot) {
	switch (GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) {
		case SPRITE_FLAGS_DIRECTION_LEFT:
			return getTile(((GameSpriteSlots[slot].x-1)>>3),GameSpriteSlots[slot].y>>3);
		default:	
			return getTile(GameSpriteSlots[slot].x>>3,GameSpriteSlots[slot].y>>3);
	}
}

/* Get tile index for tile in sprite direction. */
uint8_t getSpriteFloorDirectionTile(uint8_t slot) {
	switch (GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) {
		case SPRITE_FLAGS_DIRECTION_LEFT:
			return getTile(((GameSpriteSlots[slot].x-1)>>3)-1,GameSpriteSlots[slot].y>>3);
		case SPRITE_FLAGS_DIRECTION_RIGHT:
			return getTile((GameSpriteSlots[slot].x>>3)+1,GameSpriteSlots[slot].y>>3);
		default:	
			return getTile(GameSpriteSlots[slot].x>>3,GameSpriteSlots[slot].y>>3);
	}
}


/* Get tile index for floor tile when going up on a ladder. */
uint8_t getSpriteLadderTopTile(uint8_t slot) {
	return getTile(GameSpriteSlots[slot].x>>3,((GameSpriteSlots[slot].y-1)>>3)+1);
}


/* Get tile index for tile behind sprite. */
uint8_t getSpriteLadderTile(uint8_t slot) {
	return getTile(GameSpriteSlots[slot].x>>3,(GameSpriteSlots[slot].y-1)>>3);
}


/* Try to stomp the tile under the sprite. */
void stompUnderSprite(uint8_t slot) {
	if (!(stomp(GameSpriteSlots[slot].x>>3,(GameSpriteSlots[slot].y>>3)-1)))
		stomp(GameSpriteSlots[slot].x>>3,GameSpriteSlots[slot].y>>3);
}


/* Check if at ladder up entry. */
uint8_t checkSpriteAtLadderEntryUp(uint8_t slot) {
	/* Fail if not at exact tile coordinate. */
	if (getSpriteX(slot) & 0x07) return 0;

	/* On an exact tile coordinate. Check ladder. */
	switch (getSpriteLadderTile(slot)) {
		case TILES0_LADDER_RIGHT:
		case TILES0_LADDER_MUSTARDED_RIGHT:
		case TILES0_LADDER_MUSTARDED_CLEANED_RIGHT:
		case TILES0_LADDER_TOP_RIGHT:
		case TILES0_LADDER_TOP_FLOOREND_RIGHT:
			/* On ladder up entry. */
			return 1;
		default:
			/* Not on a ladder up entry. */
			return 0;
	}		
}


/* Check if at ladder down entry. */
uint8_t checkSpriteAtLadderEntryDown(uint8_t slot) {
	/* Fail if not at exact tile coordinate. */
	if (getSpriteX(slot) & 0x07) return 0;

	/* On an exact tile coordinate. Check floor. */
	switch (getSpriteFloorTile(slot)) {
		case TILES0_LADDER_TOP_LEFT:
		case TILES0_LADDER_TOP_FLOOREND_LEFT:
			/* On ladder entry down if we are currently moving left. */
			return ((GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) == SPRITE_FLAGS_DIRECTION_LEFT);
		case TILES0_LADDER_TOP_RIGHT:
		case TILES0_LADDER_TOP_FLOOREND_RIGHT:
			/* On ladder entry down if we are currently moving right. */
			return ((GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) == SPRITE_FLAGS_DIRECTION_RIGHT);
		default:
			/* Not at a ladder down entry. */
			return 0;
	}
}


/* Check if at ladder exit. */
uint8_t checkSpriteAtLadderExit(uint8_t slot) {
	/* Fail if not at exact tile coordinate. */
	if (getSpriteY(slot) & 0x07) return 0;

	/* On an exact tile coordinate. Check floor. */
	switch (getSpriteFloorTile(slot)) {
		case TILES0_LADDER_TOP_LEFT:
		case TILES0_LADDER_TOP_FLOOREND_LEFT:
		case TILES0_LADDER_TOP_UPONLY_LEFT:
		case TILES0_LADDER_BOTTOM_LEFT:
		case TILES0_LADDER_BOTTOM_FLOOREND_LEFT:
		case TILES0_LADDER_TOP_RIGHT:
		case TILES0_LADDER_TOP_FLOOREND_RIGHT:
		case TILES0_LADDER_TOP_UPONLY_RIGHT:
		case TILES0_LADDER_BOTTOM_RIGHT:
		case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
			/* On ladder exit. */
			return 1;
		default:
			/* Not on a ladder exit. */
			return 0;
	}
}


/* Check if at ladder top. */
uint8_t checkSpriteAtLadderTop(uint8_t slot) {
	switch (getSpriteLadderTopTile(slot)) {
		case TILES0_LADDER_TOP_RIGHT:
		case TILES0_LADDER_TOP_FLOOREND_RIGHT:
			/* Ladder top end? */
			switch (getSpriteLadderTile(slot)) {
				case TILES0_LADDER_RIGHT:
				case TILES0_LADDER_TOP_RIGHT:
				case TILES0_LADDER_TOP_FLOOREND_RIGHT:
				case TILES0_LADDER_MUSTARDED_RIGHT:
				case TILES0_LADDER_MUSTARDED_CLEANED_RIGHT:
					/* Not at ladder top. */
					return 0;
				default:
					/* At ladder top. */
					return 1;
			}
		default:
			/* Not at ladder top. */
			return 0;
	}
}	


/* Check if at ladder bottom. */
uint8_t checkSpriteAtLadderBottom(uint8_t slot) {
	switch (getSpriteFloorTile(slot)) {
		case TILES0_LADDER_BOTTOM_RIGHT:
		case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
		case TILES0_LADDER_TOP_UPONLY_RIGHT:
			/* On ladder exit. */
			return 1;
		default:
			/* Not on a ladder exit. */
			return 0;
	}
}


/* Check the floor tile for anything that should stop us. */
uint8_t checkSpriteAtLeftFloorEnd(uint8_t slot) {
	switch (getSpriteFloorTile(slot)) {
		case TILES0_FLOOR_LEFT:
		case TILES0_LADDER_TOP_FLOOREND_LEFT:
		case TILES0_LADDER_BOTTOM_FLOOREND_LEFT:
			/* At left floor end. */
			return 1;
		default:
			/* Not at left floor end. */
			return 0;
	}
}

uint8_t checkSpriteAtRightFloorEnd(uint8_t slot) {
	switch (getSpriteFloorTile(slot)) {
		case TILES0_FLOOR_RIGHT:
		case TILES0_LADDER_TOP_FLOOREND_RIGHT:
		case TILES0_LADDER_BOTTOM_FLOOREND_RIGHT:
			/* At right floor end. */
			return 1;
		default:
			/* Not at left floor end. */
			return 0;
	}
}
