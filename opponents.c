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
 *  oppenents.c - opponent functions.
 */


#include <avr/io.h> /* for uint8_t and uint16_t */


/* Local includes. */
#include "utils.h"
#include "opponents.h"
#include "sprites.h"
#include "screens.h"


/* Opponent data. */
opponent_t Opponent[OPPONENT_MAX];
position_t OpponentStartPosition[OPPONENT_START_POSITION_MAX];
uint8_t OpponentsInLevel;
uint16_t OpponentAttackWaves;


/* Reset opponents to start position. */
void resetOpponents(void) {
	uint8_t i, o;
	uint16_t s;

	/* Get first attack wave. */
	for (i=0;i<((GameScreenOptions & LEVEL_ITEM_OPTION_OPPONENT_MASK)>>LEVEL_ITEM_OPTION_OPPONENT_SHIFT)+1;i++) { 
		/* Get next opponent type. */
		o=(OpponentAttackWaves & LEVEL_ITEM_ATTACK_WAVE_MASK);

		/* Stuff it back into last position of attack wave. */
		OpponentAttackWaves>>=LEVEL_ITEM_ATTACK_WAVE_SHIFT;
		OpponentAttackWaves|=(o<<(16-LEVEL_ITEM_ATTACK_WAVE_SHIFT));

		/* Select sprite by opponent type. */
		switch (o) {
			case LEVEL_ITEM_ATTACK_WAVE_EGGHEAD:
				s=SPRITE_FLAGS_TYPE_EGGHEAD;
				break;
			case LEVEL_ITEM_ATTACK_WAVE_SAUSAGEMAN:
				s=SPRITE_FLAGS_TYPE_SAUSAGEMAN;
				break;	
			case LEVEL_ITEM_ATTACK_WAVE_MRMUSTARD:
				s=SPRITE_FLAGS_TYPE_MRMUSTARD;
				break;
			default:
				s=SPRITE_FLAGS_TYPE_EGGHEAD;
		}

		/* Setup flags. */
		Opponent[i].flags=OPPONENT_FLAGS_SPEED_SLOW|OPPONENT_FLAGS_DIRECTION_LEFT;

		/* Setup opponent sprite. */
		placeSprite(Opponent[i].sprite,
			OpponentStartPosition[i].x*8,
			OpponentStartPosition[i].y*8,
			s);
	}
}


/* Change moving/facing direction of opponent. */
void changeOpponentDirectionWithoutAnimationReset(uint8_t index, uint8_t direction) {
	Opponent[index].flags=(Opponent[index].flags & ~OPPONENT_FLAGS_DIRECTION_MASK)|direction;
}	

void changeOpponentDirection(uint8_t index, uint8_t direction) {
	/* Remember new direction. */
	changeOpponentDirectionWithoutAnimationReset(index,direction);

	/* Change sprite direction. */
	switch (direction) {
		case OPPONENT_FLAGS_DIRECTION_LEFT:
			changeSpriteDirection(Opponent[index].sprite,SPRITE_FLAGS_DIRECTION_LEFT);
			break;
		case OPPONENT_FLAGS_DIRECTION_RIGHT:
			changeSpriteDirection(Opponent[index].sprite,SPRITE_FLAGS_DIRECTION_RIGHT);
			break;
		default:	
			changeSpriteDirection(Opponent[index].sprite,SPRITE_FLAGS_DIRECTION_LADDER);
	}
}


/* Select a opponent direction from held buttons. */
void selectOpponentDirection(uint8_t index, uint8_t buttons) {
	switch (buttons) {
		case BTN_LEFT:
			/* Skip if we already are on our way left. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_LEFT)
				break;

			/* Change direction on floor if opponent direction is currently right. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_RIGHT)
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);

			/* Change direction from ladder to floor if opponent is at a ladder exit. */
			if (checkSpriteAtLadderExit(Opponent[index].sprite))
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);

			break;	
		case BTN_RIGHT:
			/* Skip if we already are on our way right. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_RIGHT)
				break;

			/* Change direction on floor if opponent direction is currently left. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_LEFT)
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);

			/* Change direction from ladder to floor if opponent is at a ladder exit. */
			if (checkSpriteAtLadderExit(Opponent[index].sprite))
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);

			break;
		case BTN_DOWN:
			/* Skip if we already are on our way down. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_DOWN)
				break;

			/* Change direction on ladder if opponent direction is currently up. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_UP)
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);

			/* Change direction from floor to ladder if opponent is onto a ladder top. */
			if (checkSpriteAtLadderEntryDown(Opponent[index].sprite))
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);

			break;
		case BTN_UP:
			/* Skip if we already are on our way up. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_UP)
				break;
			
			/* Change direction on ladder if opponent direction is currently down. */
			if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_DOWN)
				changeOpponentDirectionWithoutAnimationReset(index,OPPONENT_FLAGS_DIRECTION_UP);

			/* Change direction from floor to ladder if opponent is at a ladder. */
			if (checkSpriteAtLadderEntryUp(Opponent[index].sprite))
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_UP);

			break;
	}
}


/* Move opponent into selected direction, if possible. */
void moveOpponent(uint8_t index, uint8_t buttons) {
	/* Walk into current direction, if any button but the opposite is held. */
	switch (Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) {
		case OPPONENT_FLAGS_DIRECTION_LEFT:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLeftFloorEnd(Opponent[index].sprite))
					moveSprite(Opponent[index].sprite,-(1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)),0);
				/* Change direction from floor to ladder if opponent is at a ladder. */
				else if (checkSpriteAtLadderEntryUp(Opponent[index].sprite))
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_UP);
				else if (checkSpriteAtLadderEntryDown(Opponent[index].sprite))	
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);
				else
					/* Change direction on floor if we are at a dead end. */
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);
			break;
		case OPPONENT_FLAGS_DIRECTION_RIGHT:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtRightFloorEnd(Opponent[index].sprite))
				moveSprite(Opponent[index].sprite,(1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)),0);
				/* Change direction from floor to ladder if opponent is at a ladder. */
				else if (checkSpriteAtLadderEntryUp(Opponent[index].sprite))
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_UP);
				else if (checkSpriteAtLadderEntryDown(Opponent[index].sprite))	
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);
				else
					/* Change direction on floor if we are at a dead end. */
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);
			break;
		case OPPONENT_FLAGS_DIRECTION_DOWN:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLadderBottom(Opponent[index].sprite))
				moveSprite(Opponent[index].sprite,0,(1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)));
				/* Change direction from ladder to floor if opponent is at a ladder exit. */
				else if (!checkSpriteAtRightFloorEnd(Opponent[index].sprite))
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);
				else if (!checkSpriteAtLeftFloorEnd(Opponent[index].sprite))
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);
				else	
					/* Change direction on ladder if we are at a dead end. */
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_UP);
			break;
		case OPPONENT_FLAGS_DIRECTION_UP:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLadderTop(Opponent[index].sprite))
				moveSprite(Opponent[index].sprite,0,-(1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)));
				/* Change direction from ladder to floor if opponent is at a ladder exit. */
				else if (!checkSpriteAtLeftFloorEnd(Opponent[index].sprite))
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);
				else if (!checkSpriteAtRightFloorEnd(Opponent[index].sprite))
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);
				else	
					/* Change direction on ladder if we are at a dead end. */
					changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);
			break;
	}
}
