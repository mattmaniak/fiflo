#ifndef BUFFER_H
#define BUFFER_H

/* This file is included in every source file to provide the main structure
that describes the buffer. */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>

// Just the type for indexes.
typedef uint32_t idx_t;

#define IDX       1
#define NUL_SZ    1
#define LF_SZ     1
#define CURSOR_SZ 1
#define SPACE_SZ  1
#define SLASH_SZ  1
#define PREV      1
#define NEXT      1
#define ERROR     -1

// Max amount of chars: (16 MB - 1). Newline is also the char.
#define CHARS_MAX (UINT_MAX / 256)

#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Buffer->status, msg, STATUS_MAX - IDX)

typedef struct
{
    // True if pressed key is ANSI escape code.
    bool     escape_sequence_on_input;
    char     git_branch[NAME_MAX]; // Max size is 250 defined by Git.

    // Filename.
    char*    pathname; // Doesn't include the trailing slash.
    char     basename[NAME_MAX];

    // Full filename. Eg. /home/user/basename.
    char     fname[PATH_MAX + NAME_MAX];
    char     fname_copy[PATH_MAX + NAME_MAX];

    const uint16_t padding;

    size_t   fname_length; // Strlen of the above array.

    // File's content and some indicators.
    char**   text;         // Text buffer. E.g. text[lines_idx][chars_idx].
    idx_t    chars_idx;    // All chars index.
    idx_t    lines_idx;    // Lines index.
    idx_t*   lines_length; // Chars in the current line (index).

    // Mostly visual shit.
    idx_t    cursor_rev_x;       // User's cursor position in the reversed X.
    idx_t    cursor_rev_y;       // As above but in Y-axis.
    char     status[STATUS_MAX]; // Displayed message in the upper bar.
}
Buff_t;

// Bytes of the memory width.
#define ADDR_SZ              sizeof(Buffer->text)
#define INITIAL_MEMBLOCK     (ADDR_SZ * sizeof(char)) // Also aligned.

// Must be >= 16 and dividable by 8.
#define MEMBLOCK             (idx_t) (128 * sizeof(char))

// Some placeholders.
#define CURRENT_LINE_IDX     (Buffer->lines_idx - Buffer->cursor_rev_y)
#define CURRENT_LINE         Buffer->text[CURRENT_LINE_IDX]
#define CURRENT_LINE_LENGTH  Buffer->lines_length[CURRENT_LINE_IDX]
#define CURSOR_X             (CURRENT_LINE_LENGTH - Buffer->cursor_rev_x)
#define CURRENT_CHAR         CURRENT_LINE[CURSOR_X]
#define PREVIOUS_CHAR        CURRENT_LINE[CURSOR_X - 1]
#define LAST_CHAR_IN_LINE    CURRENT_LINE[CURRENT_LINE_LENGTH]

#define PREVIOUS_LINE_IDX    (CURRENT_LINE_IDX - 1)
#define PREVIOUS_LINE        Buffer->text[PREVIOUS_LINE_IDX]
#define PREVIOUS_LINE_LENGTH Buffer->lines_length[PREVIOUS_LINE_IDX]

#define LAST_LINE            Buffer->text[Buffer->lines_idx]
#define LAST_LINE_LENGTH     Buffer->lines_length[Buffer->lines_idx]

#define BUFFER_NOT_FULL      (Buffer->chars_idx    < CHARS_MAX)
#define CURSOR_X_SCROLLED    (Buffer->cursor_rev_x > 0)
#define CURSOR_Y_SCROLLED    (Buffer->cursor_rev_y > 0)
#define EMPTY_LINE           (CURRENT_LINE_LENGTH  == 0)
#define FIRST_LINE           (CURRENT_LINE_IDX     == 0)
#define CURSOR_AT_LINE_START (Buffer->cursor_rev_x == CURRENT_LINE_LENGTH)
#define CURSOR_AT_TOP        (Buffer->cursor_rev_y == Buffer->lines_idx)

// Initializes all Buffer structure members.
bool buffer__init(Buff_t*);

// Display a error message and exit.
void buffer__free(Buff_t*);

#endif
