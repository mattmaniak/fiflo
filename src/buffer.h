#ifndef BUFFER_H
#define BUFFER_H

#define _POSIX_C_SOURCE 2 // For popen, pclose.

/* This file is included in every source file to provide the main structure
that describes the buffer. */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <string.h>
#include <limits.h>
#include <linux/limits.h>

// Needed to char/lines/chars_in_line values.
typedef uint32_t idx_t;

#define INDEX 1

// Max amount of chars: (16 MB - 1). Newline is also the char.
#define BUFF_MAX (UINT_MAX / 256)

#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Buff->status, msg, STATUS_MAX - INDEX)

typedef struct
{
	// Modes.
	bool     key_sequence;       // True if pressed key is ANSI escape code.
	bool     live_fname_edit;    // As in the name.
	bool     pane_toggled;       // As in the name.

	// Max size is 250.
	char     git_branch[256];
	bool     padding_0;

	// Filename.
	char     fname[PATH_MAX];    // Full filename. Eg. /home/user/basename.
	uint16_t fname_len_i;        // Strlen of the above array.

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

#define ADDR_SZ     sizeof(Buff->text) // Bytes of the memory width.
#define INIT_MEMBLK ADDR_SZ            // Aligned initial memblk for a new line.
#define MEMBLK      128                // Must be >= 16 and dividable by 8.

// Placeholders. Note that "_i" means "index".
#define ACT_LINE_I           (Buff->lines_i - Buff->cusr_y)
#define ACT_LINE             Buff->text[ACT_LINE_I]
#define ACT_LINE_LEN_I       Buff->line_len_i[ACT_LINE_I]
#define CURSOR_VERTICAL_I    (ACT_LINE_LEN_I - Buff->cusr_x)

#define PREV_LINE_I          (ACT_LINE_I - 1)
#define PREV_LINE            Buff->text[PREV_LINE_I]
#define PREV_LINE_LEN_I      Buff->line_len_i[PREV_LINE_I]

#define LAST_LINE            Buff->text[Buff->lines_i]
#define LAST_LINE_LEN_I      Buff->line_len_i[Buff->lines_i]

#define BUFFER_NOT_FULL      (Buff->chars_i  <  BUFF_MAX)
#define CURSOR_X_SCROLLED    (Buff->cusr_x   >  0)
#define CURSOR_Y_SCROLLED    (Buff->cusr_y   >  0)
#define EMPTY_LINE           (ACT_LINE_LEN_I == 0)
#define FIRST_LINE           (ACT_LINE_I     == 0)
#define CURSOR_AT_LINE_START (Buff->cusr_x   == ACT_LINE_LEN_I)
#define CURSOR_AT_TOP        (Buff->cusr_y   == Buff->lines_i)

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Initializes all Buff structure members.
	Buff_t* (*init)(Buff_t*);

	// Display a error message and exit.
	void (*free_all)(Buff_t*);

	// Frees everything and exits with status code.
	void (*throw_error)(Buff_t*, const char*);
}
namespace_buffer;

extern namespace_buffer buffer;

#endif
