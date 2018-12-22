#include "include/buffer.h"
#include "include/ascii.h"
#include "include/edit.h"

#include "include/memory.h"

f_mtdt* delete_last_line(f_mtdt* Buff)
{
	free(LAST_LINE);

	Buff->lines_i--;
	Buff = memory.shrink_lines_array_mem(Buff);

	return Buff;
}

f_mtdt* delete_line(f_mtdt* Buff)
{
	buff_t next_line_len = Buff->line_len_i[ACT_LINE_I + 1];

	if(!FIRST_LINE)
	{
		if(CURSOR_Y_SCROLLED)
		{
			Buff->cusr_x = (CURSOR_AT_LINE_START) ? next_line_len : 1;

			Buff = memory.copy_lines_mem_backward(Buff);
			Buff = delete_last_line(Buff);

			Buff->cusr_y--;
		}
		else
		{
			Buff = delete_last_line(Buff);

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

		LAST_LINE = realloc(LAST_LINE, sizeof(Buff->text));
		chk_ptr(Buff, LAST_LINE, "malloc after the first line removal");

		Buff->cusr_x = 0;
	}
	return Buff;
}

f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction)
{
	const bool prev = 1;
	buff_t char_i;

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
	return Buff;
}

f_mtdt* move_lines_forward(f_mtdt* Buff)
{
	PREV_LINE_LEN_I -= Buff->cusr_x;

	// Move more lines vertically with the part of the current line.
	if(CURSOR_Y_SCROLLED)
	{
		Buff = memory.copy_lines_mem_forward(Buff);
		ACT_LINE_LEN_I = 0;
	}

	// Move the right part (separated by the cursor) of the line to the next.
	for(buff_t char_i = PREV_LINE_LEN_I;
	char_i < PREV_LINE_LEN_I + Buff->cusr_x; char_i++)
	{
		ACT_LINE[ACT_LINE_LEN_I] = PREV_LINE[char_i];
		ACT_LINE_LEN_I++;
		ACT_LINE = memory.extend_line_mem(Buff, ACT_LINE_I);
	}

	// Now the length of the upper line will be shortened after copying.
	PREV_LINE[PREV_LINE_LEN_I] = '\0';
	PREV_LINE = memory.shrink_prev_line_mem(Buff);

	return Buff;
}

f_mtdt* delete_last_empty_line(f_mtdt* Buff)
{
	free(ACT_LINE);

	Buff->lines_i--;
	ACT_LINE = memory.shrink_act_line_mem(Buff);

	ACT_LINE_LEN_I--;
	Buff->chars_i--;

	Buff = memory.shrink_lines_array_mem(Buff);

	return Buff;
}

f_mtdt* delete_non_last_line(f_mtdt* Buff)
{
	PREV_LINE_LEN_I--;
	Buff->chars_i--;

	// Append part of a next line to a previous one.
	for(buff_t char_i = 0; char_i <= ACT_LINE_LEN_I; char_i++)
	{
		PREV_LINE[PREV_LINE_LEN_I] = ACT_LINE[char_i];

		if(ACT_LINE[char_i] != '\0')
		{
			PREV_LINE_LEN_I++;
		}
		PREV_LINE = memory.extend_line_mem(Buff, PREV_LINE_I);
	}
	if(CURSOR_Y_SCROLLED)
	{
		Buff = memory.copy_lines_mem_backward(Buff);
	}
	Buff = delete_last_line(Buff);

	return Buff;
}

f_mtdt* delete_char(f_mtdt* Buff)
{
	if(!CURSOR_AT_LINE_START)
	{
		Buff = shift_text_horizonally(Buff, 'l');
		ACT_LINE = memory.shrink_act_line_mem(Buff);

		ACT_LINE_LEN_I--;
		Buff->chars_i--;
	}
	// Deletes the non-empty line and copy chars to previous.
	else if(!FIRST_LINE)
	{
		Buff = delete_non_last_line(Buff);
	}
	return Buff;
}
