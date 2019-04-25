#ifndef CHARS_H
#define CHARS_H

// Currently supported characters and their actions inside.

#include "buffer.h"
#include "config.h"
#include "modes.h"
#include "keys.h"
#include "file.h"
#include "memory.h"
#include "edit.h"

// Knows what to do next with pressed key or combination. Based on ASCII.
bool chars__parse_char(Buff_t*, const Conf_t* const, Mod_t*, const char);

// Adds char when the pressed key is a printable one.
bool chars__printable_char(Buff_t*, const char);

// Initialize the new line.
bool chars__linefeed(Buff_t*);

// Removes a last char and optionally deletes the last line.
bool chars__backspace(Buff_t*, const Conf_t* const);

// Inserts specified amount of '\t' to emulate the Tab.
bool chars__tab(Buff_t*, const Conf_t* const);

#endif
