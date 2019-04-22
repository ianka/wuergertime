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


/* Transparent sprite. */
const uint8_t SpriteTransparent[1][4] PROGMEM = {
	TILES_BLOCK(TILES2_TRANSPARENT)
};


/* Pepper sprite. */
const uint8_t SpritePepper[1][4] PROGMEM = {
	TILES_BLOCK(TILES2_PEPPER)
};


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


#define SPRITE_ANIMATION_COOK_CAUGHT_MAX 4
const uint8_t SpriteAnimationCookCaught[SPRITE_ANIMATION_COOK_CAUGHT_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_COOK_FACING_ANIMATE0),
	TILES_BLOCK(TILES2_COOK_FACING_ANIMATE1),
	TILES_BLOCK(TILES2_COOK_FACING_ANIMATE2),
	TILES_BLOCK(TILES2_COOK_FACING_ANIMATE3),
};


#define SPRITE_ANIMATION_COOK_SLIDE_MAX 1
const uint8_t SpriteAnimationCookSlide[SPRITE_ANIMATION_COOK_SLIDE_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_COOK_LADDER_ANIMATE0),
};


#define SPRITE_ANIMATION_EGGHEAD_SIDE_MAX 4
const uint8_t SpriteAnimationEggheadSide[SPRITE_ANIMATION_EGGHEAD_SIDE_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_EGGHEAD_SIDE_ANIMATE0),
	TILES_BLOCK(TILES2_EGGHEAD_SIDE_ANIMATE1),
	TILES_BLOCK(TILES2_EGGHEAD_SIDE_ANIMATE2),
	TILES_BLOCK(TILES2_EGGHEAD_SIDE_ANIMATE3),
};


#define SPRITE_ANIMATION_EGGHEAD_LADDER_MAX 4
const uint8_t SpriteAnimationEggheadLadder[SPRITE_ANIMATION_EGGHEAD_LADDER_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_EGGHEAD_LADDER_ANIMATE0),
	TILES_BLOCK(TILES2_EGGHEAD_LADDER_ANIMATE1),
	TILES_BLOCK(TILES2_EGGHEAD_LADDER_ANIMATE2),
	TILES_BLOCK(TILES2_EGGHEAD_LADDER_ANIMATE3),
};


#define SPRITE_ANIMATION_SAUSAGEMAN_SIDE_MAX 8
const uint8_t SpriteAnimationSausagemanSide[SPRITE_ANIMATION_SAUSAGEMAN_SIDE_MAX][4] PROGMEM = {
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE0),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE1),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE2),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE3),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE4),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE5),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE6),
	TILES_BLOCK(TILES2_SAUSAGEMAN_SIDE_ANIMATE7),
};

#define SPRITE_ANIMATION_SAUSAGEMAN_LADDER_MAX 4
const uint8_t SpriteAnimationSausagemanLadder[SPRITE_ANIMATION_SAUSAGEMAN_LADDER_MAX][2] PROGMEM = {
	TILES_TALLDUO(TILES2_SAUSAGEMAN_LADDER_ANIMATE0),
	TILES_TALLDUO(TILES2_SAUSAGEMAN_LADDER_ANIMATE0),
	TILES_TALLDUO(TILES2_SAUSAGEMAN_LADDER_ANIMATE1),
	TILES_TALLDUO(TILES2_SAUSAGEMAN_LADDER_ANIMATE1),
};


#define SPRITE_ANIMATION_MRMUSTARD_SIDE_MAX 4
const uint8_t SpriteAnimationMrMustardSide[SPRITE_ANIMATION_MRMUSTARD_SIDE_MAX][2] PROGMEM = {
	TILES_TALLDUO(TILES2_MRMUSTARD_SIDE_ANIMATE0),
	TILES_TALLDUO(TILES2_MRMUSTARD_SIDE_ANIMATE1),
	TILES_TALLDUO(TILES2_MRMUSTARD_SIDE_ANIMATE2),
	TILES_TALLDUO(TILES2_MRMUSTARD_SIDE_ANIMATE3),
};


#define SPRITE_ANIMATION_MRMUSTARD_SQUIRT_MAX 7
const uint8_t SpriteAnimationMrMustardSquirt[SPRITE_ANIMATION_MRMUSTARD_SQUIRT_MAX][2] PROGMEM = {
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE0),
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE1),
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE2),
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE3),
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE4),
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE5),
	TILES_TALLDUO(TILES2_MRMUSTARD_SQUIRT_ANIMATE6),
};


/* Megasprite slots. */
struct {
	uint8_t x;
	int16_t y;
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

	/* Occupy free slot. */
	if (i<SPRITE_SLOTS_MAX)
		GameSpriteSlots[i].flags=SPRITE_FLAGS_OCCUPIED_SLOT;

	/* Return slot number or full marker. */
	return i;
}


/* Unmap a sprite. */
void unmapSprite(uint8_t slot) {
	uint8_t i;

	/* Unmap kernel sprites for that slot. */
	for(i=0;i<4;i++)
		sprites[slot*4+i].x=OFF_SCREEN; /* invalid coordinate */
}


/* Free a sprite slot. */
void freeSpriteSlot(uint8_t slot) {
	/* Skip if the slot is free already. */
	if (GameSpriteSlots[slot].flags == SPRITE_FLAGS_FREE_SLOT) return;

	/* Unmap sprite. */
	unmapSprite(slot);

	/* Free the slot. */
	GameSpriteSlots[slot].flags=SPRITE_FLAGS_FREE_SLOT;
}


/* Update sprite on screen. */
void updateSprite(uint8_t slot) {
	uint8_t tile, i, j, view_right, flip_y;
	const uint8_t *p;

	/* Setup kernel sprite shape according to flags. */
	if (GameSpriteSlots[slot].flags & SPRITE_FLAGS_TRANSPARENT) {
		/* Transparent sprite. */
		p=&SpriteTransparent[0][0];
	} else {
		/* Non-transparent sprite. */
		switch (GameSpriteSlots[slot].flags & (SPRITE_FLAGS_TYPE_MASK|SPRITE_FLAGS_DIRECTION_MASK)) {
			case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LEFT:
			case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT:
			case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_LEFT:
			case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_RIGHT:
				p=&SpriteAnimationCookSide[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_COOK_SIDE_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LADDER:
			case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_LADDER:
				p=&SpriteAnimationCookLadder[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_COOK_LADDER_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_CAUGHT:
			case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_CAUGHT:
				p=&SpriteAnimationCookCaught[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_COOK_CAUGHT_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_SLIDE:
			case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_SLIDE:
				p=&SpriteAnimationCookSlide[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_COOK_SLIDE_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_LEFT:
			case SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_RIGHT:
				p=&SpriteAnimationEggheadSide[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_EGGHEAD_SIDE_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_LADDER:
				p=&SpriteAnimationEggheadLadder[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_EGGHEAD_LADDER_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_LEFT:
			case SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_RIGHT:
				p=&SpriteAnimationSausagemanSide[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_SAUSAGEMAN_SIDE_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_LADDER:
				p=&SpriteAnimationSausagemanLadder[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_SAUSAGEMAN_LADDER_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_LEFT:
			case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_RIGHT:
			case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_LADDER:
				p=&SpriteAnimationMrMustardSide[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_MRMUSTARD_SIDE_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_SQUIRT:
				p=&SpriteAnimationMrMustardSquirt[((GameSpriteSlots[slot].flags & SPRITE_FLAGS_ANIMATION_MASK)>>1) % SPRITE_ANIMATION_MRMUSTARD_SQUIRT_MAX][0];
				break;
			case SPRITE_FLAGS_TYPE_PEPPER|SPRITE_FLAGS_DIRECTION_LEFT:
			case SPRITE_FLAGS_TYPE_PEPPER|SPRITE_FLAGS_DIRECTION_RIGHT:
				p=&SpritePepper[0][0];
				break;
			default:
				return;
		}
	}

	/* Get address of first tile number for given animation step. */
	i=slot*4;

	/* Get view direction. */
	view_right=((GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) == SPRITE_FLAGS_DIRECTION_RIGHT);
	if ((GameSpriteSlots[slot].flags & SPRITE_FLAGS_TYPE_MASK) == SPRITE_FLAGS_TYPE_ANTICOOK)
		view_right^=1;

	/* No vertical flip. */
	flip_y=0;

	/* Setup kernel sprites according to flags. */
	switch (GameSpriteSlots[slot].flags & (SPRITE_FLAGS_TYPE_MASK|SPRITE_FLAGS_DIRECTION_MASK)) {
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_CAUGHT:
			/* Vertical flip. */
			flip_y=1;

			/* Fall through. */
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LADDER:
		case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_LADDER:
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_SLIDE:
		case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_SLIDE:
		case SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_LADDER:
			/* Ignore view for these pieces. */
			view_right=0;

			/* Fall through. */
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT:
		case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_ANTICOOK|SPRITE_FLAGS_DIRECTION_RIGHT:
		case SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_RIGHT:
		case SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_RIGHT:
			/* Place tiles, honor mirroring and flipping. */
			for (j=0;j<4;j++) {
				tile=pgm_read_byte(p);
				sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
				sprites[i].flags=(((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0)|(flip_y*SPRITE_FLIP_Y);
				sprites[i].x=GameSpriteSlots[slot].x-((j&1)^view_right?0:8)-SPRITE_BORDER_WIDTH;
				sprites[i].y=GameSpriteSlots[slot].y-8-((~j^(flip_y*2))&2)*4;
				p++;
				i++;
			}
			break;
		case SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_LADDER:
		case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_RIGHT:
		case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_LADDER:
		case SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_SQUIRT:
			/* Place tiles, honor mirroring. */
			for (j=0;j<2;j++) {
				tile=pgm_read_byte(p);
				sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
				sprites[i].flags=((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0;
				sprites[i].x=GameSpriteSlots[slot].x-4-SPRITE_BORDER_WIDTH;
				sprites[i].y=GameSpriteSlots[slot].y-16+(j&1)*8;
				p++;
				i++;
			}

			/* Remove unneeded sprite tiles from screen. */
			sprites[i].x=OFF_SCREEN;
			i++;
			sprites[i].x=OFF_SCREEN;
			break;
		case SPRITE_FLAGS_TYPE_PEPPER|SPRITE_FLAGS_DIRECTION_LEFT:
		case SPRITE_FLAGS_TYPE_PEPPER|SPRITE_FLAGS_DIRECTION_RIGHT:
			/* Place tiles, honor mirroring. */
			tile=pgm_read_byte(p);
			sprites[i].tileIndex=tile & (~SPRITE_MIRROR);
			sprites[i].flags=((tile & SPRITE_MIRROR)^view_right)?SPRITE_FLIP_X:0;
			sprites[i].x=GameSpriteSlots[slot].x-4-SPRITE_BORDER_WIDTH;
			sprites[i].y=GameSpriteSlots[slot].y-16;
			i++;

			/* Remove unneeded sprite tiles from screen. */
			sprites[i].x=OFF_SCREEN;
			i++;
			sprites[i].x=OFF_SCREEN;
			i++;
			sprites[i].x=OFF_SCREEN;
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
uint8_t moveSpriteIfNotBorder(uint8_t slot, int8_t x, int8_t y) {
	/* Fail if horizontal position is not within boundaries. */
	if ((GameSpriteSlots[slot].x+x<SPRITE_BORDER_WIDTH) || (GameSpriteSlots[slot].x+x>(SCREEN_WIDTH<<3)+SPRITE_BORDER_WIDTH))
		return 0;

	/* Actuall move sprite. */
	moveSprite(slot,x,y);

	/* Sucess. */
	return 1;
}

void moveSprite(uint8_t slot, int8_t x, int8_t y) {
	/* Remember new position. */
	GameSpriteSlots[slot].x+=x;
	GameSpriteSlots[slot].y+=y;

	/* Next animation step. */
	GameSpriteSlots[slot].flags=(GameSpriteSlots[slot].flags & ~SPRITE_FLAGS_ANIMATION_MASK) | ((GameSpriteSlots[slot].flags+1) & SPRITE_FLAGS_ANIMATION_MASK);

	/* Update sprite. */
	updateSprite(slot);
}


/* Align a sprite to nearby platform level. */
void alignSpriteToPlatform(uint8_t slot) {
	/* Align. */
	GameSpriteSlots[slot].y&=0xf8;

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

uint8_t getSpriteTileX(uint8_t slot, int8_t c) {
	return ((GameSpriteSlots[slot].x-SPRITE_BORDER_WIDTH+c)>>3);
}

uint8_t getSpriteY(uint8_t slot) {
	return GameSpriteSlots[slot].y;
}

uint8_t getSpriteTileY(uint8_t slot, int8_t c) {
	return ((GameSpriteSlots[slot].y+c)>>3);
}


/* Get tile index for tile under sprite foot. */
uint8_t getSpriteFloorTile(uint8_t slot) {
	switch (GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) {
		case SPRITE_FLAGS_DIRECTION_LEFT:
			return getTile(getSpriteTileX(slot,-1),getSpriteTileY(slot,0));
		default:
			return getTile(getSpriteTileX(slot,0),getSpriteTileY(slot,0));
	}
}

/* Get tile index for tile in sprite direction. */
uint8_t getSpriteFloorDirectionTile(uint8_t slot) {
	switch (GameSpriteSlots[slot].flags & SPRITE_FLAGS_DIRECTION_MASK) {
		case SPRITE_FLAGS_DIRECTION_LEFT:
			return getTile(getSpriteTileX(slot,-9),getSpriteTileY(slot,0));
		case SPRITE_FLAGS_DIRECTION_RIGHT:
			return getTile(getSpriteTileX(slot,8),getSpriteTileY(slot,0));
		default:
			return getTile(getSpriteTileX(slot,0),getSpriteTileY(slot,0));
	}
}


/* Get tile index for floor tile when going up on a ladder. */
uint8_t getSpriteLadderTopTile(uint8_t slot) {
	return getTile(getSpriteTileX(slot,0),getSpriteTileY(slot,7));
}


/* Get tile index for tile behind sprite. */
uint8_t getSpriteLadderTile(uint8_t slot) {
	return getTile(getSpriteTileX(slot,0),getSpriteTileY(slot,-1));
}


/* Try to stomp the tile under the sprite. */
void stompUnderSprite(uint8_t slot) {
	if (!(stomp(getSpriteTileX(slot,0),getSpriteTileY(slot,-8))))
		stomp(getSpriteTileX(slot,0),getSpriteTileY(slot,0));
}


/* Check if at ladder up entry. */
uint8_t checkSpriteAtLadderEntryUp(uint8_t slot) {
	/* Fail if not at exact tile coordinate. */
	if (getSpriteX(slot) & 0x07) return 0;

	/* On an exact tile coordinate. Check ladder. */
	switch (getSpriteLadderTile(slot)) {
		case TILES0_LADDER_RIGHT:
		case TILES0_LADDER_CLEANED_RIGHT:
		case TILES0_LADDER_TOP_RIGHT:
		case TILES0_LADDER_TOP_FLOOREND_RIGHT:
			/* On ladder up entry. */
			return 1;
		default:
			/* Not on a ladder up entry. */
			return 0;
	}
}


/* Check if at squirted ladder up entry. */
uint8_t checkSpriteAtSquirtedLadderEntryUp(uint8_t slot) {
	/* Fail if not at exact tile coordinate. */
	if (getSpriteX(slot) & 0x07) return 0;

	/* On an exact tile coordinate. Check ladder. */
	if (getSpriteLadderTile(slot) == TILES0_LADDER_SQUIRTED_RIGHT)
			/* On ladder up entry. */
			return 1;

	/* Not on a ladder up entry. */
	return 0;
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
				case TILES0_LADDER_SQUIRTED_RIGHT:
				case TILES0_LADDER_CLEANED_RIGHT:
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
		case TILES0_LADDER_TOP_LEFT:
		case TILES0_LADDER_TOP_UPONLY_LEFT:
		case TILES0_LADDER_BOTTOM_LEFT:
			/* At left floor end if at left screen border. */
			return (GameSpriteSlots[slot].x==8-SPRITE_BORDER_WIDTH);
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
		case TILES0_LADDER_TOP_RIGHT:
		case TILES0_LADDER_TOP_UPONLY_RIGHT:
		case TILES0_LADDER_BOTTOM_RIGHT:
			/* At right floor end if at right screen border. */
			return (GameSpriteSlots[slot].x==((SCREEN_WIDTH*8)-8-SPRITE_BORDER_WIDTH));
		default:
			/* Not at left floor end. */
			return 0;
	}
}


/* Squirt on a ladder at sprite. */
uint8_t squirtOnLadderAtSprite(uint8_t slot) {
	uint8_t x, y;

	/* Get position. */
	x=getSpriteTileX(slot,-8);
	y=getSpriteTileY(slot,-8);

	/* Skip squirted ladder pieces. */
	while (getTile(x,y) == TILES0_LADDER_SQUIRTED_LEFT)
		y--;

	/* Check if still a piece to squirt. */
	switch (getTile(x,y)) {
		case TILES0_LADDER_LEFT:
		case TILES0_LADDER_CLEANED_LEFT:
		case TILES0_LADDER_WAILED_LEFT:
			/* Squirt it. */
			drawSquirtedLadderPiece(x,y);

			/* Not fully squirted. */
			return 0;
	}

	/* Fully squirted. */
	return 1;
}


/* Clean ladder piece at sprite. */
void cleanLadderAtSprite(uint8_t slot) {
	drawCleanedLadderPiece(getSpriteTileX(slot,-8),getSpriteTileY(slot,-8));
}


/* Wail on a ladder at sprite. */
uint8_t wailOnLadderAtSprite(uint8_t slot) {
	uint8_t x, y;

	/* Get position. */
	x=getSpriteTileX(slot,-8);
	y=getSpriteTileY(slot,-8);

	/* Skip wailed ladder pieces. */
	while (getTile(x,y) == TILES0_LADDER_WAILED_LEFT)
		y--;

	/* Check if still a piece to wail over. */
	if (getTile(x,y) == TILES0_LADDER_SQUIRTED_LEFT) {
		/* Wail on that piece. */
		drawWailedLadderPiece(x,y);

		/* Not fully wailed. */
		return 0;
	}

	/* Unwail the ladder. */
	unwailLadderAtSprite(slot);

	/* Tell caller we are done with wailing. */
	return 1;
}


/* Unwail a ladder. */
void unwailLadderAtSprite(uint8_t slot) {
	uint8_t x, y;

	/* Get position. */
	x=getSpriteTileX(slot,-8);
	y=getSpriteTileY(slot,-8);

	/* Replace the wail by squirt. */
	while (getTile(x,y) == TILES0_LADDER_WAILED_LEFT)
	{
		drawSquirtedLadderPiece(x,y);
		y--;
	}
}


/* Set sprite transparency. */
void setSpriteTransparency(uint8_t slot, uint16_t transparent) {
	GameSpriteSlots[slot].flags&=~SPRITE_FLAGS_TRANSPARENT;
	GameSpriteSlots[slot].flags|=transparent;
}

