#ifndef FILE_IO_H
#define FILE_IO_H

// File operations.

#include "v_file.h"
#include "config.h"
#include "modes.h"
#include "path.h"

#define FILE__AT_LEAST_ONE_TAB 1

extern bool input__printable_char(V_file* const, const char);

// Set a filename. If basename is passed as an arg actual path will be added.
bool file_io__set_name(V_file* const, const char* const);

// Load a file to the program.
bool file_io__load(V_file* const, const Config* const, const Modes* const);

// If there is tab char in a file, load e.g. 4 tabs to a memory.
bool file_io__convert_tab_from_file(V_file* const, const Config* const,
                                    const Modes* const, const char);

// Convert fiflo's virtual tabs, e.g. "\t\t\t\t" to the original '\t'.
void file_io__convert_tab_to_file(const V_file* const, const Config* const,
                                  const size_t, size_t* const);

// Put a whole text to a file.
bool file_io__save(V_file* const, const Config* const);

// Read a set branch from the ".git/" dir.
bool file_io__get_git_branch(V_file* const);

#endif
