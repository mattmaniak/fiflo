#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Reads one char wthout confirming by enter the key. Termios based.
	char (*getch)(Buff_t*);

	// Saves the last pressed key to the temponary buffer and analyzes it.
	Buff_t* (*parse_key)(Buff_t*, char);
}
namespace_keyboard;

extern namespace_keyboard keyboard;

#endif
