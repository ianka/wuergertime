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
 *  player.h - player functions.
 */


#ifndef PLAYER_H
#define PLAYER_H


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"


/* Player flags: direction and speed. */
#define PLAYER_FLAGS_DIRECTION_SHIFT 0
#define PLAYER_FLAGS_DIRECTION_MASK  ((0x07<<PLAYER_FLAGS_DIRECTION_SHIFT))
#define PLAYER_FLAGS_DIRECTION_NONE  ((0<<PLAYER_FLAGS_DIRECTION_SHIFT))
#define PLAYER_FLAGS_DIRECTION_LEFT  ((1<<PLAYER_FLAGS_DIRECTION_SHIFT))
#define PLAYER_FLAGS_DIRECTION_RIGHT ((2<<PLAYER_FLAGS_DIRECTION_SHIFT))
#define PLAYER_FLAGS_DIRECTION_UP    ((3<<PLAYER_FLAGS_DIRECTION_SHIFT))
#define PLAYER_FLAGS_DIRECTION_DOWN  ((4<<PLAYER_FLAGS_DIRECTION_SHIFT))
#define PLAYER_FLAGS_SPEED_SHIFT     3
#define PLAYER_FLAGS_SPEED_MASK      ((0x03<<PLAYER_FLAGS_SPEED_SHIFT))
#define PLAYER_FLAGS_SPEED_SLOW      ((0<<PLAYER_FLAGS_SPEED_SHIFT))
#define PLAYER_FLAGS_SPEED_NORMAL    ((1<<PLAYER_FLAGS_SPEED_SHIFT))
#define PLAYER_FLAGS_SPEED_FAST      ((2<<PLAYER_FLAGS_SPEED_SHIFT))
#define PLAYER_FLAGS_SPEED_FLASH     ((3<<PLAYER_FLAGS_SPEED_SHIFT))


typedef struct {
	uint8_t flags;
	uint8_t sprite;
	position_t start_position;
} player_t;	

extern player_t Player;


void resetPlayer(void);
void changePlayerDirectionWithoutAnimationReset(uint8_t direction);
void changePlayerDirection(uint8_t direction);


#endif /* PLAYER_H */
