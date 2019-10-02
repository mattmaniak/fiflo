#ifndef KEYS_H
#define KEYS_H

// Currently supported characters and their actions inside.

#include "v_file.h"
#include "config.h"
#include "modes.h"
#include "arrows.h"
#include "file_io.h"
#include "memory.h"
#include "edit.h"
#include "size.h"

// Know what to do next with a pressed key or a combination. Based on ASCII.
bool keys__parse_ch(V_file_t* const, const Conf_t* const, Mod_t* const,
                    const char);

// Add a char when a pressed key is a printable one.
bool keys__printable_ch(V_file_t* const, const char);

// Initialize a new line.
bool keys__linefeed(V_file_t* const);

// Remove a last char and optionally delete a last line.
bool keys__backspace(V_file_t* const, const Conf_t* const, const Mod_t* const);

// Insert a specified amount of the '\t' to emulate the Tab.
bool keys__tab(V_file_t* const, const Conf_t* const, const Mod_t* const);

#endif
