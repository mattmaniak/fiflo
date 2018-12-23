#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// charmap.h
extern F_mtdt* key_action(F_mtdt* Buff, char key);

// seqmap.h
extern F_mtdt* recognize_sequence(F_mtdt* Buff, char sequence[8]);

// Reads one char wthout confirming by the key. Based on the termios.
char getch(F_mtdt* Buff);

// Saves the last pressed key to the temponary buffer and analyzes it.
F_mtdt* parse_key(F_mtdt* Buff, char key);

static const struct
{
	char    (*getch)(F_mtdt*);
	F_mtdt* (*parse_key)(F_mtdt*, char);
}
keyboard =
{
	getch,
	parse_key
};

#endif
