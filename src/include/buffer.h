#ifndef BUFFER_H
#define BUFFER_H

/* This file is included in every source file to provide the main structure
that describes the buffer. */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <linux/limits.h>

// Needed to char/lines/chars_in_line values.
typedef uint32_t buff_t;

// Max amount of chars: (16 MB - 1). Newline is also the char.
#define BUFF_MAX (UINT_MAX / 256)

#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Buff->status, msg, STATUS_MAX - 1)

#define INDEX 1

#pragma pack(push, 2)
#pragma pack(push, 1)
typedef struct
{
	// Modes.
	bool     key_sequence;       // True if pressed key is ANSI escape code.
	bool     live_fname_edit;    // As in the name.
	bool     pane_toggled;       // As in the name.

	// Filename.
	char     fname[PATH_MAX];    // Full filename. Eg. /home/user/basename.
	uint16_t fname_len_i;        // Strlen of the above array.

	// File's content and some indicators.
	char**   text;               // Text buffer. Eg. text[lines_i][chars_i].
	buff_t   lines_i;            // Lines index.
	buff_t*  line_len_i;         // Chars in the current line (index).
	buff_t   chars_i;            // All chars index.

	// Visual shit.
	buff_t   cusr_x;             // User's cursor position in the reversed X.
	buff_t   cusr_y;             // As above but Y-axis.
	char     status[STATUS_MAX]; // Displayed message in the upper bar.
}
f_mtdt;
#pragma pack(pop)
#pragma pack(pop)

// Placeholders. Note that "_i" means "index".
#define ACT_LINE_I        (Buff->lines_i - Buff->cusr_y)
#define ACT_LINE          Buff->text[ACT_LINE_I]
#define ACT_LINE_LEN_I    Buff->line_len_i[ACT_LINE_I]
#define CURSOR_VERTICAL_I (ACT_LINE_LEN_I - Buff->cusr_x)

#define PREV_LINE_I     (ACT_LINE_I - 1)
#define PREV_LINE       Buff->text[PREV_LINE_I]
#define PREV_LINE_LEN_I Buff->line_len_i[PREV_LINE_I]

#define LAST_LINE       Buff->text[Buff->lines_i]
#define LAST_LINE_LEN_I Buff->line_len_i[Buff->lines_i]

#define BUFFER_NOT_FULL      (Buff->chars_i  <  BUFF_MAX)
#define CURSOR_X_SCROLLED    (Buff->cusr_x   >  0)
#define CURSOR_Y_SCROLLED    (Buff->cusr_y   >  0)
#define EMPTY_LINE           (ACT_LINE_LEN_I == 0)
#define FIRST_LINE           (ACT_LINE_I     == 0)
#define CURSOR_AT_LINE_START (Buff->cusr_x   == ACT_LINE_LEN_I)
#define CURSOR_AT_TOP        (Buff->cusr_y   == Buff->lines_i)

#endif