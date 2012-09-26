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
 *  screens.h - screen definitions
 */


#ifndef SCREENS_H
#define SCREENS_H


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */


/* Items in levels. */
#define LEVEL_ITEM_SIGN             0x80
#define LEVEL_ITEM_LADDER           0xc0
#define LEVEL_ITEM_LADDER_CONTINUED 0x20
#define LEVEL_ITEM_LADDER_LENGTH    0x1f
#define LEVEL_ITEM_FLOOR            0x00
#define LEVEL_ITEM_FLOOR_CAP_LEFT   0x40 
#define LEVEL_ITEM_FLOOR_CAP_RIGHT  0x20 
#define LEVEL_ITEM_FLOOR_CAP_BOTH   ((LEVEL_ITEM_FLOOR_CAP_LEFT|LEVEL_ITEM_FLOOR_CAP_RIGHT))
#define LEVEL_ITEM_FLOOR_LENGTH     0x1f 

typedef struct { uint8_t c, x, y; } PROGMEM level_item_t;


/* Levels */
extern const level_item_t Levels[] PROGMEM;



/*
 *  Game screens numbers. Level screen numbers are composed
 *  from actual level number + in-game screen type.
 */
#define GAME_LEVELS 32

#define GAME_SCREEN_LEVEL_MASK    0x1f
#define GAME_SCREEN_LEVEL_PREPARE 0x00
#define GAME_SCREEN_LEVEL_START   0x20
#define GAME_SCREEN_LEVEL_PLAY    0x40
#define GAME_SCREEN_LEVEL_LOSE    0x60
#define GAME_SCREEN_LEVEL_HURRY   0x80
#define GAME_SCREEN_LEVEL_BONUS   0xa0
#define GAME_SCREEN_LEVEL_WIN     0xc0

#define GAME_SCREEN_DEBUG         0xe0 
#define GAME_SCREEN_START         0xe1 
#define GAME_SCREEN_CREDITS       0xe2
#define GAME_SCREEN_DEMO          0xe3
#define GAME_SCREEN_HIGHSCORES    0xe4
#define GAME_SCREEN_NEW_HIGHSCORE 0xe5
#define GAME_SCREEN_INVALID       0xff 


/* Game screen switch, animation phase and update function pointer. */ 
extern uint8_t GameScreenPrevious;
extern uint8_t GameScreen;
extern uint16_t GameScreenAnimationPhase;
extern void (*GameScreenUpdateFunction)(void);


/* Switch to new game screen. */
static inline void ChangeGameScreen(uint8_t screen) {
	GameScreen=screen;
}


/* Prepare a level. */
void prepareLevel(uint8_t level, uint8_t length_tweak);


#endif /* SCREENS_H */
