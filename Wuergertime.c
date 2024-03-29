/*
 *  Würgertime - An hommage to Burger Time
 *
 *  Copyright (C) 2012 by Jan Kandziora <jjj@gmx.de>
 *  licensed under the GNU GPL v3 or later
 *
 *  see https://github.com/ianka/wuergertime
 *
 */

#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <math.h>
#include <stdio.h>


/* Sound effects */
#include "data/patches.inc"


/* Local headers */
#include "utils.h"
#include "controllers.h"
#include "tiles.h"
#include "sprites.h"
#include "draw.h"
#include "highscores.h"
#include "screens.h"
#include "screens_outofgame.h"
#include "screens_ingame.h"
#include "patches.h"


/*
 *  Main function
 */
int main(void) {
	/* Game-wide initialisation */
	resetControllers();

	/* Setup game-wide global variables. */
	Tileset=TILESET1;
	GameScreenPrevious=GAME_SCREEN_INVALID;
	GameScreen=GAME_SCREEN_START;
	GameScreenAnimationPhase=0;
	GameScreenUpdateFunction=NULL; /* This will be set correctly below. */

	/* Setup audio. */
	InitMusicPlayer(patches);

	/* Setup video. */
	SetSpritesTileTable(SpriteTiles);

	/* Main loop */
	for (;;) {
		/* Check for screen change. */
		if (GameScreen == GameScreenPrevious) {
			/* Sample controllers. */
			sampleControllers();

			/* No change. Call update function by pointer for current screen. */
			GameScreenUpdateFunction();

			/* Next animation phase. */
			GameScreenAnimationPhase++;

			/* Wait for next frame (let the interrupt kernel work). */
			WaitVsync(1);
		} else {
			/* Changed. Cleanup previous screen. */
			switch (GameScreenPrevious) {
				case GAME_SCREEN_START:             cleanupStartScreen(); break;
				case GAME_SCREEN_HIGHSCORES:        cleanupHighscoresScreen(); break;
				case GAME_SCREEN_GAME_OVER:         cleanupGameOverScreen(); break;
				case GAME_SCREEN_NEW_HIGHSCORE:     cleanupNewHighscoreScreen(); break;
				case GAME_SCREEN_ENTER_HIGHSCORE:   cleanupEnterHighscoreScreen(); break;
				case GAME_SCREEN_ENTERED_HIGHSCORE: cleanupEnteredHighscoreScreen(); break;
				case GAME_SCREEN_LEVEL_DESCRIPTION: cleanupInGameDescriptionScreen(); break;
				case GAME_SCREEN_LEVEL_PREPARE:     cleanupInGamePrepareScreen(); break;
				case GAME_SCREEN_LEVEL_START:       cleanupInGameStartScreen(); break;
				case GAME_SCREEN_LEVEL_PLAY:        cleanupInGamePlayScreen(); break;
				case GAME_SCREEN_LEVEL_LOSE:        cleanupInGameLoseScreen(); break;
				case GAME_SCREEN_LEVEL_GAME_OVER:   cleanupInGameOverScreen(); break;
				case GAME_SCREEN_LEVEL_HURRY:       cleanupInGameHurryScreen(); break;
				case GAME_SCREEN_LEVEL_BONUS:       cleanupInGameBonusScreen(); break;
			}

			/* Initialize new screen. */
			/* Set tileset */
			SetTileset((GameScreen & GAME_SCREEN_TILESET1)?TILESET1:TILESET0);

			/* Reset animation phase. */
			GameScreenAnimationPhase=0;

			/* Call screen dependent init function. */
			switch (GameScreen) {
				case GAME_SCREEN_START:
					GameScreenUpdateFunction=&updateStartScreen;
					initStartScreen();
					break;
				case GAME_SCREEN_HIGHSCORES:
					GameScreenUpdateFunction=&updateHighscoresScreen;
					initHighscoresScreen();
					break;
				case GAME_SCREEN_GAME_OVER:
					GameScreenUpdateFunction=&updateGameOverScreen;
					initGameOverScreen();
					break;
				case GAME_SCREEN_NEW_HIGHSCORE:
					GameScreenUpdateFunction=&updateNewHighscoreScreen;
					initNewHighscoreScreen();
					break;
				case GAME_SCREEN_ENTER_HIGHSCORE:
					GameScreenUpdateFunction=&updateEnterHighscoreScreen;
					initEnterHighscoreScreen();
					break;
				case GAME_SCREEN_ENTERED_HIGHSCORE:
					GameScreenUpdateFunction=&updateEnteredHighscoreScreen;
					initEnteredHighscoreScreen();
					break;
				case GAME_SCREEN_LEVEL_DESCRIPTION:
					GameScreenUpdateFunction=&updateInGameDescriptionScreen;
					initInGameDescriptionScreen();
					break;
				case GAME_SCREEN_LEVEL_PREPARE:
					GameScreenUpdateFunction=&updateInGamePrepareScreen;
					initInGamePrepareScreen();
					break;
				case GAME_SCREEN_LEVEL_START:
					GameScreenUpdateFunction=&updateInGameStartScreen;
					initInGameStartScreen();
					break;
				case GAME_SCREEN_LEVEL_PLAY:
					GameScreenUpdateFunction=&updateInGamePlayScreen;
					initInGamePlayScreen();
					break;
				case GAME_SCREEN_LEVEL_LOSE:
					GameScreenUpdateFunction=&updateInGameLoseScreen;
					initInGameLoseScreen();
					break;
				case GAME_SCREEN_LEVEL_GAME_OVER:
					GameScreenUpdateFunction=&updateInGameOverScreen;
					initInGameOverScreen();
					break;
				case GAME_SCREEN_LEVEL_HURRY:
					GameScreenUpdateFunction=&updateInGameHurryScreen;
					initInGameHurryScreen();
					break;
				case GAME_SCREEN_LEVEL_BONUS:
					GameScreenUpdateFunction=&updateInGameBonusScreen;
					initInGameBonusScreen();
					break;
			}

			/* Screen change done. */
			GameScreenPrevious=GameScreen;
		}
	}

	/* Never reached. */
	return 0;
}
