#ifndef INPUT_H
#define INPUT_H

#include <termios.h>

#include "v_file.h"
#include "config.h"
#include "keys.h"
#include "edit.h"
#include "file_io.h"
#include "arrows.h"
#include "modes.h"
#include "window.h"

#define INPUT__SEQ_MAX 8

// Read an one char wthout clicking the enter key. Termios based.
char input__getch(void);

// Converts a given letter by the parse_key and chooses the cursor direction.
void input__recognize_sequence(V_file_t* const, const Conf_t* const,
                               const char* const, size_t* const);

// Saves a last pressed key to a temponary buffer and analyzes it.
bool input__parse_key(V_file_t* const, const Conf_t* const, Mod_t* const,
                      size_t* const, const char);

#endif
