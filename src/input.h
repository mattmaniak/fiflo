#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Reads one char wthout confirming by enter the key. Termios based.
	char (*getch)(void);

	// Converts the given letter by the parse_key and chooses the cursror direction.
	void (*recognize_sequence)(Buff_t*, char*);

	// Saves the last pressed key to the temponary buffer and analyzes it.
	bool (*parse_key)(Buff_t*, char);
}
namespace_input;

extern namespace_input input;

#endif
