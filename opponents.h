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
 *  opponents.h - opponent functions.
 */


#ifndef OPPONENTS_H
#define OPPONENTS_H


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"


/* Opponent flags: direction and speed. */
#define OPPONENT_FLAGS_DIRECTION_SHIFT 0
#define OPPONENT_FLAGS_DIRECTION_MASK  ((0x07<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_NONE  ((0<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_LEFT  ((1<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_RIGHT ((2<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_UP    ((3<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_DOWN  ((4<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_SPEED_SHIFT     3
#define OPPONENT_FLAGS_SPEED_MASK      ((0x03<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_SLOW      ((0<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_NORMAL    ((1<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_FAST      ((2<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_FLASH     ((3<<OPPONENT_FLAGS_SPEED_SHIFT))


typedef struct {
	uint8_t flags;
	uint8_t sprite;
	position_t start_position;
} opponent_t;	

extern opponent_t Opponent;


void resetOpponent(void);
void changeOpponentDirectionWithoutAnimationReset(uint8_t direction);
void changeOpponentDirection(uint8_t direction);
void selectOpponentDirection(uint8_t buttons);
void moveOpponent(uint8_t buttons);


#endif /* OPPONENTS_H */
