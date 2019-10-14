#ifndef INPUT_H
#define INPUT_H

#include <termios.h>

#include "v_file.h"
#include "config.h"
#include "keys.h"
#include "edit.h"
#include "file_io.h"
#include "modes.h"
#include "window.h"

#define INPUT__SEQ_MAX 8

// Read an one char wthout clicking the enter key. Termios based.
char input__getch(void);

// Converts a given letter by the parse_key and chooses the cursor direction.
void input__recognize_sequence(V_file* const, const Config* const,
                               const char* const, size_t* const);

// Saves a last pressed key to a temponary buffer and analyzes it.
bool input__parse_key(V_file* const, const Config* const, Modes* const,
                      size_t* const, const char);

// Know what to do next with a pressed key or a combination. Based on ASCII.
bool input__parse_char(V_file* const, const Config* const, Modes* const,
                    const char);

// Add a char when a pressed key is a printable one.
bool input__printable_char(V_file* const, const char);

#endif
