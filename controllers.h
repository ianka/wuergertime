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
 *  controllers.h - get controller input
 */


#ifndef CONTROLLERS_H
#define CONTROLLERS_H


#include <avr/io.h> /* for uint16_t */


/* Button aggreate masks. */
#define BTN_DIRECTIONS ((BTN_UP|BTN_DOWN|BTN_LEFT|BTN_RIGHT))
#define BTN_NONDIRECTION ((BTN_A|BTN_B|BTN_X|BTN_Y|BTN_SR|BTN_SL|BTN_START|BTN_SELECT))
#define BTN_INGAME ((BTN_UP|BTN_DOWN|BTN_LEFT|BTN_RIGHT|BTN_A|BTN_B|BTN_X|BTN_Y|BTN_SR|BTN_SL))
#define BTN_OUTOFGAME ((BTN_START|BTN_SELECT))
#define BTN_ALL ((BTN_UP|BTN_DOWN|BTN_LEFT|BTN_RIGHT|BTN_A|BTN_B|BTN_X|BTN_Y|BTN_SR|BTN_SL|BTN_START|BTN_SELECT))


void initControllers(void);
void sampleControllers(void);
uint16_t checkControllerButtonsHeld(uint16_t n, uint16_t mask);
uint16_t checkControllerButtonsPressed(uint16_t n, uint16_t mask);
uint16_t checkControllerButtonsReleased(uint16_t n, uint16_t mask);


#endif /* CONTROLLERS_H */
