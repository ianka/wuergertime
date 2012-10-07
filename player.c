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

