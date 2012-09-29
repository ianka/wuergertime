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
#define LEVEL_ITEM_INVALID              0x00
#define LEVEL_ITEM_BURGER_PLACEHOLDER   0x80
#define LEVEL_ITEM_BURGER_BUNTOP        0x81
#define LEVEL_ITEM_BURGER_TOMATO        0x82
#define LEVEL_ITEM_BURGER_PATTY         0x83
#define LEVEL_ITEM_BURGER_CHEESESALAD   0x84
#define LEVEL_ITEM_BURGER_BUNBOTTOM     0x85
#define LEVEL_ITEM_PLATE                0x86
#define LEVEL_ITEM_SIGN                 0x87
#define LEVEL_ITEM_OPTIONS              0xa0
#define LEVEL_ITEM_OPTION_STOMP_ONCE    0x02
#define LEVEL_ITEM_OPTION_STOMP_TWICE   0x01
#define LEVEL_ITEM_OPTION_STOMP_THREETIMES 0x00
#define LEVEL_ITEM_LADDER               0xc0
#define LEVEL_ITEM_LADDER_TOBOTTOM      0x10
#define LEVEL_ITEM_LADDER_CONTINUED     0x20
#define LEVEL_ITEM_LADDER_LENGTH        0x0f
#define LEVEL_ITEM_FLOOR                0x00
#define LEVEL_ITEM_FLOOR_CAP_LEFT       0x40 
#define LEVEL_ITEM_FLOOR_CAP_RIGHT      0x20 
#define LEVEL_ITEM_FLOOR_CAP_BOTH       ((LEVEL_ITEM_FLOOR_CAP_LEFT|LEVEL_ITEM_FLOOR_CAP_RIGHT))
#define LEVEL_ITEM_FLOOR_LENGTH         0x1f 

typedef struct { uint8_t c, x, y; } PROGMEM level_item_t;


/* Levels */
extern const level_item_t Levels[] PROGMEM;


/* Start animation phases. */
#define LEVEL_START_ANIMATION_FLOORS_ENDED ((SCREEN_WIDTH+10))
#define LEVEL_START_ANIMATION_LADDERS_ENDED ((LEVEL_START_ANIMATION_FLOORS_ENDED+SCREEN_HEIGHT+10))
#define LEVEL_START_ANIMATION_SIGNFRAME_ENDED ((LEVEL_START_ANIMATION_LADDERS_ENDED+10))
#define LEVEL_START_ANIMATION_SIGN_ENDED ((LEVEL_START_ANIMATION_SIGNFRAME_ENDED+63))
#define LEVEL_START_ANIMATION_SIGN_BLINKCODE 0xff530501
#define LEVEL_START_ANIMATION_BURGERS_ENDED ((LEVEL_START_ANIMATION_SIGN_ENDED+100))
#define LEVEL_START_ANIMATION_ENDED LEVEL_START_ANIMATION_BURGERS_ENDED


/*
 *  Game screens numbers. Level screen numbers are composed
 *  from actual level number + in-game screen type.
 */
#define GAME_LEVELS 100

#define GAME_SCREEN_INGAME            0x00
#define GAME_SCREEN_OUTOFGAME         0x80
#define GAME_SCREEN_TILESET0          0x00
#define GAME_SCREEN_TILESET1          0x40

#define GAME_SCREEN_LEVEL_DESCRIPTION ((GAME_SCREEN_TILESET1|0))
#define GAME_SCREEN_LEVEL_PREPARE     ((GAME_SCREEN_TILESET0|1))
#define GAME_SCREEN_LEVEL_START       ((GAME_SCREEN_TILESET0|2))
#define GAME_SCREEN_LEVEL_PLAY        ((GAME_SCREEN_TILESET0|3))
#define GAME_SCREEN_LEVEL_HURRY       ((GAME_SCREEN_TILESET0|4))
#define GAME_SCREEN_LEVEL_BONUS       ((GAME_SCREEN_TILESET0|5))
#define GAME_SCREEN_LEVEL_LOSE        ((GAME_SCREEN_TILESET0|6))
#define GAME_SCREEN_LEVEL_WIN         ((GAME_SCREEN_TILESET0|7))
#define GAME_SCREEN_LEVEL_AFTERMATH   ((GAME_SCREEN_TILESET1|8))

#define GAME_SCREEN_DEBUG             ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|0))
#define GAME_SCREEN_START             ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|1))
#define GAME_SCREEN_CREDITS           ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|2))
#define GAME_SCREEN_DEMO              ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|3))
#define GAME_SCREEN_HIGHSCORES        ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|4))
#define GAME_SCREEN_NEW_HIGHSCORE     ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|5))
#define GAME_SCREEN_INVALID           ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|6)) 


/* Game screen switch, level number, animation phase and update function pointer. */ 
extern uint8_t GameScreenPrevious;
extern uint8_t GameScreen;
extern uint16_t GameScreenAnimationPhase;
extern void (*GameScreenUpdateFunction)(void);
extern uint8_t Level;


/* Switch to new game screen. */
static inline void ChangeGameScreen(uint8_t screen) {
	GameScreen=screen;
}


void selectLevel(uint8_t level);
void prepareLevel(void);
void animateLevelStart(void);
void stomp(uint8_t x, uint8_t y);

#endif /* SCREENS_H */
