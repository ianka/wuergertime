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
 *  pepper.c - pepper functions.
 */


#include <avr/io.h> /* for uint8_t */


/* Local includes. */
#include "utils.h"
#include "pepper.h"
#include "player.h"
#include "sprites.h"
#include "draw.h"
#include "patches.h"


/* Pepper data. */
pepper_t Pepper;


/* Throw pepper from player position if no pepper is on its way already. */
uint8_t throwPepper(void) {
	/* Fail if a pepper is already on its way. */
	if (Pepper.flags)
		return 0;

	/* Fail if the player isn't running on a platform. */
	switch (Player.flags & PLAYER_FLAGS_DIRECTION_MASK) {
		case PLAYER_FLAGS_DIRECTION_LEFT:
		case PLAYER_FLAGS_DIRECTION_RIGHT:
			break;
		default:
			return 0;
	}

	/* Setup pepper flags. */
	Pepper.flags=(Player.flags & PEPPER_FLAGS_DIRECTION_MASK) | PEPPER_FLAGS_SPEED_FLASH;

	/* Setup pepper sprite. */
	placeSprite(Pepper.sprite,
		getSpriteX(Player.sprite),
		getSpriteY(Player.sprite),
		SPRITE_FLAGS_TYPE_PEPPER|SPRITE_FLAGS_DIRECTION_RIGHT);

	/* Trigger pepper sound. */
	TriggerFx(PATCH_THROW_PEPPER,192,true);

	/* Sucess. */
	return 1;
}


/* Move pepper, take from screen if off-screen. */
void movePepper(void) {
	int8_t dir;

	/* Fly into current direction, or skip. */
	switch (Pepper.flags & PEPPER_FLAGS_DIRECTION_MASK) {
		case PEPPER_FLAGS_DIRECTION_LEFT:
			dir=-1;
			break;
		case PEPPER_FLAGS_DIRECTION_RIGHT:
			dir=1;
			break;
		default:
			return;
	}

	/* Move pepper. Remove it from screen at borders. */
	if (!moveSpriteIfNotBorder(Pepper.sprite,dir*(1<<((Pepper.flags & PEPPER_FLAGS_SPEED_MASK)>>PEPPER_FLAGS_SPEED_SHIFT)),0))
		removePepper();
}


/* Remove pepper from screen. */
void removePepper(void) {
	Pepper.flags=0;
	unmapSprite(Pepper.sprite);
}
