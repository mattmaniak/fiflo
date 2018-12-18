#include "include/buffer.h"
#include "include/seqmap.h"
#include "include/charmap.h"

f_mtdt* recognize_sequence(f_mtdt* Buff, char sequence[8])
{
	const uint8_t seq_max = 3;

	/* Notice that the structure of these sequences is: <ansi_esc_code> + '['
	+ <some_unique_numbers_and_letters>. */
	const char* arrow_up    = "\x1b[A";
	const char* arrow_down  = "\x1b[B";
	const char* arrow_right = "\x1b[C";
	const char* arrow_left  = "\x1b[D";

	if(!strcmp(sequence, arrow_up))
	{
		Buff = cursor_up(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_down))
	{
		Buff = cursor_down(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_right))
	{
		Buff = cursor_right(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_left))
	{
		Buff = cursor_left(Buff);
		Buff->key_sequence = false;
	}
	else if(strlen(sequence) > seq_max)
	{
		Buff->key_sequence = false;
	}

#ifdef SHOW_VALUES
	printf("cusr_x %d, cusr_y %d.\n", Buff->cusr_x, Buff->cusr_y);
#endif

	return Buff;
}

f_mtdt* cursor_left(f_mtdt* Buff)
{
	bool more_than_one_line = Buff->lines_i > 0;

	if(!CURSOR_AT_LINE_START)
	{
		Buff->cusr_x++;
	}
	else if(more_than_one_line && !CURSOR_AT_TOP)
	{
		// Set to the right ignoring the linefeed.
		Buff->cusr_x = 1;
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* cursor_right(f_mtdt* Buff)
{
	if(CURSOR_X_SCROLLED)
	{
		Buff->cusr_x--;
		if(!CURSOR_X_SCROLLED && CURSOR_Y_SCROLLED)
		{
			Buff->cusr_y--;
			Buff->cusr_x = ACT_LINE_LEN_I;
		}
		// Last line doesn't contain linefeed so ignoring that isn't necessary.
		else if(!CURSOR_X_SCROLLED && (Buff->cusr_y == 1))
		{
			Buff->cusr_y--;
		}
	}
	return Buff;
}

f_mtdt* cursor_up(f_mtdt* Buff)
{
	if(!CURSOR_AT_TOP)
	{
		/* Cursor at the left side: doesn't go at the end of a line. Always
		at the beginning or ignore the linefeed. */
		Buff->cusr_x = (CURSOR_AT_LINE_START) ? PREV_LINE_LEN_I : 1;
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* cursor_down(f_mtdt* Buff)
{
	bool cursor_at_prev_line_start = CURSOR_AT_LINE_START;

	if(CURSOR_Y_SCROLLED)
	{
		Buff->cusr_y--;

		if(cursor_at_prev_line_start)
		{
			/* Cursor at the left side: doesn't go at the end of a line. Always
			at the beginning */
			Buff->cusr_x = ACT_LINE_LEN_I;
		}
		else
		{
			// Ignore the linefeed or no.
			(CURSOR_Y_SCROLLED) ? Buff->cusr_x = 1 : 0;
		}
	}
	return Buff;
}
