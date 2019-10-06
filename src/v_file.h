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
   recomended to set the V_FILE__CHAR_MAX to a value bigger than (INT_MAX - 1)
   because of possible casting. */

#define V_FILE__CHAR_MAX (INT_MAX / 128)

// if SIZE_MAX == SHRT_MAX
//     #define V_FILE__CHAR_MAX SHRT_MAX
// #else
//     #define V_FILE__CHAR_MAX (INT_MAX / 128)
// #endif

#define V_FILE__STATUS_MAX      32
#define V_FILE__SET_STATUS(msg) \
strncpy(v_file->status, msg, V_FILE__STATUS_MAX - SIZE__I)

typedef struct
{
    char     extension[NAME_MAX];

    // True if pressed key is ANSI escape code.
    bool     esc_seq_on_input;
    char     git_branch[NAME_MAX]; // Max size is 250 defined by Git.

    const int32_t _padding_0;

    // Filename.
    char*    pathname; // Doesn't include the trailing slash.
    char     basename[NAME_MAX];

    // Full filename. Eg. /home/user/basename.
    char     fname[PATH_MAX + NAME_MAX];
    char     fname_copy[PATH_MAX + NAME_MAX];

    const int8_t  _padding_1;
    const int16_t _padding_2;

    size_t   fname_len; // Strlen of the above array.

    // File's content and some indicators.
    Line*  lines;
    size_t   chars_amount; // All chars amount index.
    size_t   lines_amount; // All lines amount index.

    // Visual shit.
    size_t   mirrored_cursor_x;       // User's cursor position in the reversed X.
    size_t   mirrored_cursor_y;       // As above but in Y-axis.
    char     status[V_FILE__STATUS_MAX]; // Message displayed in a upper bar.
}
V_file;

// Aligned memory blocks.
#define V_FILE__BASIC_MEMBLK (sizeof(v_file->lines) * sizeof(char))

// Must be >= 16 and dividable by 8.
#define V_FILE__MEMBLK (size_t) (128 * sizeof(char))

// Some placeholders.
#define V_FILE__ACTUAL_LINE_I (v_file->lines_amount - v_file->mirrored_cursor_y)

#define V_FILE__ACTUAL_LINE v_file->lines[V_FILE__ACTUAL_LINE_I]

#define V_FILE__CURSOR_X (V_FILE__ACTUAL_LINE.len - v_file->mirrored_cursor_x)

#define V_FILE__ACTUAL_CHAR V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X]

#define V_FILE__PREV_CHAR V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__PREV]

#define V_FILE__LAST_CHAR_IN_LINE V_FILE__ACTUAL_LINE.txt[V_FILE__ACTUAL_LINE.len]

#define V_FILE__PREV_LINE_I (V_FILE__ACTUAL_LINE_I - SIZE__PREV)

#define V_FILE__PREV_LINE v_file->lines[V_FILE__PREV_LINE_I]

#define V_FILE__LAST_LINE v_file->lines[v_file->lines_amount]

#define V_FILE__CHAR_LIMIT_NOT_EXCEEDED (v_file->chars_amount < V_FILE__CHAR_MAX)

#define V_FILE__CURSOR_X_SCROLLED (v_file->mirrored_cursor_x > 0)

#define V_FILE__CURSOR_Y_SCROLLED (v_file->mirrored_cursor_y > 0)

#define V_FILE__EMPTY_LINE (V_FILE__ACTUAL_LINE.len == 0)

#define V_FILE__FIRST_LINE (V_FILE__ACTUAL_LINE_I == 0)

#define V_FILE__CURSOR_AT_LINE_START \
(v_file->mirrored_cursor_x == V_FILE__ACTUAL_LINE.len)

#define V_FILE__CURSOR_AT_TOP (v_file->mirrored_cursor_y == v_file->lines_amount)

V_file* v_file__new(void);

// Initialize all v_file structure members.
bool buffer__init(V_file* const);

// Display an error message and exit.
void v_file__delete(V_file* const);

#endif
