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
 *  player.c - player functions.
 */


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"
#include "player.h"
#include "sprites.h"
#include "draw.h"
#include "patches.h"


/* Player data. */
player_t Player;


/* Reset player to start position. */
void resetPlayer(void) {
	uint16_t flags;

	/* Setup player flags. */
	Player.flags=PLAYER_FLAGS_SPEED_NORMAL;

	/* Determine player direction. */
	flags=SPRITE_FLAGS_TYPE_COOK;
	if (Player.start_position.x<(SCREEN_WIDTH/2)) {
		/* Face right. */
		flags|=SPRITE_FLAGS_DIRECTION_RIGHT;
		Player.flags|=PLAYER_FLAGS_DIRECTION_RIGHT;
	} else {
		/* Face left. */
		flags|=SPRITE_FLAGS_DIRECTION_LEFT;
		Player.flags|=PLAYER_FLAGS_DIRECTION_LEFT;
	}

	/* Setup player sprite. */
	placeSprite(Player.sprite,
		Player.start_position.x*8+SPRITE_BORDER_WIDTH,
		Player.start_position.y*8,
		flags);
}


/* Change moving/facing direction of player. */
void changePlayerDirectionWithoutAnimationReset(uint8_t direction) {
	Player.flags=(Player.flags & ~PLAYER_FLAGS_DIRECTION_MASK)|direction;
}

void changePlayerDirection(uint8_t direction) {
	/* Remember new direction. */
	changePlayerDirectionWithoutAnimationReset(direction);

	/* Change sprite direction. */
	switch (direction) {
		case PLAYER_FLAGS_DIRECTION_LEFT:
			changeSpriteDirection(Player.sprite,SPRITE_FLAGS_DIRECTION_LEFT);
			break;
		case PLAYER_FLAGS_DIRECTION_RIGHT:
			changeSpriteDirection(Player.sprite,SPRITE_FLAGS_DIRECTION_RIGHT);
			break;
		case PLAYER_FLAGS_DIRECTION_CAUGHT:
			changeSpriteDirection(Player.sprite,SPRITE_FLAGS_DIRECTION_CAUGHT);

			/* Initialize hit speed. */
			Player.hit_speed=min(getSpriteY(Player.sprite)*getSpriteY(Player.sprite),PLAYER_START_HIT_SPEED_Y);
			break;
		case PLAYER_FLAGS_DIRECTION_SLIDE:
			changeSpriteDirection(Player.sprite,SPRITE_FLAGS_DIRECTION_SLIDE);
			break;
		default:
			changeSpriteDirection(Player.sprite,SPRITE_FLAGS_DIRECTION_LADDER);
	}
}


/* Select a player direction from held buttons. */
void selectPlayerDirection(uint8_t buttons) {
	/* Remove button direction the player is already moving to. */
	switch (Player.flags & PLAYER_FLAGS_DIRECTION_MASK) {
		case PLAYER_FLAGS_DIRECTION_LEFT:
			buttons&=~BTN_LEFT;
			break;
		case PLAYER_FLAGS_DIRECTION_RIGHT:
			buttons&=~BTN_RIGHT;
			break;
		case PLAYER_FLAGS_DIRECTION_UP:
		case PLAYER_FLAGS_DIRECTION_CLEAN:
			buttons&=~BTN_UP;
			break;
		case PLAYER_FLAGS_DIRECTION_SLIDE:
			buttons=0;
			break;
		case PLAYER_FLAGS_DIRECTION_DOWN:
			buttons&=~BTN_DOWN;
			break;
	}

	/* Switch by remaining button flag. */
	switch (buttons) {
		case BTN_LEFT:
			/* Change direction on floor if player direction is currently right. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_RIGHT)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);

			/* Change direction from ladder to floor if player is at a ladder exit. */
			if (checkSpriteAtLadderExit(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);

			break;
		case BTN_RIGHT:
			/* Change direction on floor if player direction is currently left. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_LEFT)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			/* Change direction from ladder to floor if player is at a ladder exit. */
			if (checkSpriteAtLadderExit(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			break;
		case BTN_DOWN:
			/* Change direction on ladder if player direction is currently up. */
			switch (Player.flags & PLAYER_FLAGS_DIRECTION_MASK) {
				case PLAYER_FLAGS_DIRECTION_UP:
				case PLAYER_FLAGS_DIRECTION_CLEAN:
					changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);
					break;
			}

			/* Change direction from floor to ladder if player is onto a ladder top. */
			if (checkSpriteAtLadderEntryDown(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);

			break;
		case BTN_UP:
			/* Change direction on ladder if player direction is currently down. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_DOWN)
				changePlayerDirectionWithoutAnimationReset(PLAYER_FLAGS_DIRECTION_UP);

			/* Change direction from floor to ladder if player is at a ladder. */
			if (checkSpriteAtLadderEntryUp(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_UP);

			/* Change direction from floor to ladder if player is at a squirted ladder. */
			if (checkSpriteAtSquirtedLadderEntryUp(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_CLEAN);

			break;
	}
}


/* Move player into selected direction, if possible. */
void movePlayer(uint8_t buttons) {
	/* Walk into current direction, if any button but the opposite is held. */
	switch (Player.flags & PLAYER_FLAGS_DIRECTION_MASK) {
		case PLAYER_FLAGS_DIRECTION_LEFT:
			/* Move sprite if nothing should stop us. */
			if ((buttons & (BTN_LEFT|BTN_UP|BTN_DOWN)) && (!checkSpriteAtLeftFloorEnd(Player.sprite))) {
				/* Move! */
				moveSprite(Player.sprite,-(1<<((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)),0);

				/* Stomp tile under player sprite. */
				stompUnderSprite(Player.sprite);
			}

			break;
		case PLAYER_FLAGS_DIRECTION_RIGHT:
			/* Move sprite if nothing should stop us. */
			if ((buttons & (BTN_RIGHT|BTN_UP|BTN_DOWN)) && (!checkSpriteAtRightFloorEnd(Player.sprite))) {
				/* Move! */
				moveSprite(Player.sprite,(1<<((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)),0);

				/* Stomp tile under player sprite. */
				stompUnderSprite(Player.sprite);
			}

			break;
		case PLAYER_FLAGS_DIRECTION_DOWN:
			/* Change direction from ladder down to slide if player is at a squirted ladder. */
			/* Else, move sprite if nothing should stop us. */
			if (checkSpriteAtSquirtedLadderEntryUp(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_SLIDE);
			else if ((buttons & (BTN_RIGHT|BTN_LEFT|BTN_DOWN)) && (!checkSpriteAtLadderBottom(Player.sprite)))
				moveSprite(Player.sprite,0,(1<<min(0,((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)-1)));

			break;
		case PLAYER_FLAGS_DIRECTION_SLIDE:
			/* Move sprite if nothing should stop us. */
			if (!checkSpriteAtLadderBottom(Player.sprite)) {
				moveSprite(Player.sprite,0,(1<<((PLAYER_FLAGS_SPEED_FAST>>PLAYER_FLAGS_SPEED_SHIFT)-1)));
			} else {
				/* Else, align sprite to platform level. */
				alignSpriteToPlatform(Player.sprite);

				/* Yes. Continue wailing until the whole ladder is messed up. */
				if (!(GameScreenAnimationPhase & PLAYER_WAIL_PHASE)) {
					if (wailOnLadderAtSprite(Player.sprite)) {
						/* Switch to clean direction as soon the wailing is done. */
						changePlayerDirection(PLAYER_FLAGS_DIRECTION_CLEAN);
					} else {
						/* Trigger wail sound. */
						TriggerFx(PATCH_WAIL_LADDER,127,true);
					}
				}
			}

			break;
		case PLAYER_FLAGS_DIRECTION_UP:
			/* Change direction from ladder up to clean if player is at a squirted ladder. */
			/* Else, move sprite if nothing should stop us. */
			if (checkSpriteAtSquirtedLadderEntryUp(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_CLEAN);
			else if ((buttons & (BTN_RIGHT|BTN_LEFT|BTN_UP)) && (!checkSpriteAtLadderTop(Player.sprite)))
				moveSprite(Player.sprite,0,-(1<<min(0,((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)-1)));

			break;
		case PLAYER_FLAGS_DIRECTION_CLEAN:
			/* Clean ladder if nothing should stop us. */
			if ((buttons & (BTN_RIGHT|BTN_LEFT|BTN_UP))
				&& (!checkSpriteAtLadderTop(Player.sprite))
				&& !(GameScreenAnimationPhase & PLAYER_CLEAN_PHASE)) {
				/* Clean ladder tile, or change to up direction if all done. */
				if (checkSpriteAtSquirtedLadderEntryUp(Player.sprite)) {
					/* Clean ladder. */
					cleanLadderAtSprite(Player.sprite);

					/* Trigger clean sound. */
					TriggerFx(PATCH_CLEAN_LADDER,63,true);
				} else {
					changePlayerDirection(PLAYER_FLAGS_DIRECTION_UP);
				}

				/* Move player slowly. */
				moveSprite(Player.sprite,0,-1);
			}

			break;
	}
}


/* Animate caught player. */
uint8_t animateCaughtPlayer(void) {
	int16_t y;

	/* Move to invalid coordinate? */
	y=getSpriteY(Player.sprite);
	y+=Player.hit_speed;
	if (y>(SCREEN_HEIGHT*8)) {
		/* Yes. Remove player from screen. */
		unmapSprite(Player.sprite);

		/* Animation completed. */
		return 1;
	} else {
		/* No. Move with hit speed. */
		moveSprite(Player.sprite,0,Player.hit_speed);

		/* Turn hit speed from negative to more positive for next step. */
		Player.hit_speed++;

		/* Animation not completed. */
		return 0;
	}
}


/* Check if the player is hit by a burger component. */
uint8_t checkIfPlayerIsHit(void) {
	return (checkFallingBurgerComponentPosition(getSpriteX(Player.sprite)-SPRITE_BORDER_WIDTH,getSpriteY(Player.sprite)));
}

