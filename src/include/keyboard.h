#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// charmap.h
extern Buff_t* key_action(Buff_t* Buff, char key);

// seqmap.h
extern Buff_t* recognize_sequence(Buff_t* Buff, char sequence[8]);

// Reads one char wthout confirming by the key. Based on the termios.
char getch(Buff_t* Buff);

// Saves the last pressed key to the temponary buffer and analyzes it.
Buff_t* parse_key(Buff_t* Buff, char key);

static const struct
{
	char    (*getch)(Buff_t*);
	Buff_t* (*parse_key)(Buff_t*, char);
}
keyboard =
{
	getch,
	parse_key
};

#endif
