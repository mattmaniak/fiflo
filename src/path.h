#ifndef PATH_H
#define PATH_H

// Takes care about extracting names from the passed arg.

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

// Used when the non-absolute path is passed as arg.
bool path__extract_pathname_from_arg(Buff_t*);
void path__extract_basename_from_arg(Buff_t*);
void path__merge_pathname_and_basename(Buff_t*);

#endif
