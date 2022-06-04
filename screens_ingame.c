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
 *  screens_ingame.c - in game screen init, update, cleanup functions.
 */


#include <uzebox.h>


/* Local includes. */
#include "utils.h"
#include "screens.h"
#include "draw.h"
#include "tiles.h"
#include "sprites.h"
#include "controllers.h"
#include "player.h"
#include "pepper.h"
#include "opponents.h"
#include "patches.h"


/* Fixed strings. */
const char TextHeatUp[] PROGMEM = "HEAT IS ON!";


/*
 *  The description screen is the first screen for each level to be showed.
 *  It places a signpost with some description what this level will be like.
 *  Tileset 1 is selected as we need an alphabet.
 */
void initInGameDescriptionScreen(void) {
	/* Draw level description picture. */
	clearScreen();
	drawFloor(0,20,SCREEN_WIDTH,DRAW_OPTION_FLOOR_FORCE_WRAP);
	prepareLevelDescription(20);

	/* Fade in.*/
	FadeIn(1,0);

	/* Setup cook start position and direction. */
	Player.flags=PLAYER_FLAGS_DIRECTION_RIGHT|PLAYER_FLAGS_SPEED_NORMAL;
	placeSprite(Player.sprite,
		DESCRIPTION_COOK_START_POSITION_X*8+SPRITE_BORDER_WIDTH,
		DESCRIPTION_COOK_START_POSITION_Y*8,
		SPRITE_FLAGS_TYPE_COOK|SPRITE_FLAGS_DIRECTION_RIGHT);
}

void updateInGameDescriptionScreen(void) {
	/* Animate "Ready for Heat Up". */
	if (GameScreenAnimationPhase & 8) {
		drawStringCentered(8,TextHeatUp);
	} else {
		clearLine(8);
	}

	/* Trigger heat up sound. */
	if (!(GameScreenAnimationPhase & 63))
		TriggerFx(PATCH_HEAT_UP,127,true);

	/* Move cook until end position reached. */
	if (getSpriteTileX(Player.sprite,0) < DESCRIPTION_COOK_END_POSITION_X)
		moveSprite(Player.sprite,1,0);
	else
		ChangeGameScreen(GAME_SCREEN_LEVEL_PREPARE);
}

void cleanupInGameDescriptionScreen(void) {
	/* Fade out. */
	FadeOut(1,1);

	/* Unmap the cook.*/
	unmapSprite(Player.sprite);
}


/*
 *  The prepare screen is showed when a new level is reached.
 *    + Score displays
 *    + Opening animation of floors and burgers
 *    + Burger and opponent positions are being reset
 */
void initInGamePrepareScreen(void) {
	/* Fade into clear screen */
	clearScreen();
	FadeIn(1,0);

	/* Prepare level by description. */
	prepareLevel();
}

void updateInGamePrepareScreen(void) {
	/* Level build animation. */
	animateLevelStart();

	/* Trigger fall sound. */
	if (!(GameScreenAnimationPhase & 15))
		TriggerFx(PATCH_BURGER_FALL,63,true);

	/* Change screen when animation is done. */
	if (GameScreenAnimationPhase > LEVEL_START_ANIMATION_ENDED+1)
		ChangeGameScreen(GAME_SCREEN_LEVEL_START);
}

void cleanupInGamePrepareScreen(void) {
	/* Reset sprites. */
	resetSpriteSlots();
	Player.sprite=occupySpriteSlot();
	Pepper.sprite=occupySpriteSlot();
	Opponent[0].sprite=occupySpriteSlot();
	Opponent[1].sprite=occupySpriteSlot();
	Opponent[2].sprite=occupySpriteSlot();
	Opponent[3].sprite=occupySpriteSlot();

	/* Reset opponent positions. */
	resetOpponents();
}


/*
 *  The start screen is showed after prepare screen or after a life is lost.
 *     + Cook reset
 *     + Bonus reset
 */
void initInGameStartScreen(void) {
	/* Reset controllers. */
	resetControllers();

	/* Reset bonus. */
	Bonus=DEFAULT_BONUS;

	/* Reset player to start position. */
	resetPlayer();
}

void updateInGameStartScreen(void) {
	/* Burger drop animation. */
	/* Change screen when animation is done. */
	dropHattedComponents();
	if (!animateBurgers())
		ChangeGameScreen(GAME_SCREEN_LEVEL_PLAY);
}

void cleanupInGameStartScreen(void) {
}


/*
 *  The play screen is showed when the actual game is happening.
 */
void initInGamePlayScreen(void) {
	/* Trigger respawn sound. */
	TriggerFx(PATCH_RESPAWN_COOK,255,true);
}


void updateInGamePlayScreen(void) {
	uint8_t directional_buttons_held;
	uint8_t i;

	/* Animate open sign. */
	if (GameScreenAnimationPhase & 64) {
		SetTile(15,0,TILES0_OPEN_OFF_LEFT);
		SetTile(16,0,TILES0_OPEN_OFF_RIGHT);
	} else {
		SetTile(15,0,TILES0_OPEN_ON_LEFT);
		SetTile(16,0,TILES0_OPEN_ON_RIGHT);
	}

	/* Player start blink animation. */
	if (GameScreenAnimationPhase < PLAYER_START_BLINKING_ENDED) {
		if (blink((GameScreenAnimationPhase)>>PLAYER_START_BLINKING_SHIFT,PLAYER_START_BLINKCODE)) {
			/* Normal player. */
			setSpriteTransparency(Player.sprite,0);
			updateSprite(Player.sprite);
		}	else {
			/* Transparent player. */
			setSpriteTransparency(Player.sprite,SPRITE_FLAGS_TRANSPARENT);
			updateSprite(Player.sprite);
		}
	}

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Check if player and opponents should move. */
	if ((Bonus < HURRY_BONUS) || (GameScreenAnimationPhase & 1)) {
		/* Yes. Get held direction buttons. */
		directional_buttons_held=checkControllerButtonsHeld(0,BTN_DIRECTIONS);

		/* Select direction to move player. */
		selectPlayerDirection(directional_buttons_held);

		/* Move player into selected direction, if possible. */
		movePlayer(directional_buttons_held);

		/* Try to throw pepper if fire button is pressed and the cook has peppers. */
		if (checkControllerButtonsPressed(0,BTN_NONDIRECTION) && Peppers>0) {
			/* Decrement the amount of available peppers if throwing was possible. */
			if (throwPepper())
				Peppers--;
		}

		/* Move pepper if present. */
		movePepper();

		/* Handle all opponents. */
		for (i=0;i<OPPONENT_MAX;i++) {
			/* Kick opponent if it is hit by a burger component. */
			kickOpponentIfHit(i);

			/* Kick opponent if it is hit by pepper. */
			kickOpponentIfPeppered(i);

			/* Select direction and move all active opponents. */
			selectOpponentDirection(i);
			moveOpponent(i);

			/* Change to lose screen when an opponent caught a player. */
			if ((GameScreenAnimationPhase >= PLAYER_START_BLINKING_ENDED)
				&& (checkIfOpponentCaughtPlayer(i) || checkIfPlayerIsHit()))
				ChangeGameScreen(GAME_SCREEN_LEVEL_LOSE);

		}
	}

	/* Start next attack wave on matching animation phase. */
	nextAttackWave();

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Decrement bonus, lose a life when bonus is zero. */
	if (decrementBonus())
		ChangeGameScreen(GAME_SCREEN_LEVEL_LOSE);

	/* Check if we are at hurry moment. */
	if (Bonus == HURRY_BONUS)
		ChangeGameScreen(GAME_SCREEN_LEVEL_HURRY);

	/* Award served burgers with score. Switch to bonus screen if all burgers are served. */
	if (awardServedBurgers())
		ChangeGameScreen(GAME_SCREEN_LEVEL_BONUS);
}

void cleanupInGamePlayScreen(void) {
}


/*
 *  The hurry screen is showed when bonus counter reaches zero before all burgers are done.
 *  It shows the hurry animation on the level screen.
 */
void initInGameHurryScreen(void) {
}

void updateInGameHurryScreen(void) {
	/* Animate "Hurry!" sign, leave screen when animation is done. */
	if (animateHurry())
		ChangeGameScreen(GAME_SCREEN_LEVEL_PLAY);
}

void cleanupInGameHurryScreen(void) {
}


/*
 *  The bonus screen is showed when all burgers have been served.
 *  It shows the bonus animation on the level screen.
 */
void initInGameBonusScreen(void) {
	uint8_t i, p, start_positions=0;

	/* Fast bonus counting. */
	GameScreenOptions&=~LEVEL_ITEM_OPTION_BONUS_MASK;

	/* Go through all start positions. */
	for (i=0;i<OPPONENT_START_POSITION_MAX;i++) {
		/* Skip invalid start positions. */
		if (OpponentStartPosition[i].x == OPPONENT_START_POSITION_INVALID)
			continue;

		/* Draw a roach on each valid startpoint. */
		drawBonusItem(OpponentStartPosition[i].x-1,OpponentStartPosition[i].y-2,SHAPE_BONUS_ROACH);

		/* Remember the number of valid start positions. */
		start_positions++;
	}

	/* Handle all opponents. */
	for (i=0;i<OPPONENT_MAX;i++) {
		/* Skip invalid and hit opponents. */
		if (checkInvalidOrHitOpponent(i))
			continue;

		/* Calculate start position to use. */
		p=(i*11)%start_positions;

		/* Draw bonus item depending on opponent over roach. */
		drawBonusItem(OpponentStartPosition[p].x-1,OpponentStartPosition[p].y-2,
			((Opponent[i].flags & OPPONENT_FLAGS_ALGORITHM_MASK)>>OPPONENT_FLAGS_ALGORITHM_SHIFT)+1);

		/* Kick opponent off-screen. */
		kickOpponent(i);
	}
}

void updateInGameBonusScreen(void) {
	uint8_t i, directional_buttons_held;
	uint8_t cleared=1;

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Get held buttons. */
	directional_buttons_held=checkControllerButtonsHeld(0,BTN_DIRECTIONS);

	/* Select direction to move player. */
	selectPlayerDirection(directional_buttons_held);

	/* Move player into selected direction, if possible. */
	movePlayer(directional_buttons_held);

	/* Move pepper if present. */
	movePepper();

	/* Move all active opponents. */
	for (i=0;i<OPPONENT_MAX;i++)
		moveOpponent(i);

	/* Check if bonus stage cleared. */
	for (i=0;i<OPPONENT_START_POSITION_MAX;i++) {
		if (OpponentStartPosition[i].x != OPPONENT_START_POSITION_INVALID) {
			/* Not cleared yet. */
			cleared=0;

			/* Check if bonus item reached. */
			if (((getSpriteTileX(Player.sprite,0) == OpponentStartPosition[i].x-1)
				|| (getSpriteTileX(Player.sprite,0) == OpponentStartPosition[i].x))
				&& (getSpriteTileY(Player.sprite,0) == OpponentStartPosition[i].y)) {
				/* Reached. Get item type.*/
				switch (getTile(OpponentStartPosition[i].x-1,OpponentStartPosition[i].y-2)) {
					case TILES0_ROACH_UPPER_LEFT:
						/* Score. */
						Score+=SCORE_BONUS_ITEM_ROACH;

						/* Trigger collect roach sound. */
						TriggerFx(PATCH_COLLECT_ROACH,192,true);
						break;
					case TILES0_SODA_UPPER_LEFT:
						/* Score. */
						Score+=SCORE_BONUS_ITEM_SODA;

						/* Trigger collect soda sound. */
						TriggerFx(PATCH_COLLECT_SODA,192,true);
						break;
					case TILES0_FRIES_UPPER_LEFT:
						/* Score. */
						Score+=SCORE_BONUS_ITEM_FRIES;

						/* Trigger collect fries sound. */
						TriggerFx(PATCH_COLLECT_FRIES,192,true);
						break;
					case TILES0_PEPPER_UPPER_LEFT:
						/* Score. */
						Score+=SCORE_BONUS_ITEM_PEPPER;

						/* Increment number of peppers.*/
						Peppers++;

						/* Trigger collect pepper sound. */
						TriggerFx(PATCH_COLLECT_PEPPER,255,true);
						break;
					case TILES0_CROWN_UPPER_LEFT:
						/* Score. */
						Score+=SCORE_BONUS_ITEM_CROWN;

						/* Increment number of lives. */
						Lives++;

						/* Trigger collect crown sound. */
						TriggerFx(PATCH_COLLECT_CROWN,255,true);
						break;
				}

				/* Clear item. */
				clearQuad(OpponentStartPosition[i].x-1,OpponentStartPosition[i].y-2);
				OpponentStartPosition[i].x=OPPONENT_START_POSITION_INVALID;
			}
		}
	}

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Check if all bonus items have been collected. */
	if (cleared) {
		/* Yes. Decrement bonus fast. */
		if (decrementBonusFast()) {
			/* Next level as soon the bonus is zero and the displayed score is updated. */
			if (DisplayedScore == Score)
				ChangeGameScreen(GAME_SCREEN_LEVEL_DESCRIPTION);
		} else {
			/* Not zero. Award score for each fast bonus tick. */
			Score+=BONUS_FAST_DECREMENT;
		}
	} else {
		/* No. Decrement bonus. */
		if (decrementBonus()) {
			/* Next level as soon the bonus is zero and the displayed score is updated. */
			if (DisplayedScore == Score)
				ChangeGameScreen(GAME_SCREEN_LEVEL_DESCRIPTION);
		}
	}
}

void cleanupInGameBonusScreen(void) {
	/* Fade out. */
	FadeOut(1,1);

	/* Advance to next level. */
	selectLevel(Level+1);
	pushentropy(GameScreenAnimationPhase);
}


/*
 *  The lose screen is showed when a life is lost.
 *  It shows the lose animation on the level screen.
 */
void initInGameLoseScreen(void) {
	/* Unwail the ladder the player is at, if any. */
	unwailLadderAtSprite(Player.sprite);

	/* Kick player from screen. */
	changePlayerDirection(PLAYER_FLAGS_DIRECTION_CAUGHT);

	/* Trigger hit sound. */
	TriggerFx(PATCH_HIT_COOK,255,true);
}

void updateInGameLoseScreen(void) {
	uint8_t i;

	/* Burger drop animation. */
	dropHattedComponents();
	animateBurgers();

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Move pepper if present. */
	movePepper();

	/* Handle all opponents. */
	for (i=0;i<OPPONENT_MAX;i++) {
		/* Kick opponent if it is hit by a burger component. */
		kickOpponentIfHit(i);

		/* Kick opponent if it is hit by pepper. */
		kickOpponentIfPeppered(i);

		/* Select direction and move all active opponents. */
		selectOpponentDirection(i);
		moveOpponent(i);
	}

	/* Animate caught player. */
	if (animateCaughtPlayer()) {
		/* Animations done. Notice lost life. */
		Lives--;

		/* Add new peppers. */
		Peppers+=DEFAULT_PEPPERS;

		/* Re-initialize level or game over. */
		if (Lives)
				ChangeGameScreen(GAME_SCREEN_LEVEL_START);
			else
				ChangeGameScreen(GAME_SCREEN_LEVEL_GAME_OVER);
	}
}

void cleanupInGameLoseScreen(void) {
}


/*
 *  The in game game over screen is showed when all lives are lost.
 *  It shows an animation on the level screen.
 */
void initInGameOverScreen(void) {
	/* Let all burgers drop off-screen. */
	dropAllBurgersOffScreen();

	/* Trigger game over sound. */
	TriggerFx(PATCH_GAME_OVER,255,true);
}

void updateInGameOverScreen(void) {
	uint8_t o, b, i;

	/* Move pepper if present. */
	movePepper();

	/* Handle all opponents. */
	for (i=0;i<OPPONENT_MAX;i++) {
		/* Select direction and move all active opponents. */
		selectOpponentDirection(i);
		moveOpponent(i);
	}

	/* Burger drop animation. */
	b=animateBurgers();

	/* Decrement Bonus */
	o=decrementBonusFast();

	/* Update game screen statistics. */
	updateGameScreenStatistics();

	/* Change screen when burger drop animation is done, no bonus left and displayed score updated. */
	if (o && b == 0 && DisplayedScore == Score)
		ChangeGameScreen(GAME_SCREEN_GAME_OVER);
}

void cleanupInGameOverScreen(void) {
	/* Unmap all opponents. */
	unmapOpponents();

	/* Fade out. */
	FadeOut(1,1);
}



