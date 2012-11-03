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
#include <stdlib.h> /* for abs() */


/* Local includes. */
#include "utils.h"
#include "player.h"
#include "opponents.h"
#include "sprites.h"
#include "screens.h"
#include "draw.h"


/* Opponent data. */
opponent_t Opponent[OPPONENT_MAX];
position_t OpponentStartPosition[OPPONENT_START_POSITION_MAX];
uint8_t OpponentsInLevel;
uint16_t OpponentAttackWaves;
uint8_t OpponentRandomness;


const uint8_t OpponentRandomnessMasks[] PROGMEM = {
	OPPONENT_RANDOMNESS_MINIMAL,
	OPPONENT_RANDOMNESS_NORMAL,
	OPPONENT_RANDOMNESS_MEDIUM,
	OPPONENT_RANDOMNESS_HIGH,
};


/* Reset opponents to start position. */
void resetOpponents(void) {
	uint8_t i;

	/* Set opponent randomness from level description. */
	OpponentRandomness=pgm_read_byte(&OpponentRandomnessMasks[(GameScreenOptions & LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_MASK)>>LEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_SHIFT]);

	/* Clear opponent slots and unmap sprites. */
	for (i=0;i<OPPONENT_MAX;i++) {
		Opponent[i].flags=OPPONENT_FLAGS_INVALID;
		unmapSprite(Opponent[i].sprite);
	}	

	/* Start first attack wave. */
	for (i=0;i<OPPONENT_MAX;i++)
		nextOpponent();
}


/* Create new opponent, if there isn't the maximum active for this level. */
void nextOpponent(void) {
	uint8_t i,o;
	uint16_t s;

	/* Count occupied opponent slots. */
	o=0;
	for (i=0;i<OPPONENT_MAX;i++)
		if (Opponent[i].flags != OPPONENT_FLAGS_INVALID) o++;

	/* Skip if selected number of opponents is already active. */
	if (o>((GameScreenOptions & LEVEL_ITEM_OPTION_OPPONENT_MASK)>>LEVEL_ITEM_OPTION_OPPONENT_SHIFT)) return;

	/* Get free opponent slot. */
	for (i=0;i<OPPONENT_MAX;i++)
		if (Opponent[i].flags == OPPONENT_FLAGS_INVALID) {
			/* Slot found. Get next opponent type. */
			o=(OpponentAttackWaves & LEVEL_ITEM_ATTACK_WAVE_MASK);

			/* Stuff it back into last position of the attack waves. */
			OpponentAttackWaves>>=LEVEL_ITEM_ATTACK_WAVE_SHIFT;
			OpponentAttackWaves|=(o<<(16-LEVEL_ITEM_ATTACK_WAVE_SHIFT));

			/* Select sprite by opponent type. */
			switch (o) {
				case LEVEL_ITEM_ATTACK_WAVE_EGGHEAD:
					s=SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_RIGHT;
					Opponent[i].flags=OPPONENT_FLAGS_SPEED_SLOW|OPPONENT_FLAGS_DIRECTION_RIGHT|OPPONENT_FLAGS_ALGORITHM_BUNTOP_PATROLLER;
					Opponent[i].info.target=getRandomBurgerComponentPosition(LEVEL_ITEM_BURGER_BUNTOP);
					break;
				case LEVEL_ITEM_ATTACK_WAVE_SAUSAGEMAN:
					s=SPRITE_FLAGS_TYPE_SAUSAGEMAN|SPRITE_FLAGS_DIRECTION_RIGHT;
					Opponent[i].flags=OPPONENT_FLAGS_SPEED_SLOW|OPPONENT_FLAGS_DIRECTION_RIGHT|OPPONENT_FLAGS_ALGORITHM_FOLLOW_PLAYER;
					break;	
				case LEVEL_ITEM_ATTACK_WAVE_MRMUSTARD:
					s=SPRITE_FLAGS_TYPE_MRMUSTARD|SPRITE_FLAGS_DIRECTION_RIGHT;
					Opponent[i].flags=OPPONENT_FLAGS_SPEED_SLOW|OPPONENT_FLAGS_DIRECTION_RIGHT|OPPONENT_FLAGS_ALGORITHM_FOLLOW_PLAYER;
					break;
				default:
					s=SPRITE_FLAGS_TYPE_EGGHEAD|SPRITE_FLAGS_DIRECTION_RIGHT;
					Opponent[i].flags=OPPONENT_FLAGS_SPEED_SLOW|OPPONENT_FLAGS_DIRECTION_RIGHT|OPPONENT_FLAGS_ALGORITHM_FOLLOW_PLAYER;
			}

			/* Setup flags. */
			
			/* Setup opponent sprite. */
			placeSprite(Opponent[i].sprite,
				OpponentStartPosition[i].x*8,
				OpponentStartPosition[i].y*8,
				s);

			/* Opponent setup complete. */
			return;
		}
}


/* Next opponent at certain animation phases. */
void nextAttackWave(void) {
	if (!(GameScreenAnimationPhase & ~(0xffff<<(((GameScreenOptions & LEVEL_ITEM_OPTION_ATTACK_WAVE_MASK)>>LEVEL_ITEM_OPTION_ATTACK_WAVE_SHIFT)+OPPONENT_ATTACK_WAVE_DEFAULT_SHIFT))))
		nextOpponent();
}	


/* Change moving/facing direction of opponent. */
void changeOpponentDirectionWithoutAnimationReset(uint8_t index, uint8_t direction) {
	Opponent[index].flags=(Opponent[index].flags & ~OPPONENT_FLAGS_DIRECTION_MASK)|direction;
}	

void changeOpponentDirection(uint8_t index, uint8_t direction) {
	/* Skip if direction is same as before. */
	if ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) == direction) return;

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


/* Select a possible opponent direction randomly. */
uint8_t selectPossibleOpponentDirectionRandomly(uint8_t index, uint8_t directions) {
	uint8_t d, e, s;

	/* Set mad flag for this opponent. This avoids opponents stick in a dead end. */
	Opponent[index].flags|=OPPONENT_FLAGS_MAD;

	/* Get number of other possible directions. */
	s=0;
	d=directions;
	for (e=0;e<4;e++) {
		if (d & 0x01) s++;
		d>>=1;
	}

	/* TODO: s may not be 0 (no direction possible) here. */

	/* Skip a random number (out of 1,2,3) of those possible directions. Return direction. */
	s=fastrandom()%s;
	
	/* Go through all directions. */
	for (e=0;e<4;e++) {
		/* Check if possible direction found. */
		if (directions & 0x01) {
			/* Yes. Return if enough skipped. */
			if (s==0) return (e<<OPPONENT_FLAGS_DIRECTION_SHIFT);

			/* Skip one less. */
			s--;
		}

		/* Next possible direction bit. */
		directions>>=1;
	}

	/* TODO: This is never reached. */
	return 0;
}


/* Select a possible opponent direction randomly, prefer current direction. */
uint8_t selectPossibleOpponentDirection(uint8_t index, uint8_t directions) {
	/* Stay in current direction if possible. */
	if ((1<<((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK)>>OPPONENT_FLAGS_DIRECTION_SHIFT)) & directions) return (Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK);

	/* Select a possible opponent direction randomly. */
	return selectPossibleOpponentDirectionRandomly(index,directions);
}


/* Select direction leading nearer to target position. */
void selectOpponentDirectionNearerToTarget(uint8_t index, uint8_t directions, uint8_t x, uint8_t y) {
	uint8_t r;

	/* Make it a little random if moving to target or randomly. */
	r=fastrandom() & OpponentRandomness;

	/* Make opponent movement more random if mad flag was set. */
	if (Opponent[index].flags & OPPONENT_FLAGS_MAD) r&=OPPONENT_MAD_RANDOMNESS;

	/* Choose direction randomly? */
	if (!r) {
		/* Yes, but prefer current direction. */
		changeOpponentDirection(index,selectPossibleOpponentDirection(index,directions));
		return;
	}	

	/* Not randomly. On same floor as target? */
	if (y == getSpriteY(Opponent[index].sprite)) {
		/* Yes. Try to catch the target on this floor. */
		if ((x < getSpriteX(Opponent[index].sprite)) && (directions & (1<<OPPONENT_DIRECTION_LEFT)))
			changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);
		else if ((x > getSpriteX(Opponent[index].sprite)) && (directions & (1<<OPPONENT_DIRECTION_RIGHT)))
			changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);
		else
			/* Not possible to catch the target on this floor. Choose a possible direction randomly, prefer current direction. */
			changeOpponentDirection(index,selectPossibleOpponentDirection(index,directions));
	}	else {
		/* On same ladder? */
		if (x == getSpriteX(Opponent[index].sprite)) {
			/* Yes. Try to catch the target on this ladder. */
			if ((y < getSpriteY(Opponent[index].sprite)) && (directions & (1<<OPPONENT_DIRECTION_UP)))
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_UP);
			else if ((x > getSpriteX(Opponent[index].sprite)) && (directions & (1<<OPPONENT_DIRECTION_DOWN)))
				changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);
			else
				/* Not possible to catch the target on this ladder. Choose a possible direction randomly, prefer current direction. */
				changeOpponentDirection(index,selectPossibleOpponentDirection(index,directions));
		}	else {
			/* Neither on same floor nor ladder. Check if we are on floor or ladder. */
			switch (Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) {
				case OPPONENT_FLAGS_DIRECTION_LEFT:
				case OPPONENT_FLAGS_DIRECTION_RIGHT:
					/* Currently on a floor. Take the next ladder leading in the right direction. */
					if ((y < getSpriteY(Opponent[index].sprite))
							&& (directions & (1<<OPPONENT_DIRECTION_UP)))
						changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_UP);
					else if ((y > getSpriteY(Opponent[index].sprite))
							&& (directions & (1<<OPPONENT_DIRECTION_DOWN)))
						changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_DOWN);
					else
						/* Not possible to change direction on ladder. Choose a possible direction randomly, prefer current direction. */
						changeOpponentDirection(index,selectPossibleOpponentDirection(index,directions));
					break;
				case OPPONENT_FLAGS_DIRECTION_UP:
				case OPPONENT_FLAGS_DIRECTION_DOWN:
					/* Currently on a ladder. Take the next floor leading in the right direction. */
					if ((x < getSpriteX(Opponent[index].sprite))
							&& (directions & (1<<OPPONENT_DIRECTION_LEFT)))
						changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_LEFT);
					else if ((x > getSpriteX(Opponent[index].sprite))
							&& (directions & (1<<OPPONENT_DIRECTION_LEFT)))
						changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_RIGHT);
					else
						/* Not possible to change direction on floor. Choose a possible direction randomly, prefer current direction. */
						changeOpponentDirection(index,selectPossibleOpponentDirection(index,directions));
					break;
			}
		}	
	}
}


/* Select a new opponent direction. */
void selectOpponentDirection(uint8_t index) {
	uint8_t i, directions=0;

	/* Skip invalid opponent. */
	if (Opponent[index].flags == OPPONENT_FLAGS_INVALID) return;

	/* Check possible directions depending on current position. */
	switch (Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) {
		case OPPONENT_FLAGS_DIRECTION_LEFT:
		case OPPONENT_FLAGS_DIRECTION_RIGHT:
			/* Walking on a floor. Check for floor ends. */
			directions|=((checkSpriteAtLeftFloorEnd(Opponent[index].sprite))^1)<<OPPONENT_DIRECTION_LEFT;
			directions|=((checkSpriteAtRightFloorEnd(Opponent[index].sprite))^1)<<OPPONENT_DIRECTION_RIGHT;

			/* Check if we could step on a ladder */
			directions|=(checkSpriteAtLadderEntryUp(Opponent[index].sprite))<<OPPONENT_DIRECTION_UP;
			directions|=(checkSpriteAtLadderEntryDown(Opponent[index].sprite))<<OPPONENT_DIRECTION_DOWN;
			break;
		case OPPONENT_FLAGS_DIRECTION_UP:
		case OPPONENT_FLAGS_DIRECTION_DOWN:
			/* Climbing/stepping down a ladder. Check for ladder end. */
			directions|=((checkSpriteAtLadderTop(Opponent[index].sprite))^1)<<OPPONENT_DIRECTION_UP;
			directions|=((checkSpriteAtLadderBottom(Opponent[index].sprite))^1)<<OPPONENT_DIRECTION_DOWN;

			/* Check if we could step on a floor. */
			if (checkSpriteAtLadderExit(Opponent[index].sprite)) {
				directions|=((checkSpriteAtLeftFloorEnd(Opponent[index].sprite))^1)<<OPPONENT_DIRECTION_LEFT;
				directions|=((checkSpriteAtRightFloorEnd(Opponent[index].sprite))^1)<<OPPONENT_DIRECTION_RIGHT;
			}
			break;
		case OPPONENT_FLAGS_DIRECTION_HIT:
			/* Do not select a new direction once an opponent is hit. */
			return;
	}

	/*
	 *  Check if this opponent has nearly the same coordinates
	 *  as another opponent with the same direction and algorithm.
	 */
	for (i=0;i<OPPONENT_MAX;i++) {
		/* Skip self */
		if (i==index) continue;

		/* Skip if different direction or algorithm. */
		if ((Opponent[index].flags & (OPPONENT_FLAGS_DIRECTION_MASK|OPPONENT_FLAGS_ALGORITHM_MASK))
				!= (Opponent[i].flags & (OPPONENT_FLAGS_DIRECTION_MASK|OPPONENT_FLAGS_ALGORITHM_MASK)))
			continue;

		/* Check coordinates. */
		if (((getSpriteX(Opponent[index].sprite) & 0xf0) == ((getSpriteX(Opponent[i].sprite) & 0xf0)))
				&& ((getSpriteY(Opponent[index].sprite) & 0xf0) == ((getSpriteY(Opponent[i].sprite) & 0xf0)))) {
			/* Same. Change oppenent direction randomly. */
			changeOpponentDirection(index,selectPossibleOpponentDirectionRandomly(index,directions));
			return;
		}	
	}	

	/* Feed possible directions into movement algorithm. */
	switch (Opponent[index].flags & OPPONENT_FLAGS_ALGORITHM_MASK) {
		case OPPONENT_FLAGS_ALGORITHM_FOLLOW_PLAYER:
			/* Target position is player position. */
			selectOpponentDirectionNearerToTarget(index,directions,
				getSpriteX(Player.sprite),getSpriteY(Player.sprite));

			/* Reset mad flag every other animation phase. */
			if (!(GameScreenAnimationPhase & OPPONENT_MAD_RESET_PHASE))
				Opponent[index].flags&=~OPPONENT_FLAGS_MAD;

			break;
		case OPPONENT_FLAGS_ALGORITHM_BUNTOP_PATROLLER:
			/* Target reached? */
			if (((getSpriteX(Opponent[index].sprite) & 0xf0) == (Opponent[index].info.target.x & 0xf0))
					&& ((getSpriteY(Opponent[index].sprite) & 0xf0) == (Opponent[index].info.target.y & 0xf0))) {
				/* Yes. Get a new target position, random buntop. */
				Opponent[index].info.target=getRandomBurgerComponentPosition(LEVEL_ITEM_BURGER_BUNTOP);

				/* Reset mad flag. */
				Opponent[index].flags&=~OPPONENT_FLAGS_MAD;
			}

			/* Move nearer to target. */
			selectOpponentDirectionNearerToTarget(index,directions,
				Opponent[index].info.target.x,Opponent[index].info.target.y);
			break;
	}
}



/* Move opponent into selected direction. */
void moveOpponent(uint8_t index) {
	int16_t y;

	/* Skip invalid opponent. */
	if (Opponent[index].flags == OPPONENT_FLAGS_INVALID) return;

	/* Move by direction. */
	switch (Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK) {
		case OPPONENT_FLAGS_DIRECTION_LEFT:
			moveSprite(Opponent[index].sprite,-(1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)),0);
			break;
		case OPPONENT_FLAGS_DIRECTION_RIGHT:
			moveSprite(Opponent[index].sprite,1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT),0);
			break;
		case OPPONENT_FLAGS_DIRECTION_UP:
			moveSprite(Opponent[index].sprite,0,-(1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT)));
			break;
		case OPPONENT_FLAGS_DIRECTION_DOWN:
			moveSprite(Opponent[index].sprite,0,1<<((Opponent[index].flags & OPPONENT_FLAGS_SPEED_MASK)>>OPPONENT_FLAGS_SPEED_SHIFT));
			break;
		case OPPONENT_FLAGS_DIRECTION_HIT:
			/* Move to invalid coordinate? */
			y=getSpriteY(Opponent[index].sprite);
			y+=Opponent[index].info.hit_speed;
			if (y>(SCREEN_HEIGHT*8)) {
				/* Yes. Remove opponent from screen. */
				unmapSprite(Opponent[index].sprite);

				/* Mark opponent as removed. */
				Opponent[index].flags=OPPONENT_FLAGS_INVALID;
			} else {
				/* No. Move with hit speed. */
				moveSpriteUncondionally(Opponent[index].sprite,OPPONENT_HIT_SPEED_X,Opponent[index].info.hit_speed);

				/* Turn hit speed from negative to more positive for next step. */
				Opponent[index].info.hit_speed++;
			}	
			break;
	}
}


/* Remove an opponent if it is hit by a burger component. */
void removeOpponentIfHit(uint8_t index) {
	/* Skip invalid and hit opponent. */
	if ((Opponent[index].flags == OPPONENT_FLAGS_INVALID)
			|| ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK)) == OPPONENT_FLAGS_DIRECTION_HIT)
		return 0;

	/* Check if opponent is hit by any burger component. */
	if (checkFallingBurgerComponentPosition(getSpriteX(Opponent[index].sprite),getSpriteY(Opponent[index].sprite))) {
		/* Yes. Kick it from the screen. */
		changeOpponentDirection(index,OPPONENT_FLAGS_DIRECTION_HIT);

		/* Initialize hit speed. */
		Opponent[index].info.hit_speed=min(getSpriteY(Opponent[index].sprite)*getSpriteY(Opponent[index].sprite),OPPONENT_START_HIT_SPEED_Y);

		/* Score for hit opponent. */
		Score+=SCORE_OPPONENT_HIT;
	}
}


/* Check if opponent has caught player. */
uint8_t checkOpponentCaughtPlayer(uint8_t index) {
	int16_t px, py, ox, oy;

	/* Skip invalid and hit opponent. */
	if ((Opponent[index].flags == OPPONENT_FLAGS_INVALID)
			|| ((Opponent[index].flags & OPPONENT_FLAGS_DIRECTION_MASK)) == OPPONENT_FLAGS_DIRECTION_HIT)
		return 0;

	/* Get coordinates. */
	px=getSpriteX(Player.sprite);
	py=getSpriteY(Player.sprite);
	ox=getSpriteX(Opponent[index].sprite);
	oy=getSpriteY(Opponent[index].sprite);

	/* Check if the opponent is roughly on same floor as the player. */
	if (abs(py-oy) <= OPPONENT_PLAYER_COLLISION_DISTANCE_FLOOR) {
		/* Same floor. Check position on floor. */
		return (abs(px-ox) <= OPPONENT_PLAYER_COLLISION_DISTANCE_ON_FLOOR);
	}

	/* Check if the opponent is roughly on same ladder as the player. */
	if (abs(px-ox) <= OPPONENT_PLAYER_COLLISION_DISTANCE_LADDER) {
		/* Same ladder. Check position on ladder. */
		return (abs(py-oy) <= OPPONENT_PLAYER_COLLISION_DISTANCE_ON_LADDER);
	}

	/* No collision. */
	return 0;
}
	
