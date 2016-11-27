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
 *  highscores.h - highscore functions
 */


#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <avr/io.h> /* for uint8_t et al. */


/* Highscore entries. */
#define HIGHSCORE_ENTRY_MAX 5


void readHighscoreEntry(uint8_t index, uint32_t *name, uint32_t *score);
uint8_t findToppedHighscoreEntry(uint32_t score);
char replaceHighscoreEntry(uint8_t index, uint32_t name, uint32_t score);


#endif /* HIGHSCORES_H */
