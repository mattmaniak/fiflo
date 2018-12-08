#include "buffer.h"
#include "seqmap.h"

f_mtdt* recognize_sequence(f_mtdt* Buff, char sequence[8])
{
	const uint8_t seq_max = 4;

	/* Notice that the structure of these sequences is:
	<ansi_esc_code> + '[' + <some_unique_numbers_and_letters> + '\0'.
	Comments at the right side are identifiers of these codes. */
	const char* arrow_up    = "\x1b\x5b\x41\x00"; // A
	const char* arrow_down  = "\x1b\x5b\x42\x00"; // B
	const char* arrow_right = "\x1b\x5b\x43\x00"; // C
	const char* arrow_left  = "\x1b\x5b\x44\x00"; // D

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
	// Prevent everything other than bare arrows in a very ugly way.
	else if(strlen(sequence) > seq_max)
	{
		getch(Buff);

		if(strlen(sequence) == seq_max + 2)
		{
			getch(Buff);
		}
		Buff->key_sequence = false;
	}
	return Buff;
}

f_mtdt* cursor_left(f_mtdt* Buff)
{
	if(Buff->cusr_x < ACT_LINE_LEN_I)
	{
		Buff->cusr_x++;
	}
	else if((Buff->lines_i > 0) && (Buff->cusr_y < Buff->lines_i))
	{
		// Set to the right ignoring the linefeed.
		Buff->cusr_x = 1;
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* cursor_right(f_mtdt* Buff)
{
	if(Buff->cusr_x > 0)
	{
		Buff->cusr_x--;
		if((Buff->cusr_y > 0) && (Buff->cusr_x == 0))
		{
			Buff->cusr_y--;
			Buff->cusr_x = ACT_LINE_LEN_I;
		}
		// Last line doesn't contain linefeed so ignoring that isn't necessary.
		else if((Buff->cusr_y == 1) && (Buff->cusr_x == 0))
		{
			Buff->cusr_y--;
		}
	}
	return Buff;
}

f_mtdt* cursor_up(f_mtdt* Buff)
{
	if(Buff->cusr_y < Buff->lines_i)
	{
		if(Buff->cusr_x == ACT_LINE_LEN_I)
		{
			/* Cursor at the left side: doesn't go at the end of a line. Always
			at the beginning */
			Buff->cusr_x = PREV_LINE_LEN_I;
		}
		else
		{
			// Ignore the linefeed.
			Buff->cusr_x = 1;
		}
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* cursor_down(f_mtdt* Buff)
{
	if(Buff->cusr_y > 0)
	{
		Buff->cusr_y--;

		if(Buff->cusr_x == PREV_LINE_LEN_I)
		{
			/* Cursor at the left side: doesn't go at the end of a line. Always
			at the beginning */
			Buff->cusr_x = ACT_LINE_LEN_I;
		}
		else
		{
			// Is last line so ignoring the LF isn't needed.
			if(Buff->cusr_y == 0)
			{
				Buff->cusr_x = 0;
			}
			else
			{
				// Ignore the linefeed.
				Buff->cusr_x = 1;
			}
		}
	}
	return Buff;
}
