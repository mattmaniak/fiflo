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

// Needed to char/lines/chars_in_line values.
typedef uint32_t idx_t;

#define IDX    1
#define NUL_SZ 1
#define LF_SZ  1

#define ERROR -1

// Max amount of chars: (16 MB - 1). Newline is also the char.
#define CHARS_MAX (UINT_MAX / 256)

#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Buff->status, msg, STATUS_MAX - IDX) // TODO.

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
	char     fname[PATH_MAX];    // Full filename. Eg. /home/user/basename.
	size_t   fname_len_i;        // Strlen of the above array.

	uint16_t padding_1;

	// File's content and some indicators.
	char**   text;               // Text buffer. Eg. text[lines_i][chars_i].
	idx_t    chars_i;            // All chars index.
	idx_t    lines_i;            // Lines index.
	idx_t*   line_len_i;         // Chars in the current line (index).

	// Visual shit.
	idx_t    cusr_x;             // User's cursor position in the reversed X.
	idx_t    cusr_y;             // As above but Y-axis.
	char     status[STATUS_MAX]; // Displayed message in the upper bar.
}
Buff_t;

// Bytes of the memory width.
#define ADDRESS_SZ     sizeof(Buff->text)
#define INITIAL_MEMBLOCK (ADDRESS_SZ * sizeof(char)) // Aligned initial memblk.
#define MEMBLOCK      (idx_t) (128 * sizeof(char)) // Must be >= 16 and dividable by 8.

// Placeholders. Note that "_i" means "index".
#define CURRENT_LINE_IDX        (Buff->lines_i - Buff->cusr_y)
#define CURRENT_LINE            Buff->text[CURRENT_LINE_IDX]
#define CURRENT_LINE_LENGTH_IDX Buff->line_len_i[CURRENT_LINE_IDX]
#define CURSOR_X_POSITION       (CURRENT_LINE_LENGTH_IDX - Buff->cusr_x)

#define PREVIOUS_LINE_IDX        (CURRENT_LINE_IDX - 1)
#define PREVIOUS_LINE            Buff->text[PREVIOUS_LINE_IDX]
#define PREVIOUS_LINE_LENGTH_IDX Buff->line_len_i[PREVIOUS_LINE_IDX]

#define LAST_LINE            Buff->text[Buff->lines_i]
#define LAST_LINE_LENGTH_IDX Buff->line_len_i[Buff->lines_i]

#define BUFFER_NOT_FULL      (Buff->chars_i  <  CHARS_MAX)
#define CURSOR_X_SCROLLED    (Buff->cusr_x   >  0)
#define CURSOR_Y_SCROLLED    (Buff->cusr_y   >  0)
#define EMPTY_LINE           (CURRENT_LINE_LENGTH_IDX == 0)
#define FIRST_LINE           (CURRENT_LINE_IDX     == 0)
#define CURSOR_AT_LINE_START (Buff->cusr_x   == CURRENT_LINE_LENGTH_IDX)
#define CURSOR_AT_TOP        (Buff->cusr_y   == Buff->lines_i)

// Initializes all Buff structure members.
bool buffer__init(Buff_t* Buff);

// Display a error message and exit.
void buffer__free(Buff_t* Buff);

#endif
