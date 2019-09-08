#ifndef PATH_H
#define PATH_H

// Takes care about extracting names from a passed arg.

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

// Used when a non-absolute path is passed as an arg.
bool path__extract_pathname_from_arg(Buff_t*);
void path__extract_basename_from_arg(Buff_t*);
void path__merge_pathname_and_basename(Buff_t*);

#endif
