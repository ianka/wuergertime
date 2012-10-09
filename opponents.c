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


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"
#include "opponents.h"
#include "sprites.h"


/* Opponent data. */
opponent_t Opponent;


/* Reset opponent to start position. */
void resetOpponent(void) {
	/* Setup flags. */
	Opponent.flags=OPPONENT_FLAGS_SPEED_NORMAL|OPPONENT_FLAGS_DIRECTION_LEFT;

	/* Setup opponent sprite. */
	placeSprite(Opponent.sprite,
		Opponent.start_position.x*8,
		Opponent.start_position.y*8,
		SPRITE_FLAGS_TYPE_EGGHEAD);
}


/* Change moving/facing direction of opponent. */
void changeOpponentDirectionWithoutAnimationReset(uint8_t direction) {
	Opponent.flags=(Opponent.flags & ~OPPONENT_FLAGS_DIRECTION_MASK)|direction;
}	

void changeOpponentDirection(uint8_t direction) {
	/* Remember new direction. */
	changeOpponentDirectionWithoutAnimationReset(direction);

	/* Change sprite direction. */
	switch (direction) {
		case OPPONENT_FLAGS_DIRECTION_LEFT:
			changeSpriteDirection(Opponent.sprite,SPRITE_FLAGS_DIRECTION_LEFT);
			break;
		case OPPONENT_FLAGS_DIRECTION_RIGHT:
			changeSpriteDirection(Opponent.sprite,SPRITE_FLAGS_DIRECTION_RIGHT);
			break;
		default:	
			changeSpriteDirection(Opponent.sprite,SPRITE_FLAGS_DIRECTION_LADDER);
	}
}


/* Select a opponent direction from held buttons. */
void selectOpponentDirection(uint8_t buttons) {
	switch (buttons) {
		case BTN_LEFT:
			/* Skip if we already are on our way left. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_LEFT)
				break;

			/* Change direction on floor if opponent direction is currently right. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_RIGHT)
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_LEFT);

			/* Change direction from ladder to floor if opponent is at a ladder exit. */
			if (checkSpriteAtLadderExit(Opponent.sprite))
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_LEFT);

			break;	
		case BTN_RIGHT:
			/* Skip if we already are on our way right. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_RIGHT)
				break;

			/* Change direction on floor if opponent direction is currently left. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_LEFT)
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_RIGHT);

			/* Change direction from ladder to floor if opponent is at a ladder exit. */
			if (checkSpriteAtLadderExit(Opponent.sprite))
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_RIGHT);

			break;
		case BTN_DOWN:
			/* Skip if we already are on our way down. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_DOWN)
				break;

			/* Change direction on ladder if opponent direction is currently up. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_UP)
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_DOWN);

			/* Change direction from floor to ladder if opponent is onto a ladder top. */
			if (checkSpriteAtLadderEntryDown(Opponent.sprite))
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_DOWN);

			break;
		case BTN_UP:
			/* Skip if we already are on our way up. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_UP)
				break;
			
			/* Change direction on ladder if opponent direction is currently down. */
			if ((Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) == OPPONENT_FLAGS_DIRECTION_DOWN)
				changeOpponentDirectionWithoutAnimationReset(OPPONENT_FLAGS_DIRECTION_UP);

			/* Change direction from floor to ladder if opponent is at a ladder. */
			if (checkSpriteAtLadderEntryUp(Opponent.sprite))
				changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_UP);

			break;
	}
}


/* Move opponent into selected direction, if possible. */
void moveOpponent(uint8_t buttons) {
	/* Walk into current direction, if any button but the opposite is held. */
	switch (Opponent.flags & OPPONENT_FLAGS_DIRECTION_MASK) {
		case OPPONENT_FLAGS_DIRECTION_LEFT:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLeftFloorEnd(Opponent.sprite))
					moveSprite(Opponent.sprite,-(1<<((Opponent.flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)),0);
				/* Change direction from floor to ladder if opponent is at a ladder. */
				else if (checkSpriteAtLadderEntryUp(Opponent.sprite))
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_UP);
				else if (checkSpriteAtLadderEntryDown(Opponent.sprite))	
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_DOWN);
				else
					/* Change direction on floor if we are at a dead end. */
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_RIGHT);
			break;
		case OPPONENT_FLAGS_DIRECTION_RIGHT:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtRightFloorEnd(Opponent.sprite))
				moveSprite(Opponent.sprite,(1<<((Opponent.flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)),0);
				/* Change direction from floor to ladder if opponent is at a ladder. */
				else if (checkSpriteAtLadderEntryUp(Opponent.sprite))
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_UP);
				else if (checkSpriteAtLadderEntryDown(Opponent.sprite))	
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_DOWN);
				else
					/* Change direction on floor if we are at a dead end. */
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_LEFT);
			break;
		case OPPONENT_FLAGS_DIRECTION_DOWN:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLadderBottom(Opponent.sprite))
				moveSprite(Opponent.sprite,0,(1<<((Opponent.flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)));
				/* Change direction from ladder to floor if opponent is at a ladder exit. */
				else if (!checkSpriteAtRightFloorEnd(Opponent.sprite))
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_RIGHT);
				else if (!checkSpriteAtLeftFloorEnd(Opponent.sprite))
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_LEFT);
				else	
					/* Change direction on ladder if we are at a dead end. */
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_UP);
			break;
		case OPPONENT_FLAGS_DIRECTION_UP:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLadderTop(Opponent.sprite))
				moveSprite(Opponent.sprite,0,-(1<<((Opponent.flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)));
				/* Change direction from ladder to floor if opponent is at a ladder exit. */
				else if (!checkSpriteAtLeftFloorEnd(Opponent.sprite))
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_LEFT);
				else if (!checkSpriteAtRightFloorEnd(Opponent.sprite))
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_RIGHT);
				else	
					/* Change direction on ladder if we are at a dead end. */
					changeOpponentDirection(OPPONENT_FLAGS_DIRECTION_DOWN);
			break;
	}
}
