#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

bool path__extract_path_from_arg(Buff_t*);
void path__extract_basename_from_arg(Buff_t*);
void path__merge_path_and_basename(Buff_t*);

#endif
