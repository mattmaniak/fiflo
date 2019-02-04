#ifndef BUFFER_H
#define BUFFER_H

/* This file is included in every source file to provide the main structure
that describes the buffer. */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

#define ERROR -1

// Max amount of chars: (16 MB - 1). Newline is also the char.
#define CHARS_MAX (UINT_MAX / 256)

#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Buffer->status, msg, STATUS_MAX - IDX) // TODO.

typedef struct // TODO
{
	bool chars_sequence;     // True if pressed key is ANSI escape code.
	bool live_filename_edit; // As in the name.
	bool pane_toggled;       // As in the name.
}
Modes_t;

typedef struct // TODO: SPLIT AND PADDING
{
	// Modes.
	bool     chars_sequence;     // True if pressed key is ANSI escape code.
	bool     live_fname_edit;    // As in the name.
	bool     pane_toggled;       // As in the name.

	// Max size is 250.
	char     git_branch[256];
	bool     padding_0;

	// Filename.
	char     fname[PATH_MAX + SLASH_SZ + NAME_MAX]; // Full filename. Eg. /home/user/basename.
	char     orig_fname[PATH_MAX + SLASH_SZ+ NAME_MAX];
	size_t   fname_length;       // Strlen of the above array.

	uint16_t padding_1;

	// File's content and some indicators.
	char**   text;               // Text buffer. E.g. text[lines_idx][chars_idx].
	idx_t    chars_idx;          // All chars index.
	idx_t    lines_idx;          // Lines index.
	idx_t*   lines_length_idx;   // Chars in the current line (index).

	// Visual shit.
	idx_t    preffered_cursor_rev_x; // E.g. end of the line offset when scrolling through the first chars.
	idx_t    cursor_rev_x;       // User's cursor position in the reversed X.
	idx_t    cursor_rev_y;       // As above but Y-axis.
	char     status[STATUS_MAX]; // Displayed message in the upper bar.
}
Buff_t;

// Bytes of the memory width.
#define ADDR_SZ          sizeof(Buffer->text)
#define INITIAL_MEMBLOCK (ADDR_SZ * sizeof(char)) // Aligned initial memblk.

// Must be >= 16 and dividable by 8.
#define MEMBLOCK         (idx_t) (128 * sizeof(char))

// Some placeholders.
#define CURRENT_LINE_IDX        (Buffer->lines_idx - Buffer->cursor_rev_y)
#define CURRENT_LINE            Buffer->text[CURRENT_LINE_IDX]
#define CURRENT_LINE_LENGTH_IDX Buffer->lines_length_idx[CURRENT_LINE_IDX]
#define CURSOR_X            (CURRENT_LINE_LENGTH_IDX - Buffer->cursor_rev_x)

#define PREVIOUS_LINE_IDX        (CURRENT_LINE_IDX - 1)
#define PREVIOUS_LINE            Buffer->text[PREVIOUS_LINE_IDX]
#define PREVIOUS_LINE_LENGTH_IDX Buffer->lines_length_idx[PREVIOUS_LINE_IDX]

#define LAST_LINE            Buffer->text[Buffer->lines_idx]
#define LAST_LINE_LENGTH_IDX Buffer->lines_length_idx[Buffer->lines_idx]

#define BUFFER_NOT_FULL      (Buffer->chars_idx        < CHARS_MAX)
#define CURSOR_X_SCROLLED    (Buffer->cursor_rev_x     > 0)
#define CURSOR_Y_SCROLLED    (Buffer->cursor_rev_y     > 0)
#define EMPTY_LINE           (CURRENT_LINE_LENGTH_IDX == 0)
#define FIRST_LINE           (CURRENT_LINE_IDX        == 0)
#define CURSOR_AT_LINE_START (Buffer->cursor_rev_x    == CURRENT_LINE_LENGTH_IDX)
#define CURSOR_AT_TOP        (Buffer->cursor_rev_y    == Buffer->lines_idx)

// Initializes all Buffer structure members.
bool buffer__init(Buff_t*);

// Display a error message and exit.
void buffer__free(Buff_t*);

#endif
