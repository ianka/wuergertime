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


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"
#include "player.h"
#include "sprites.h"


/* Player data. */
player_t Player;


/* Reset player to start position. */
void resetPlayer(void) {
	/* Setup flags. */
	Player.flags=PLAYER_FLAGS_DIRECTION_RIGHT|PLAYER_FLAGS_SPEED_NORMAL;

	/* Setup player sprite. */
	placeSprite(Player.sprite,
		Player.start_position.x*8,
		Player.start_position.y*8,
		SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT);
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
		default:	
			changeSpriteDirection(Player.sprite,SPRITE_FLAGS_DIRECTION_LADDER);
	}
}


/* Select a player direction from held buttons. */
void selectPlayerDirection(uint8_t buttons) {
	switch (buttons) {
		case BTN_LEFT:
			/* Skip if we already are on our way left. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_LEFT)
				break;

			/* Change direction on floor if player direction is currently right. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_RIGHT)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);

			/* Change direction from ladder to floor if player is at a ladder exit. */
			if (checkSpriteAtLadderExit(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_LEFT);

			break;	
		case BTN_RIGHT:
			/* Skip if we already are on our way right. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_RIGHT)
				break;

			/* Change direction on floor if player direction is currently left. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_LEFT)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			/* Change direction from ladder to floor if player is at a ladder exit. */
			if (checkSpriteAtLadderExit(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_RIGHT);

			break;
		case BTN_DOWN:
			/* Skip if we already are on our way down. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_DOWN)
				break;

			/* Change direction on ladder if player direction is currently up. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_UP)
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);

			/* Change direction from floor to ladder if player is onto a ladder top. */
			if (checkSpriteAtLadderEntryDown(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_DOWN);

			break;
		case BTN_UP:
			/* Skip if we already are on our way up. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_UP)
				break;
			
			/* Change direction on ladder if player direction is currently down. */
			if ((Player.flags & PLAYER_FLAGS_DIRECTION_MASK) == PLAYER_FLAGS_DIRECTION_DOWN)
				changePlayerDirectionWithoutAnimationReset(PLAYER_FLAGS_DIRECTION_UP);

			/* Change direction from floor to ladder if player is at a ladder. */
			if (checkSpriteAtLadderEntryUp(Player.sprite))
				changePlayerDirection(PLAYER_FLAGS_DIRECTION_UP);

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
			if ((buttons & (BTN_RIGHT|BTN_UP|BTN_DOWN)) && (!checkSpriteAtRightFloorEnd(Player.sprite))) {
				/* Move! */
				moveSprite(Player.sprite,(1<<((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)),0);

				/* Stomp tile under player sprite. */
				stompUnderSprite(Player.sprite);
			}

			break;
		case PLAYER_FLAGS_DIRECTION_DOWN:
			if ((buttons & (BTN_RIGHT|BTN_LEFT|BTN_DOWN)) && (!checkSpriteAtLadderBottom(Player.sprite)))
				/* Move! */
				moveSprite(Player.sprite,0,(1<<min(0,((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)-1)));

			break;
		case PLAYER_FLAGS_DIRECTION_UP:
			if ((buttons & (BTN_RIGHT|BTN_LEFT|BTN_UP)) && (!checkSpriteAtLadderTop(Player.sprite)))
				/* Move! */
				moveSprite(Player.sprite,0,-(1<<min(0,((Player.flags & PLAYER_FLAGS_SPEED_MASK)>>PLAYER_FLAGS_SPEED_SHIFT)-1)));
			break;
	}
}
