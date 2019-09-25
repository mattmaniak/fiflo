#ifndef FILE_IO_H
#define FILE_IO_H

// File operations.

#include "v_file.h"
#include "config.h"
#include "arrows.h"
#include "modes.h"
#include "path.h"

#define FILE__AT_LEAST_ONE_TAB 1

extern bool chars__printable_char(V_file_t*, const char);

// Sets a filename. If basename is passed as arg actent path will be added.
bool file_io__set_name(V_file_t* const, const char* const);

// Read a file using "input_recognize_char".
bool file_io__load(V_file_t*, const Conf_t* const, const Mod_t* const);

// If there is tab char in a file, load e.g. 4 tabs to a memory.
bool file_io__convert_tab_from_file(V_file_t* const, const Conf_t* const,
                                    const Mod_t* const, const char);

// Convert fiflo's virtual tabs [e.g. "\t\t\t\t"] to the original '\t'.
void file_io__convert_tab_to_file(const V_file_t* const, const Conf_t* const,
                                  const size_t, size_t* const);

// Put a whole text to a file.
bool file_io__save(V_file_t* const, const Conf_t* const);

// Read a set branch from the ".git/" dir.
bool file_io__get_git_branch(V_file_t* const);

#endif
