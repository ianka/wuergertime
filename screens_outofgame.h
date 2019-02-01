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
 *  screen_outofgame.h - out of game screen init, update, cleanup functions.
 */


#ifndef SCREENS_OUTOFGAME_H
#define SCREENS_OUTOFGAME_H


void initDebugScreen(void);
void updateDebugScreen(void);
void cleanupDebugScreen(void);

void initStartScreen(void);
void updateStartScreen(void);
void cleanupStartScreen(void);

void initCreditsScreen(void);
void updateCreditsScreen(void);
void cleanupCreditsScreen(void);

void initDemoScreen(void);
void updateDemoScreen(void);
void cleanupDemoScreen(void);

void initHighscoresScreen(void);
void updateHighscoresScreen(void);
void cleanupHighscoresScreen(void);

void initGameOverScreen(void);
void updateGameOverScreen(void);
void cleanupGameOverScreen(void);

void initNewHighscoreScreen(void);
void updateNewHighscoreScreen(void);
void cleanupNewHighscoreScreen(void);

void initEnterHighscoreScreen(void);
void updateEnterHighscoreScreen(void);
void cleanupEnterHighscoreScreen(void);

void initEnteredHighscoreScreen(void);
void updateEnteredHighscoreScreen(void);
void cleanupEnteredHighscoreScreen(void);


#endif /* SCREENS_OUTOFGAME_H */
