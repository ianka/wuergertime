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
 *  controllers.c - get controller input
 */


#include <avr/io.h> /* for uint16_t */
#include <uzebox.h> /* for ReadJoypad */


/* Controller data. */
struct { uint16_t current, previous, held, pressed, released; } controllers[2];


/* Reset controller struct. */
void resetControllers(void) {
	uint16_t i;

	for (i=0;i<2;i++) {
		controllers[i].previous = 0;
		controllers[i].current = 0;
		controllers[i].held = 0;
		controllers[i].pressed = 0;
		controllers[i].released = 0;
	}
}


/* Sample controllers. */
void sampleControllers(void) {
	uint16_t i;

	for (i=0;i<2;i++) {
		/* Read current value. */
		controllers[i].previous = controllers[i].current;
		controllers[i].current = ReadJoypad(i);

		/* Set event bits. */
		controllers[i].held |= controllers[i].current & controllers[i].previous;
		controllers[i].pressed |= controllers[i].current & (controllers[i].current ^ controllers[i].previous);
		controllers[i].released |= controllers[i].previous & (controllers[i].current ^ controllers[i].previous);
	}	
}


/* Calculate change since previous check. Update masked bits only. */
uint16_t checkControllerButtonsHeld(uint16_t n, uint16_t mask) {
	uint16_t result;

	/* Save masked result. */
	result=controllers[n].held & mask;

	/* Reset masked event bits, they are already checked. */
	controllers[n].held &= ~mask;

	/* Return. */
	return result;
}


uint16_t checkControllerButtonsPressed(uint16_t n, uint16_t mask) {
	uint16_t result;

	/* Save masked result. */
	result=controllers[n].pressed & mask;

	/* Reset masked event bits, they are already checked. */
	controllers[n].pressed &= ~mask;

	/* Return. */
	return result;
}


uint16_t checkControllerButtonsReleased(uint16_t n, uint16_t mask) {
	uint16_t result;

	/* Save masked result. */
	result=controllers[n].released & mask;

	/* Reset masked event bits, they are already checked. */
	controllers[n].released &= ~mask;

	/* Return. */
	return result;
}

