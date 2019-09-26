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

#include "line.h"
#include "size.h"

/* Max amount of chars: (16 MB - 1). Newline is also a char. It's not
   recomended to set the V_FILE__CH_MAX to a value bigger than (INT_MAX - 1)
   because of possible casting. */
#define V_FILE__CH_MAX USHRT_MAX // TODO

#define V_FILE__STATUS_MAX      32
#define V_FILE__SET_STATUS(msg) \
strncpy(V_file->status, msg, V_FILE__STATUS_MAX - SIZE__I)

typedef struct
{
    int      extension;

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
    Line_t*  Lines;
    size_t   ch_amount; // All chars amount index.
    size_t   ln_amount; // All lines amount index.

    // Visual shit.
    size_t   cursor_rev_x;       // User's cursor position in the reversed X.
    size_t   cursor_rev_y;       // As above but in Y-axis.
    char     status[V_FILE__STATUS_MAX]; // Message displayed in a upper bar.
}
V_file_t;

// Aligned memory blocks.
#define V_FILE__BASIC_MEMBLK (sizeof(V_file->Lines) * sizeof(char))

// Must be >= 16 and dividable by 8.
#define V_FILE__MEMBLK (size_t) (128 * sizeof(char))

// Some placeholders.
#define V_FILE__ACTUAL_LN_I (V_file->ln_amount - V_file->cursor_rev_y)

#define V_FILE__ACTUAL_LN V_file->Lines[V_FILE__ACTUAL_LN_I]

#define V_FILE__CURSOR_X (V_FILE__ACTUAL_LN.len - V_file->cursor_rev_x)

#define V_FILE__ACTUAL_CH V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X]

#define V_FILE__PREV_CH V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__PREV]

#define V_FILE__LAST_CH_IN_LN V_FILE__ACTUAL_LN.txt[V_FILE__ACTUAL_LN.len]

#define V_FILE__PREV_LN_I (V_FILE__ACTUAL_LN_I - SIZE__PREV)

#define V_FILE__PREV_LN V_file->Lines[V_FILE__PREV_LN_I]

#define V_FILE__LAST_LN V_file->Lines[V_file->ln_amount]

#define V_FILE__CH_LIMIT_NOT_EXCEEDED (V_file->ch_amount < V_FILE__CH_MAX)

#define V_FILE__CURSOR_X_SCROLLED (V_file->cursor_rev_x > 0)

#define V_FILE__CURSOR_Y_SCROLLED (V_file->cursor_rev_y > 0)

#define V_FILE__EMPTY_LN (V_FILE__ACTUAL_LN.len == 0)

#define V_FILE__FIRST_LN (V_FILE__ACTUAL_LN_I == 0)

#define V_FILE__CURSOR_AT_LN_START \
(V_file->cursor_rev_x == V_FILE__ACTUAL_LN.len)

#define V_FILE__CURSOR_AT_TOP (V_file->cursor_rev_y == V_file->ln_amount)

// Initialize all V_file structure members.
bool buffer__init(V_file_t* const);

// Display an error message and exit.
void buffer__free(V_file_t* const);

#endif
