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
 *  sprites.h - sprite tile organisation and aggregates.
 */


#ifndef SPRITES_H
#define SPRITES_H


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */
#include <uzebox.h>


/* Local includes */


/* Access to sprite tiles. */
extern const char SpriteTiles[] PROGMEM;


/*  Tile offsets. */
#define SPRITEMAP_WIDTH 8
#define SPRITE_MIRROR 0x80


/* Tile index by sprite tile. */
#define mtile(row,column,mirror) (((SPRITEMAP_WIDTH*row+column)|(mirror?SPRITE_MIRROR:0)))


/* Megasprite slots. */
#define SPRITE_SLOTS_MAX              6
#define SPRITE_SLOTS_FULL             SPRITE_SLOTS_MAX
#define SPRITE_FLAGS_FREE_SLOT        0x00
#define SPRITE_FLAGS_ANIMATION_MASK   0x0f
#define SPRITE_FLAGS_DIRECTION_SHIFT  4
#define SPRITE_FLAGS_DIRECTION_MASK   ((0x03<<SPRITE_FLAGS_DIRECTION_SHIFT))
#define SPRITE_FLAGS_DIRECTION_LEFT   ((0<<SPRITE_FLAGS_DIRECTION_SHIFT)) 
#define SPRITE_FLAGS_DIRECTION_RIGHT  ((1<<SPRITE_FLAGS_DIRECTION_SHIFT))
#define SPRITE_FLAGS_DIRECTION_LADDER ((2<<SPRITE_FLAGS_DIRECTION_SHIFT))
#define SPRITE_FLAGS_DIRECTION_FACING ((3<<SPRITE_FLAGS_DIRECTION_SHIFT))
#define SPRITE_FLAGS_TYPE_SHIFT       8 
#define SPRITE_FLAGS_TYPE_MASK        ((0x07<<SPRITE_FLAGS_TYPE_SHIFT))
#define SPRITE_FLAGS_TYPE_SCORE       ((0<<SPRITE_FLAGS_TYPE_SHIFT))
#define SPRITE_FLAGS_TYPE_COOK        ((1<<SPRITE_FLAGS_TYPE_SHIFT))
#define SPRITE_FLAGS_TYPE_PEPPER      ((2<<SPRITE_FLAGS_TYPE_SHIFT))
#define SPRITE_FLAGS_TYPE_SAUSAGEMAN  ((3<<SPRITE_FLAGS_TYPE_SHIFT))
#define SPRITE_FLAGS_TYPE_EGGHEAD     ((4<<SPRITE_FLAGS_TYPE_SHIFT))
#define SPRITE_FLAGS_TYPE_MRMUSTARD   ((5<<SPRITE_FLAGS_TYPE_SHIFT))


void resetSpriteSlots(void);
uint8_t occupySpriteSlot(void);
void freeSpriteSlot(uint8_t slot);
void placeSprite(uint8_t slot, uint8_t x, uint8_t y, uint16_t flags);
void moveSprite(uint8_t slot, int8_t x, int8_t y); 
void changeSpriteDirection(uint8_t slot, uint8_t direction);
uint8_t getSpriteX(uint8_t slot);
uint8_t getSpriteY(uint8_t slot);
uint8_t getSpriteFloorTile(uint8_t slot);
uint8_t getSpriteFloorDirectionTile(uint8_t slot);
uint8_t getSpriteLadderTopTile(uint8_t slot);
uint8_t getSpriteLadderTile(uint8_t slot);
void stompUnderSprite(uint8_t slot);


#endif /* SPRITES_H */
