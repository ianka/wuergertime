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
 *  screens.h - screen definitions
 */


#ifndef SCREENS_H
#define SCREENS_H


#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */


/* Local includes. */
#include "utils.h"


/* Number of lives at beginning of game. */
#define DEFAULT_LIVES 3

/* Default bonus whenever a life is lost. */
#define DEFAULT_BONUS 999

/* Hurry mode if bonus below. */
#define HURRY_BONUS 250
#define HURRY_ANIMATION_SHIFT 2
#define HURRY_ANIMATION_DRAW_UPPER_LEFT    ((0<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_REMOVE_UPPER_LEFT  ((2<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_DRAW_LOWER_RIGHT   ((4<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_REMOVE_LOWER_RIGHT ((6<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_DRAW_UPPER_RIGHT   ((8<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_REMOVE_UPPER_RIGHT ((10<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_DRAW_LOWER_LEFT    ((12<<HURRY_ANIMATION_SHIFT))
#define HURRY_ANIMATION_REMOVE_LOWER_LEFT  ((14<<HURRY_ANIMATION_SHIFT))


/* Shift for bonus speed. */
#define BONUS_DEFAULT_SHIFT 1


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
#define LEVEL_ITEM_SCORE                0x88
#define LEVEL_ITEM_LEVEL                0x89
#define LEVEL_ITEM_BONUS                0x8a
#define LEVEL_ITEM_LIVES                0x8b
#define LEVEL_ITEM_OPTIONS              0x90
#define LEVEL_ITEM_OPTION_STOMP_SHIFT   0
#define LEVEL_ITEM_OPTION_STOMP_MASK    ((0x03<<LEVEL_ITEM_OPTION_STOMP_SHIFT))
#define LEVEL_ITEM_OPTION_STOMP_ONCE    0x02
#define LEVEL_ITEM_OPTION_STOMP_TWICE   0x01
#define LEVEL_ITEM_OPTION_STOMP_THREETIMES 0x00
#define LEVEL_ITEM_OPTION_OPPONENT_SHIFT 2
#define LEVEL_ITEM_OPTION_OPPONENT_MASK ((0x03<<LEVEL_ITEM_OPTION_OPPONENT_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_SINGLE ((0<<LEVEL_ITEM_OPTION_OPPONENT_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_DUO    ((1<<LEVEL_ITEM_OPTION_OPPONENT_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_TRIO   ((2<<LEVEL_ITEM_OPTION_OPPONENT_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_QUAD   ((3<<LEVEL_ITEM_OPTION_OPPONENT_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT 4
#define LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_MASK ((0x03<<LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_MINIMAL ((0<<LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_NORMAL  ((1<<LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_MEDIUM  ((2<<LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT))
#define LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_HIGH    ((3<<LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT))
#define LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT 6
#define LEVEL_ITEM_OPTION_ATTACK_WAVE_MASK  ((0x03<<LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT))
#define LEVEL_ITEM_OPTION_ATTACK_WAVE_FAST    ((0<<LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT))
#define LEVEL_ITEM_OPTION_ATTACK_WAVE_MEDIUM  ((1<<LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT))
#define LEVEL_ITEM_OPTION_ATTACK_WAVE_SLOW    ((2<<LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT))
#define LEVEL_ITEM_OPTION_ATTACK_WAVE_SLOWEST ((3<<LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT))
#define LEVEL_ITEM_OPTION_BONUS_SHIFT 8
#define LEVEL_ITEM_OPTION_BONUS_MASK  ((0x03<<LEVEL_ITEM_OPTION_BONUS_SHIFT))
#define LEVEL_ITEM_OPTION_BONUS_FAST    ((0<<LEVEL_ITEM_OPTION_BONUS_SHIFT))
#define LEVEL_ITEM_OPTION_BONUS_MEDIUM  ((1<<LEVEL_ITEM_OPTION_BONUS_SHIFT))
#define LEVEL_ITEM_OPTION_BONUS_SLOW    ((2<<LEVEL_ITEM_OPTION_BONUS_SHIFT))
#define LEVEL_ITEM_OPTION_BONUS_SLOWEST ((3<<LEVEL_ITEM_OPTION_BONUS_SHIFT))
#define LEVEL_ITEM_PLAYERSTARTPOINT     0x91
#define LEVEL_ITEM_OPPONENTSTARTPOINT   0x92
#define LEVEL_ITEM_ATTACKWAVES          0x93
#define LEVEL_ITEM_ATTACK_WAVE_SHIFT    2
#define LEVEL_ITEM_ATTACK_WAVE_MASK     0x03
#define LEVEL_ITEM_ATTACK_WAVE_EGGHEAD     1
#define	LEVEL_ITEM_ATTACK_WAVE_SAUSAGEMAN  2
#define	LEVEL_ITEM_ATTACK_WAVE_MRMUSTARD   3
#define LEVEL_ITEM_LADDER               0xc0
#define LEVEL_ITEM_LADDER_SIMPLE        0x00
#define LEVEL_ITEM_LADDER_UPONLY        0x10
#define LEVEL_ITEM_LADDER_CONTINUED     0x20
#define LEVEL_ITEM_LADDER_CONTUPONLY    ((LEVEL_ITEM_LADDER_UPONLY|LEVEL_ITEM_LADDER_CONTINUED))
#define LEVEL_ITEM_LADDER_LENGTH        0x0f
#define LEVEL_ITEM_FLOOR                0x00
#define LEVEL_ITEM_FLOOR_CAP_NONE       0x00
#define LEVEL_ITEM_FLOOR_CAP_LEFT       0x40
#define LEVEL_ITEM_FLOOR_CAP_RIGHT      0x20
#define LEVEL_ITEM_FLOOR_CAP_BOTH       ((LEVEL_ITEM_FLOOR_CAP_LEFT|LEVEL_ITEM_FLOOR_CAP_RIGHT))
#define LEVEL_ITEM_FLOOR_LENGTH         0x1f


typedef struct {
	uint8_t component;
	union {
		position_t position;
		uint16_t options;
	};
} PROGMEM level_item_t;



/* Level components. */
#define LEVEL_COMPONENT_END { component: 0 }
#define LEVEL_COMPONENT_OPTIONS(opts) { component: LEVEL_ITEM_OPTIONS, { options: opts } }
#define LEVEL_COMPONENT_PLAYERSTARTPOINT(xc,yc) { component: LEVEL_ITEM_PLAYERSTARTPOINT, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_OPPONENTSTARTPOINT(xc,yc) { component: LEVEL_ITEM_OPPONENTSTARTPOINT, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_ATTACKWAVES(a,b,c,d,e,f,g,h) { component: LEVEL_ITEM_ATTACKWAVES, { options: ((a|(b<<2)|(c<<4)|(d<<6)|(e<<8)|(f<<10)|(g<<12)|(h<<14))) } }

#define LEVEL_COMPONENT_SIGN(xc,yc) { component: LEVEL_ITEM_SIGN, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_PLATE(xc,yc) { component: LEVEL_ITEM_PLATE, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_FLOOR(type,xc,yc,len) { component: ((LEVEL_ITEM_FLOOR|LEVEL_ITEM_FLOOR_ ## type|len)), { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_LADDER(type,xc,yc,len) { component: ((LEVEL_ITEM_LADDER|LEVEL_ITEM_LADDER_ ## type|len)), { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_BURGER(type,xc,yc) { component: LEVEL_ITEM_BURGER_ ## type, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_SCORE(xc,yc) { component: LEVEL_ITEM_SCORE, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_LEVEL(xc,yc) { component: LEVEL_ITEM_LEVEL, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_BONUS(xc,yc) { component: LEVEL_ITEM_BONUS, { position: { x: xc, y: yc } } }
#define LEVEL_COMPONENT_LIVES(xc,yc) { component: LEVEL_ITEM_LIVES, { position: { x: xc, y: yc } } }


/* Levels */
extern const uint8_t LevelDrawings[] PROGMEM;
extern const level_item_t LevelComponents[] PROGMEM;


/* Start animation phases. */
#define LEVEL_START_ANIMATION_FLOORS_ENDED ((SCREEN_WIDTH+10))
#define LEVEL_START_ANIMATION_LADDERS_ENDED ((LEVEL_START_ANIMATION_FLOORS_ENDED+SCREEN_HEIGHT+10))
#define LEVEL_START_ANIMATION_SIGNFRAME_ENDED ((LEVEL_START_ANIMATION_LADDERS_ENDED+10))
#define LEVEL_START_ANIMATION_SIGN_ENDED ((LEVEL_START_ANIMATION_SIGNFRAME_ENDED+63))
#define LEVEL_START_ANIMATION_SIGN_BLINKCODE 0xff531005
#define LEVEL_START_ANIMATION_SIGN_SHIFT 1
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
#define GAME_SCREEN_LEVEL_GAME_OVER   ((GAME_SCREEN_TILESET0|7))
#define GAME_SCREEN_LEVEL_WIN         ((GAME_SCREEN_TILESET0|8))
#define GAME_SCREEN_LEVEL_AFTERMATH   ((GAME_SCREEN_TILESET1|9))

#define GAME_SCREEN_DEBUG             ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|0))
#define GAME_SCREEN_START             ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|1))
#define GAME_SCREEN_CREDITS           ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|2))
#define GAME_SCREEN_DEMO              ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|3))
#define GAME_SCREEN_HIGHSCORES        ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|4))
#define GAME_SCREEN_NEW_HIGHSCORE     ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|5))
#define GAME_SCREEN_ENTER_HIGHSCORE   ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|6))
#define GAME_SCREEN_ENTERED_HIGHSCORE ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|7))
#define GAME_SCREEN_INVALID           ((GAME_SCREEN_OUTOFGAME|GAME_SCREEN_TILESET1|8))

/* Game screen switch, level number, animation phase and update function pointer. */
extern uint8_t GameScreenPrevious;
extern uint8_t GameScreen;
extern uint16_t GameScreenAnimationPhase;
extern uint16_t GameScreenOptions;
extern void (*GameScreenUpdateFunction)(void);
extern uint8_t Level;


/* Games stats. */
#define SCORE_STOMPED_TILE                1
#define SCORE_COMPONENT_FALLING          10
#define SCORE_COMPONENT_FALLING_CASCADE  20
#define SCORE_OPPONENT_HIT              200

extern uint8_t Scratchpad;
extern uint32_t Score;
extern uint32_t DisplayedScore;
extern uint16_t Bonus;
extern uint8_t Lives;


/* Switch to new game screen. */
static inline void ChangeGameScreen(uint8_t screen) {
	GameScreen=screen;
}


void selectLevel(uint8_t level);
void prepareLevel(void);
uint8_t animateBurgers(void);
void animateLevelStart(void);
void dropComponent(uint8_t burger, uint8_t component);
void dropHattedComponents(void);
uint8_t stomp(uint8_t x, uint8_t y);
position_t getRandomBurgerComponentPosition(uint8_t type);
uint8_t checkFallingBurgerComponentPosition(uint8_t x, uint8_t y);
void updateGameScreenStatistics(void);
uint8_t decrementBonus(void);
uint8_t decrementBonusFast(void);
uint8_t animateHurry(void);
void dropAllBurgersOffScreen(void);
uint8_t allBurgersServed(void);

#endif /* SCREENS_H */
