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
 *  screen_ingame.h - in game screen init, update, cleanup functions.
 */


#ifndef SCREENS_INGAME_H
#define SCREENS_INGAME_H


void initInGameDescriptionScreen(void);
void updateInGameDescriptionScreen(void);
void cleanupInGameDescriptionScreen(void);

void initInGamePrepareScreen(void);
void updateInGamePrepareScreen(void);
void cleanupInGamePrepareScreen(void);

void initInGameStartScreen(void);
void updateInGameStartScreen(void);
void cleanupInGameStartScreen(void);

void initInGamePlayScreen(void);
void updateInGamePlayScreen(void);
void cleanupInGamePlayScreen(void);

void initInGameHurryScreen(void);
void updateInGameHurryScreen(void);
void cleanupInGameHurryScreen(void);

void initInGameBonusScreen(void);
void updateInGameBonusScreen(void);
void cleanupInGameBonusScreen(void);

void initInGameLoseScreen(void);
void updateInGameLoseScreen(void);
void cleanupInGameLoseScreen(void);

void initInGameWinScreen(void);
void updateInGameWinScreen(void);
void cleanupInGameWinScreen(void);

void initInGameAftermathScreen(void);
void updateInGameAftermathScreen(void);
void cleanupInGameAftermathScreen(void);

#endif /* SCREENS_INGAME_H */
