#ifndef INPUT_H
#define INPUT_H

#include <termios.h>

#include "buffer.h"
#include "config.h"
#include "chars.h"
#include "edit.h"
#include "file.h"
#include "keys.h"
#include "modes.h"
#include "window.h"

#define INPUT__SEQ_MAX 8

// Reads one char wthout confirming by enter the key. Termios based.
char input__getch(void);

// Converts the given letter by the parse_key and chooses the cursror direction.
void input__recognize_sequence(Buff_t*, const Conf_t* const, const char* const,
                               size_t*);

// Saves the last pressed key to the temponary buffer and analyzes it.
bool input__parse_key(Buff_t*, const Conf_t* const, Mod_t*, size_t*,
                      const char);

#endif
