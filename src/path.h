#ifndef PATH_H
#define PATH_H

// Takes care about extracting names from a passed arg.

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "v_file.h"

// Used when a non-absolute path is passed as an arg.
bool path__extract_pathname_from_arg(V_file_t* const);
void path__extract_basename_from_arg(V_file_t* const);
void path__merge_pathname_and_basename(V_file_t* const);

#endif
