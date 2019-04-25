#ifndef FILE_H
#define FILE_H

// File operations.

#include "buffer.h"
#include "config.h"
#include "keys.h"
#include "modes.h"
#include "path.h"

#define AT_LEAST_ONE_TAB 1

extern bool chars__printable_char(Buff_t*, const char);

// Sets the filename. If basename is passed as arg actent path will be added.
bool file__set_name(Buff_t*, const char* const);

// If there is tab char in a file, load e.g. 4 tabs to a memory.
bool file__convert_tab_from_file(Buff_t*, const Conf_t* const, const char);

// Reads the file using "input_recognize_char".
bool file__load(Buff_t*, const Conf_t* const);

// Converts in-fiflo tabs [e.g. "\t\t\t\t"] to the original '\t'.
void file__convert_tab_to_file(Buff_t*, const Conf_t* const, const idx_t,
                               idx_t* const);

// Puts the whole text to the file.
bool file__save(Buff_t*, const Conf_t* const);

// Reads the set branch from the ".git/" dir.
bool file__get_git_branch(Buff_t*);

#endif
