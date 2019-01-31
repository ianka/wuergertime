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


/* Opponent basics. */
#define OPPONENT_MAX 4
#define OPPONENT_START_POSITION_MAX 8
#define OPPONENT_START_POSITION_INVALID 0xff
#define OPPONENT_TARGET_POSITION_INVALID 0xff
#define OPPONENT_ATTACK_WAVE_DEFAULT_SHIFT 9


/* Opponent flags: direction, speed, movement algorithm. */
#define OPPONENT_DIRECTION_LEFT  0
#define OPPONENT_DIRECTION_RIGHT 1
#define OPPONENT_DIRECTION_UP    2
#define OPPONENT_DIRECTION_DOWN  3

#define OPPONENT_FLAGS_INVALID         0xff
#define OPPONENT_FLAGS_DIRECTION_SHIFT 0
#define OPPONENT_FLAGS_DIRECTION_MASK  ((0x07<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_LEFT  ((OPPONENT_DIRECTION_LEFT<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_RIGHT ((OPPONENT_DIRECTION_RIGHT<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_UP    ((OPPONENT_DIRECTION_UP<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_DOWN  ((OPPONENT_DIRECTION_DOWN<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_NONE  ((5<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_DIRECTION_HIT   ((6<<OPPONENT_FLAGS_DIRECTION_SHIFT))
#define OPPONENT_FLAGS_SPEED_SHIFT     3
#define OPPONENT_FLAGS_SPEED_MASK      ((0x03<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_SLOW      ((0<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_NORMAL    ((1<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_FAST      ((2<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_SPEED_DASH      ((3<<OPPONENT_FLAGS_SPEED_SHIFT))
#define OPPONENT_FLAGS_ALGORITHM_SHIFT            5
#define OPPONENT_FLAGS_ALGORITHM_MASK             ((0x03<<OPPONENT_FLAGS_ALGORITHM_SHIFT))
#define OPPONENT_FLAGS_ALGORITHM_FOLLOW_PLAYER    ((0<<OPPONENT_FLAGS_ALGORITHM_SHIFT))
#define OPPONENT_FLAGS_ALGORITHM_BUNTOP_PATROLLER ((1<<OPPONENT_FLAGS_ALGORITHM_SHIFT))
#define OPPONENT_FLAGS_MAD_SHIFT   7
#define OPPONENT_FLAGS_MAD         ((0x01<<OPPONENT_FLAGS_MAD_SHIFT))


/* Opponent randomness. */
#define OPPONENT_RANDOMNESS_MINIMAL 0xff
#define OPPONENT_RANDOMNESS_NORMAL  0x3f
#define OPPONENT_RANDOMNESS_MEDIUM  0x0f
#define OPPONENT_RANDOMNESS_HIGH    0x03
#define OPPONENT_MAD_RANDOMNESS     0x01
#define OPPONENT_MAD_RESET_PHASE    0x00ff


/* For animation when opponent is hit. */
#define OPPONENT_HIT_SPEED_X        2
#define OPPONENT_START_HIT_SPEED_Y -8


/* Opponent/Player collision distance. */
#define OPPONENT_PLAYER_COLLISION_DISTANCE_FLOOR  4
#define OPPONENT_PLAYER_COLLISION_DISTANCE_ON_FLOOR  8
#define OPPONENT_PLAYER_COLLISION_DISTANCE_LADDER 2
#define OPPONENT_PLAYER_COLLISION_DISTANCE_ON_LADDER 8


typedef struct {
	uint8_t flags;
	uint8_t sprite;
	union {
		position_t target;
		int8_t hit_speed;
	} info;
} opponent_t;

extern opponent_t Opponent[OPPONENT_MAX];
extern position_t OpponentStartPosition[OPPONENT_START_POSITION_MAX];
extern uint16_t OpponentAttackWaves;
extern uint8_t OpponentRandomness;


void unmapOpponents(void);
void resetOpponents(void);
void nextOpponent(void);
void nextAttackWave(void);
void changeOpponentDirectionWithoutAnimationReset(uint8_t index, uint8_t direction);
void changeOpponentDirection(uint8_t index, uint8_t direction);
void selectOpponentDirection(uint8_t index);
void moveOpponent(uint8_t index);
void removeOpponentIfHit(uint8_t index);
void kickOpponent(uint8_t index);
uint8_t checkOpponentCaughtPlayer(uint8_t index);


#endif /* OPPONENTS_H */
