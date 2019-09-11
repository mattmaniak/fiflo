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

// Macros that makes a life a little bit more readable.
#define IDX       1
#define NUL_SZ    1
#define LF_SZ     1
#define CURSOR_SZ 1
#define SPACE_SZ  1
#define SLASH_SZ  1
#define PREV      1
#define NEXT      1
#define ERROR     -1

// Max amount of chars: (16 MB - 1). Newline is also a char.
#define CHARS_MAX (UINT_MAX / 256)

#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Buffer->status, msg, STATUS_MAX - IDX)

typedef struct
{
    int      extension;

    // True if pressed key is ANSI escape code.
    bool     escape_sequence_on_input;
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
    idx_t    chars_amount; // All chars index.
    idx_t    lines_amount; // Lines index.

    // Visual shit.
    idx_t    cursor_rev_x;       // User's cursor position in the reversed X.
    idx_t    cursor_rev_y;       // As above but in Y-axis.
    char     status[STATUS_MAX]; // Message displayed in a upper bar.
}
Buff_t;

// Aligned memory blocks.
#define BUFFER__BASIC_MEMBLK (sizeof(Buffer->Lines) * sizeof(char))

// Must be >= 16 and dividable by 8.
#define BUFFER__MEMBLK (const idx_t) (128 * sizeof(char))

// Some placeholders.
#define BUFFER__ACTUAL_LINE_IDX \
(Buffer->lines_amount - Buffer->cursor_rev_y)

#define BUFFER__ACTUAL_LINE Buffer->Lines[BUFFER__ACTUAL_LINE_IDX].text

#define BUFFER__ACTUAL_LINE_LEN  \
Buffer->Lines[BUFFER__ACTUAL_LINE_IDX].length

#define BUFFER__CURSOR_X (BUFFER__ACTUAL_LINE_LEN - Buffer->cursor_rev_x)

#define BUFFER__ACTUAL_CHAR BUFFER__ACTUAL_LINE[BUFFER__CURSOR_X]

#define BUFFER__PREV_CHAR BUFFER__ACTUAL_LINE[BUFFER__CURSOR_X - PREV]

#define BUFFER__LAST_CHAR_IN_LINE \
BUFFER__ACTUAL_LINE[BUFFER__ACTUAL_LINE_LEN]

#define BUFFER__PREV_LINE_IDX (BUFFER__ACTUAL_LINE_IDX - PREV)

#define BUFFER__PREV_LINE Buffer->Lines[BUFFER__PREV_LINE_IDX].text

#define BUFFER__PREV_LINE_LEN \
Buffer->Lines[BUFFER__PREV_LINE_IDX].length

#define BUFFER__LAST_LINE Buffer->Lines[Buffer->lines_amount].text

#define BUFFER__LAST_LINE_LEN Buffer->Lines[Buffer->lines_amount].length

#define BUFFER__CHARS_LIMIT_NOT_EXCEEDED (Buffer->chars_amount < CHARS_MAX)

#define BUFFER__CURSOR_X_SCROLLED (Buffer->cursor_rev_x > 0)

#define BUFFER__CURSOR_Y_SCROLLED (Buffer->cursor_rev_y > 0)

#define BUFFER__EMPTY_LINE (BUFFER__ACTUAL_LINE_LEN == 0)

#define BUFFER__FIRST_LINE (BUFFER__ACTUAL_LINE_IDX == 0)

#define BUFFER__CURSOR_AT_LINE_BEGINNING \
(Buffer->cursor_rev_x == BUFFER__ACTUAL_LINE_LEN)

#define BUFFER__CURSOR_AT_TOP \
(Buffer->cursor_rev_y == Buffer->lines_amount)

// Initializes all Buffer structure members.
bool buffer__init(Buff_t*);

// Display a error message and exit.
void buffer__free(Buff_t*);

#endif
