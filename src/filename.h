#ifndef FILENAME_H
#define FILENAME_H

// Takes care about extracting names from a passed arg and setting names.

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "v_file.h"

// Set a filename. If basename is passed as an arg actual path will be added.
bool filename__set_name(V_file* const, const char* const);

// Used when a non-absolute path is passed as an arg.
bool filename__extract_pathname_from_arg(V_file* const);
void filename__extract_basename_from_arg(V_file* const);
void filename__merge_pathname_and_basename(V_file* const);

#endif
