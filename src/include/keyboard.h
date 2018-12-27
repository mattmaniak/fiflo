#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// Reads one char wthout confirming by the key. Based on the termios.
// char getch(Buff_t* Buff);

// Saves the last pressed key to the temponary buffer and analyzes it.
// Buff_t* parse_key(Buff_t* Buff, char key);

typedef const struct
{
	char    (*getch)(Buff_t*);
	Buff_t* (*parse_key)(Buff_t*, char);
}
namespace_keyboard;

extern namespace_keyboard keyboard;

#endif
