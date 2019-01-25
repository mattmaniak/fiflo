#include "buffer.h"
#include "ascii.h"
#include "edit.h"

bool edit__delete_char(Buff_t* Buff)
{
	if(!CURSOR_AT_LINE_START)
	{
		edit__shift_text_horizonally(Buff, 'l');
		if(!memory__shrink_act_line(Buff))
		{
			return false;
		}
		CURRENT_LINE_LENGTH_IDX--;
		Buff->chars_i--;
	}
	// Deletes the non-empty line and copy chars to previous.
	else if(!FIRST_LINE)
	{
		if(!edit__move_lines_backward(Buff))
		{
			return false;
		}
	}
	return true;
}

bool edit__delete_line(Buff_t* Buff)
{
	idx_t next_line_idx = CURRENT_LINE_IDX + 1;
	idx_t next_line_len = Buff->line_len_i[next_line_idx];

	if(!FIRST_LINE)
	{
		if(CURSOR_Y_SCROLLED)
		{
			Buff->cusr_x = (CURSOR_AT_LINE_START) ? next_line_len : LF_SZ;

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
			LAST_LINE_LENGTH_IDX--;
			LAST_LINE[LAST_LINE_LENGTH_IDX] = '\0';

			Buff->cusr_x = 0;
		}
	}
	else
	{
		LAST_LINE_LENGTH_IDX = 0;
		LAST_LINE[LAST_LINE_LENGTH_IDX] = '\0';

		LAST_LINE = realloc(LAST_LINE, INITIAL_MEMBLOCK);
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
		for(char_i = CURSOR_X_POSITION; char_i <= CURRENT_LINE_LENGTH_IDX; char_i++)
		{
			CURRENT_LINE[char_i - prev] = CURRENT_LINE[char_i];
		}
		break;

		case 'r':
		for(char_i = CURRENT_LINE_LENGTH_IDX; char_i >= CURSOR_X_POSITION; char_i--)
		{
			CURRENT_LINE[char_i] = CURRENT_LINE[char_i - prev];
		}
	}
}

bool edit__move_lines_forward(Buff_t* Buff)
{
	PREVIOUS_LINE_LENGTH_IDX -= Buff->cusr_x;

	// Move more lines vertically with the part of the current line.
	if(CURSOR_Y_SCROLLED)
	{
		if(!memory__copy_lines_forward(Buff))
		{
			return false;
		}
		CURRENT_LINE_LENGTH_IDX = 0;
	}

	// Move the right part (separated by the cursor) of the line to the next.
	for(idx_t char_i = PREVIOUS_LINE_LENGTH_IDX;
	    char_i < PREVIOUS_LINE_LENGTH_IDX + Buff->cusr_x; char_i++)
	{
		CURRENT_LINE[CURRENT_LINE_LENGTH_IDX] = PREVIOUS_LINE[char_i];
		CURRENT_LINE_LENGTH_IDX++;
		if(!memory__extend_line(Buff, CURRENT_LINE_IDX))
		{
			return false;
		}
	}

	// Now the length of the upper line will be shortened after copying.
	PREVIOUS_LINE[PREVIOUS_LINE_LENGTH_IDX] = '\0';
	if(!memory__shrink_prev_line(Buff))
	{
		return false;
	}
	return true;
}

bool edit__move_lines_backward(Buff_t* Buff)
{
	Buff->chars_i--;
	PREVIOUS_LINE_LENGTH_IDX--;
	PREVIOUS_LINE[PREVIOUS_LINE_LENGTH_IDX] = '\0';

	PREVIOUS_LINE_LENGTH_IDX += CURRENT_LINE_LENGTH_IDX;
	if(!memory__extend_line(Buff, PREVIOUS_LINE_IDX))
	{
		return false;
	}
	// Append part of a next line to a previous one.
	strcat(PREVIOUS_LINE, CURRENT_LINE);

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

bool edit__delete_last_empty_line(Buff_t* Buff)
{
	free(CURRENT_LINE);

	Buff->lines_i--;
	if(!memory__shrink_act_line(Buff))
	{
		return false;
	}

	CURRENT_LINE_LENGTH_IDX--;
	Buff->chars_i--;

	if(!memory__shrink_lines_array(Buff))
	{
		return false;
	}
	return true;
}

bool edit__delete_last_line(Buff_t* Buff)
{
	free(LAST_LINE);

	Buff->lines_i--;
	return memory__shrink_lines_array(Buff);
}
