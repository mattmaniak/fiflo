#include "buffer.h"
#include "ascii.h"
#include "edit.h"

bool edit__delete_last_line(Buff_t* Buff)
{
	free(LAST_LINE);

	Buff->lines_i--;
	return memory__shrink_lines_array(Buff);
}

bool edit__delete_line(Buff_t* Buff)
{
	idx_t next_line_len = Buff->line_len_i[ACT_LINE_I + 1];

	if(!FIRST_LINE)
	{
		if(CURSOR_Y_SCROLLED)
		{
			Buff->cusr_x = (CURSOR_AT_LINE_START) ? next_line_len : 1;

			if(!memory__copy_lines_backward(Buff))
			{
				return false;
			}
			if(!edit__delete_last_line(Buff))
			{
				return false;
			}
			Buff->cusr_y--;
		}
		else
		{
			if(!edit__delete_last_line(Buff))
			{
				return false;
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
		if(LAST_LINE == NULL)
		{
			fprintf(stderr, "Can't realloc a memory in the first line.\n");
		}
		Buff->cusr_x = 0;
	}
	return true;
}

void edit__shift_text_horizonally(Buff_t* Buff, char direction)
{
	const size_t prev = 1;
	idx_t        char_i;

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

bool edit__move_lines_forward(Buff_t* Buff)
{
	PREV_LINE_LEN_I -= Buff->cusr_x;

	// Move more lines vertically with the part of the current line.
	if(CURSOR_Y_SCROLLED)
	{
		if(!memory__copy_lines_forward(Buff))
		{
			return false;
		}
		ACT_LINE_LEN_I = 0;
	}

	// Move the right part (separated by the cursor) of the line to the next.
	for(idx_t char_i = PREV_LINE_LEN_I;
	    char_i < PREV_LINE_LEN_I + Buff->cusr_x; char_i++)
	{
		ACT_LINE[ACT_LINE_LEN_I] = PREV_LINE[char_i];
		ACT_LINE_LEN_I++;
		if(!memory__extend_line(Buff, ACT_LINE_I))
		{
			return false;
		}
	}

	// Now the length of the upper line will be shortened after copying.
	PREV_LINE[PREV_LINE_LEN_I] = '\0';
	if(!memory__shrink_prev_line(Buff))
	{
		return false;
	}
	return true;
}

bool edit__delete_last_empty_line(Buff_t* Buff)
{
	free(ACT_LINE);

	Buff->lines_i--;
	if(!memory__shrink_act_line(Buff))
	{
		return false;
	}

	ACT_LINE_LEN_I--;
	Buff->chars_i--;

	if(!memory__shrink_lines_array(Buff))
	{
		return false;
	}
	return true;
}

bool edit__delete_non_last_line(Buff_t* Buff)
{
	Buff->chars_i--;
	PREV_LINE_LEN_I--;
	PREV_LINE[PREV_LINE_LEN_I] = '\0';

	PREV_LINE_LEN_I += ACT_LINE_LEN_I;
	if(!memory__extend_line(Buff, PREV_LINE_I))
	{
		return false;
	}
	// Append part of a next line to a previous one.
	strcat(PREV_LINE, ACT_LINE);

	if(CURSOR_Y_SCROLLED)
	{
		if(!memory__copy_lines_backward(Buff))
		{
			return false;
		}
	}
	if(!edit__delete_last_line(Buff))
	{
		return false;
	}
	return true;
}

bool edit__delete_char(Buff_t* Buff)
{
	if(!CURSOR_AT_LINE_START)
	{
		edit__shift_text_horizonally(Buff, 'l');
		if(!memory__shrink_act_line(Buff))
		{
			return false;
		}
		ACT_LINE_LEN_I--;
		Buff->chars_i--;
	}
	// Deletes the non-empty line and copy chars to previous.
	else if(!FIRST_LINE)
	{
		if(!edit__delete_non_last_line(Buff))
		{
			return false;
		}
	}
	return true;
}
