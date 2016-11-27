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
 *  highscores.c - highscore functions
 */


#include <avr/io.h> /* for uint16_t */
#include <uzebox.h> /* for EepromReadBlock() and friends */


/* Local includes. */
#include "highscores.h"
#include "utils.h"


/* Highscore entries. */
#define HIGHSCORE_ENTRY_SIZE 6

typedef struct {
	uint16_t eid;
	uint8_t  entries[HIGHSCORE_ENTRY_MAX][HIGHSCORE_ENTRY_SIZE];
} highscores_t;


/* Wuergertime EEPROM block id. */
#define WUERGERTIME_EID 0x5754


/* Default Highscore entries. */
const uint8_t DefaultHighscores[] PROGMEM={
	0xa0, 0x86, 0x81, 0x5b, 0x8b, 0x3c, /* WUERG 100000 */
	0x50, 0xc3, 0x80, 0x5c, 0xc7, 0x02, /* YUCK   50000 */
	0x20, 0x4e, 0x80, 0x74, 0x12, 0xa5, /* IGITT  20000 */
	0x10, 0x27, 0x00, 0xc1, 0x82, 0x3c, /* BLARG  10000 */
	0x88, 0x13, 0x00, 0x11, 0x0a, 0x02, /* BAEH    5000 */
};


/* Decode name from highscore entry. */
uint32_t decodeHighscoreEntryName(uint8_t entry[HIGHSCORE_ENTRY_SIZE]) {
	uint32_t name;

	name=entry[5];
	name<<=8;
	name|=entry[4];
	name<<=8;
	name|=entry[3];
	name<<=1;
	name|=(entry[2]>>7);

	return name;
}


/* Decode score from highscore entry. */
uint32_t decodeHighscoreEntryScore(uint8_t entry[HIGHSCORE_ENTRY_SIZE]) {
	uint32_t score;

	score=(entry[2] & 0x7f);
	score<<=8;
	score|=entry[1];
	score<<=8;
	score|=entry[0];

	return score;
}


/* Read highscores from EEPROM/program memory. */
void readHighscores(highscores_t *highscores) {
	/* Read highscore block from EEPROM. */
	if (EepromReadBlock(WUERGERTIME_EID,(struct EepromBlockStruct *)highscores)) {
		/* Not found. Initialize highscores from program memory. */
		meminit(highscores->entries,&DefaultHighscores,sizeof(highscores->entries));
	}
}


/* Read single highscore entry. */
void readHighscoreEntry(uint8_t index, uint32_t *name, uint32_t *score) {
	highscores_t highscores;

	/* Read highscores. */
	readHighscores(&highscores);

	/* Decode and return the entry. */
	*name=decodeHighscoreEntryName((uint8_t*)&highscores.entries[index]);
	*score=decodeHighscoreEntryScore((uint8_t*)&highscores.entries[index]);
}


/* Find topped highscore entry. */
uint8_t findToppedHighscoreEntry(uint32_t score) {
	highscores_t highscores;
	uint8_t i;

	/* Read highscores. */
	readHighscores(&highscores);

	/* Go through all entries, lowest highscore first. */
	for(i=HIGHSCORE_ENTRY_MAX-1;i>0;i--) {
		if (score < decodeHighscoreEntryScore((uint8_t*)&highscores.entries[i]))
			return (i+1);
	}

	/* Topped the champion! */
	return 0;
}


/* Replace highscore entry. */
char replaceHighscoreEntry(uint8_t index, uint32_t name, uint32_t score) {
	highscores_t highscores;
	uint8_t i,j;

	/* Skip when index is invalid */
	if (index >= HIGHSCORE_ENTRY_MAX) return 0;

	/* Read highscores. */
	readHighscores(&highscores);

	/* Move existing entries. */
	for(i=HIGHSCORE_ENTRY_MAX-1;i>index;i--)
		for(j=0;j<HIGHSCORE_ENTRY_SIZE;j++)
			highscores.entries[i][j]=highscores.entries[i-1][j];

	/* Encode name and score into highscore entry. */
	highscores.entries[index][0]=score & 0xff;
	score>>=8;
	highscores.entries[index][1]=score & 0xff;
	score>>=8;
	highscores.entries[index][2]=score & 0x7f;

	highscores.entries[index][2]|=(name & 0x01)<<7;
	name>>=1;
	highscores.entries[index][3]=name & 0xff;
	name>>=8;
	highscores.entries[index][4]=name & 0xff;
	name>>=8;
	highscores.entries[index][5]=name & 0xff;

	/* Write highscores. */
	highscores.eid=WUERGERTIME_EID;
	return EepromWriteBlock((struct EepromBlockStruct *)&highscores);
}
