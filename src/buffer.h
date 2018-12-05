#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <linux/limits.h>

#define BUFF_MAX        (UINT_MAX / 256) // (16 MB - 1) buffer limit.
#define STATUS_MAX      40
#define SET_STATUS(msg) strncpy(Buff->status, msg, STATUS_MAX)

typedef uint32_t buff_t; // Only for amount indicators.

#pragma pack(push, 2)
#pragma pack(push, 1)
typedef struct
{
	uint16_t fname_len;
	bool     live_fname_edit;
	char     fname[PATH_MAX];    // Full filename. Eg. /home/user/basename.

	bool     key_sequence;

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

// Placeholders.
#define ACT_LINE_I        (Buff->lines_i - Buff->cusr_y)
#define ACT_LINE          Buff->text[ACT_LINE_I]
#define ACT_LINE_LEN_I    Buff->line_len_i[ACT_LINE_I]

#define CURSOR_VERTICAL_I (ACT_LINE_LEN_I - Buff->cusr_x)

#define PREV_LINE_I       (ACT_LINE_I - 1)
#define PREV_LINE         Buff->text[PREV_LINE_I]
#define PREV_LINE_LEN_I   Buff->line_len_i[PREV_LINE_I]

#define LAST_LINE         Buff->text[Buff->lines_i]
#define LAST_LINE_LEN_I   Buff->line_len_i[Buff->lines_i]

#endif
