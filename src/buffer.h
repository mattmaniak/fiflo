#ifndef BUFFER_H
#define BUFFER_H

/* This file is included in every source file to provide the main structure
   that describes the single file buffer. */

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
   recomended to set the CHARS_MAX to a value bigger than (INT_MAX - 1)
   because of possible casting. */
#define CHARS_MAX (SIZE_MAX / 128)

#define BUFFER__STATUS_MAX      32
#define BUFFER__SET_STATUS(msg) \
strncpy(Buffer->status, msg, BUFFER__STATUS_MAX - SIZE__I)

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
    char     status[BUFFER__STATUS_MAX]; // Message displayed in a upper bar.
}
Buff_t;

// Aligned memory blocks.
#define BUFFER__BASIC_MEMBLK (sizeof(Buffer->Lines) * sizeof(char))

// Must be >= 16 and dividable by 8.
#define BUFFER__MEMBLK (const size_t) (128 * sizeof(char))

// Some placeholders.
#define BUFFER__ACTUAL_LN_I (Buffer->ln_amount - Buffer->cursor_rev_y)

#define BUFFER__ACTUAL_LN Buffer->Lines[BUFFER__ACTUAL_LN_I]

#define BUFFER__CURSOR_X (BUFFER__ACTUAL_LN.len - Buffer->cursor_rev_x)

#define BUFFER__ACTUAL_CH BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X]

#define BUFFER__PREV_CH BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__PREV]

#define BUFFER__LAST_CH_IN_LN BUFFER__ACTUAL_LN.txt[BUFFER__ACTUAL_LN.len]

#define BUFFER__PREV_LN_I (BUFFER__ACTUAL_LN_I - SIZE__PREV)

#define BUFFER__PREV_LN Buffer->Lines[BUFFER__PREV_LN_I]

#define BUFFER__LAST_LN Buffer->Lines[Buffer->ln_amount]

#define BUFFER__CH_LIMIT_NOT_EXCEEDED (Buffer->ch_amount < CHARS_MAX)

#define BUFFER__CURSOR_X_SCROLLED (Buffer->cursor_rev_x > 0)

#define BUFFER__CURSOR_Y_SCROLLED (Buffer->cursor_rev_y > 0)

#define BUFFER__EMPTY_LN (BUFFER__ACTUAL_LN.len == 0)

#define BUFFER__FIRST_LN (BUFFER__ACTUAL_LN_I == 0)

#define BUFFER__CURSOR_AT_LN_START \
(Buffer->cursor_rev_x == BUFFER__ACTUAL_LN.len)

#define BUFFER__CURSOR_AT_TOP (Buffer->cursor_rev_y == Buffer->ln_amount)

// Initializes all Buffer structure members.
bool buffer__init(Buff_t* const);

// Display a error message and exit.
void buffer__free(Buff_t* const);

#endif
