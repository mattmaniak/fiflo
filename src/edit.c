#include "buffer.h"
#include "ascii.h"
#include "edit.h"

#include "memory.h"

static int delete_last_line(Buff_t* Buff)
{
	free(LAST_LINE);

	Buff->lines_i--;
	return memory.shrink_lines_array_mem(Buff);
}

static int delete_line(Buff_t* Buff)
{
	idx_t next_line_len = Buff->line_len_i[ACT_LINE_I + 1];

	if(!FIRST_LINE)
	{
		if(CURSOR_Y_SCROLLED)
		{
			Buff->cusr_x = (CURSOR_AT_LINE_START) ? next_line_len : 1;

			if(memory.copy_lines_mem_backward(Buff) == -1)
			{
				return -1;
			}
			if(delete_last_line(Buff) == -1)
			{
				return -1;
			}
			Buff->cusr_y--;
		}
		else
		{
			if(delete_last_line(Buff) == -1)
			{
				return -1;
			}

			/* With the last line deletion there is a need to remove the
			linefeed in the previous line. */
			LAST_LINE_LEN_I--;
			LAST_LINE[LAST_LINE_LEN_I] = '\0';

			Buff->cusr_x = 0;
		}
	}
	else
	{
		LAST_LINE_LEN_I = 0;
		LAST_LINE[LAST_LINE_LEN_I] = '\0';

		LAST_LINE = realloc(LAST_LINE, INIT_MEMBLK);
		memory.chk_ptr(Buff, LAST_LINE, "malloc after the first line removal");

		Buff->cusr_x = 0;
	}
	return 0;
}

static void shift_text_horizonally(Buff_t* Buff, char direction)
{
	const bool prev = 1;
	idx_t      char_i;

	switch(direction)
	{
		case 'l':
		for(char_i = CURSOR_VERTICAL_I; char_i <= ACT_LINE_LEN_I; char_i++)
		{
			ACT_LINE[char_i - prev] = ACT_LINE[char_i];
		}
		break;

		case 'r':
		for(char_i = ACT_LINE_LEN_I; char_i >= CURSOR_VERTICAL_I; char_i--)
		{
			ACT_LINE[char_i] = ACT_LINE[char_i - prev];
		}
	}
}

static int move_lines_forward(Buff_t* Buff)
{
	PREV_LINE_LEN_I -= Buff->cusr_x;

	// Move more lines vertically with the part of the current line.
	if(CURSOR_Y_SCROLLED)
	{
		if(memory.copy_lines_mem_forward(Buff) == -1)
		{
			return -1;
		}
		ACT_LINE_LEN_I = 0;
	}

	// Move the right part (separated by the cursor) of the line to the next.
	for(idx_t char_i = PREV_LINE_LEN_I;
	char_i < PREV_LINE_LEN_I + Buff->cusr_x; char_i++)
	{
		ACT_LINE[ACT_LINE_LEN_I] = PREV_LINE[char_i];
		ACT_LINE_LEN_I++;
		if(memory.extend_line_mem(Buff, ACT_LINE_I) == -1)
		{
			return -1;
		}
	}

	// Now the length of the upper line will be shortened after copying.
	PREV_LINE[PREV_LINE_LEN_I] = '\0';
	if(memory.shrink_prev_line_mem(Buff) == -1)
	{
		return -1;
	}
	return 0;
}

static int delete_last_empty_line(Buff_t* Buff)
{
	free(ACT_LINE);

	Buff->lines_i--;
	if(memory.shrink_act_line_mem(Buff) == -1)
	{
		return -1;
	}

	ACT_LINE_LEN_I--;
	Buff->chars_i--;

	if(memory.shrink_lines_array_mem(Buff) == -1)
	{
		return -1;
	}
	return 0;
}

static int delete_non_last_line(Buff_t* Buff)
{
	PREV_LINE_LEN_I--;
	Buff->chars_i--;

	// Append part of a next line to a previous one.
	for(idx_t char_i = 0; char_i <= ACT_LINE_LEN_I; char_i++)
	{
		PREV_LINE[PREV_LINE_LEN_I] = ACT_LINE[char_i];

		if(ACT_LINE[char_i] != '\0')
		{
			PREV_LINE_LEN_I++;
		}
		if(memory.extend_line_mem(Buff, PREV_LINE_I) == -1)
		{
			return -1;
		}
	}
	if(CURSOR_Y_SCROLLED)
	{
		if(memory.copy_lines_mem_backward(Buff) == -1)
		{
			return -1;
		}
	}
	if(delete_last_line(Buff) == -1)
	{
		return -1;
	}
	return 0;
}

static int delete_char(Buff_t* Buff)
{
	if(!CURSOR_AT_LINE_START)
	{
		shift_text_horizonally(Buff, 'l');
		if(memory.shrink_act_line_mem(Buff) == -1)
		{
			return -1;
		}

		ACT_LINE_LEN_I--;
		Buff->chars_i--;
	}
	// Deletes the non-empty line and copy chars to previous.
	else if(!FIRST_LINE)
	{
		if(delete_non_last_line(Buff) == -1)
		{
			return -1;
		}
	}
	return 0;
}

namespace_edit edit =
{
	delete_last_line,
	delete_line,
	shift_text_horizonally,
	move_lines_forward,
	delete_last_empty_line,
	delete_non_last_line,
	delete_char
};
