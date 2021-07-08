#ifndef V_FILE_H
#define V_FILE_H

// An implementation of the virtual file structure.

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>

#include "extension.h"
#include "line.h"
#include "size.h"

/* Max amount of chars: (16 MB - 1). Newline is also a char. It's not
   recomended to set the V_FILE__CHAR_MAX to a value bigger than INT_MAX
   because of possible casting to the integer. */

#define V_FILE__CHAR_MAX (INT_MAX / 128)

#define V_FILE__STATUS_MAX      32
#define V_FILE__SET_STATUS(msg) \
strncpy(v_file->status, msg, strlen(msg) + SIZE__I);

// Aligned memory blocks.
#define V_FILE__BASIC_MEMBLOCK (sizeof(v_file->lines) * sizeof(char))

// Must be >= 16 and dividable by 8.
#define V_FILE__MEMBLOCK (size_t) (128 * sizeof(char))

typedef struct
{
    char extension[NAME_MAX];
    bool esc_seq_on_input; // True if a key generated an ANSI escape code.

    // Filename.
    char* pathname; // Doesn't include the trailing slash.
    char  basename[NAME_MAX];
    char  git_branch[NAME_MAX]; // Max size is 250 defined by Git.

    // Full filename. Eg. /home/user/basename.
    char fname[PATH_MAX + NAME_MAX];
    char fname_copy[PATH_MAX + NAME_MAX];

    const int32_t _padding;

    // File's content and some indicators.
    size_t fname_len; // Strlen of the above array.
    Line*  lines;
    size_t chars_amount; // All chars amount index.
    size_t lines_amount; // All lines amount index.

    // Visual shit.
    size_t mirrored_cursor_x; // E.g. cursor_x = line_len - mirrored_cursor_x.
    size_t mirrored_cursor_y;
    char   status[V_FILE__STATUS_MAX]; // Message displayed in a upper bar.
}
V_file;

// Initialize all v_file structure members.
bool v_file__init(V_file* const);

// Display an error message and exit.
void v_file__delete(V_file* const);

// Some basic getters those combine multiple values in one function.
size_t v_file__cursor_x(const V_file* const);

size_t v_file__cursor_y(const V_file* const);

// As they are pointers, they getters but can be also explicitly assigned.
char* v_file__actual_char(const V_file* const);

char* v_file__last_char_in_actual_line(const V_file* const);

char* v_file__last_char(const V_file* const);

Line* v_file__actual_line(const V_file* const);

Line* v_file__prev_line(const V_file* const);

Line* v_file__last_line(const V_file* const);

// And some boolean getters.
bool v_file__is_cursor_x_scrolled(const V_file* const);

bool v_file__is_cursor_y_scrolled(const V_file* const);

bool v_file__is_actual_line_empty(const V_file* const);

bool v_file__is_actual_line_first(const V_file* const);

bool v_file__is_cursor_at_top(const V_file* const);

bool v_file__is_cursor_at_line_start(const V_file* const);

#endif
