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

// Read an one char wthout clicking the enter key. Termios based.
char input__getch(void);

// Converts a given letter by the parse_key and chooses the cursor direction.
void input__recognize_sequence(Buff_t*, const Conf_t* const, const char* const,
                               size_t*);

// Saves a last pressed key to a temponary buffer and analyzes it.
bool input__parse_key(Buff_t*, const Conf_t* const, Mod_t*, size_t*,
                      const char);

#endif
