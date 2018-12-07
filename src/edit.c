#include "buffer.h"
#include "charmap.h"
#include "edit.h"

f_mtdt* parse_key(f_mtdt* Buff, char key)
{
	enum           {seq_len = 8};
	static char    key_sequence[seq_len];
	static uint8_t char_i;

	if((key == ESC__CTRL_LEFT_SQR_BRACKET) && (!Buff->live_fname_edit))
	{

#ifndef VALUES_INSTEAD_OF_ACTIONS
		Buff->key_sequence = true;
#endif

		char_i = 0;
	}
	if(Buff->key_sequence)
	{
		key_sequence[char_i]          = key;
		key_sequence[char_i + NUL_SZ] = NUL__CTRL_SHIFT_2;

		if(char_i < (seq_len - NUL_SZ))
		{
			char_i++;
		}
		Buff = recognize_arrow(Buff, key_sequence);

		if(!Buff->key_sequence)
		{
			char_i = 0;
		}
	}
	else if(Buff->live_fname_edit)
	{
		Buff = edit_fname(Buff, key);
	}
	else if(!Buff->live_fname_edit)
	{
		Buff = key_action(Buff, key);
	}
	return Buff;
}

f_mtdt* delete_last_line(f_mtdt* Buff)
{
	safer_free(LAST_LINE);

	Buff->lines_i--;
	Buff = shrink_lines_array_mem(Buff);

	return Buff;
}

f_mtdt* delete_line(f_mtdt* Buff)
{
	buff_t next_line_len = Buff->line_len_i[ACT_LINE_I + 1];

	if(!ONE_LINE)
	{
		if(CURSOR_Y_SCROLLED)
		{
			if(CURSOR_AT_LINE_START)
			{
				Buff->cusr_x = next_line_len;
			}
			else
			{
				Buff->cusr_x = 1;
			}
			Buff = copy_lines_mem_backward(Buff);
			Buff = delete_last_line(Buff);

			Buff->cusr_y--;
		}
		else
		{
			Buff = delete_last_line(Buff);

			/* With the last line deletion there is a need to remove the
			linefeed in the previous line. */
			LAST_LINE_LEN_I--;
			LAST_LINE[LAST_LINE_LEN_I] = NUL__CTRL_SHIFT_2;

			Buff->cusr_x = 0;
		}
	}
	else
	{
		LAST_LINE_LEN_I = 0;
		LAST_LINE[LAST_LINE_LEN_I] = NUL__CTRL_SHIFT_2;

		LAST_LINE = realloc(LAST_LINE, sizeof(Buff->text));
		chk_ptr(Buff, LAST_LINE, "malloc after the first line removal");

		Buff->cusr_x = 0;
	}
	return Buff;
}

f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction)
{
	const bool prev = 1;
	buff_t     char_i;

	switch(direction)
	{
		case 'l':
		{
			for(char_i = CURSOR_VERTICAL_I; char_i <= ACT_LINE_LEN_I; char_i++)
			{
				ACT_LINE[char_i - prev] = ACT_LINE[char_i];
			}
			break;
		}
		case 'r':
		{
			for(char_i = ACT_LINE_LEN_I; char_i >= CURSOR_VERTICAL_I; char_i--)
			{
				ACT_LINE[char_i] = ACT_LINE[char_i - prev];
			}
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
		Buff = copy_lines_mem_forward(Buff);
		ACT_LINE_LEN_I = 0;
	}

	// Move the right part (separated by the cursor) of the line to the next.
	for(buff_t char_i = PREV_LINE_LEN_I;
	char_i < PREV_LINE_LEN_I + Buff->cusr_x; char_i++)
	{
		ACT_LINE[ACT_LINE_LEN_I] = PREV_LINE[char_i];
		ACT_LINE_LEN_I++;
		ACT_LINE = extend_line_mem(Buff, ACT_LINE_I);
	}

	// Now the length of the upper line will be shortened after copying.
	PREV_LINE[PREV_LINE_LEN_I] = NUL__CTRL_SHIFT_2;
	PREV_LINE = shrink_prev_line_mem(Buff);

	return Buff;
}
