#include "buffer.h"
#include "keys.h"

f_mtdt* key_action(f_mtdt* Buff, char key)
{
	switch(key)
	{
		case NEG:
		{
			fputs("Pipe isn't supported, exit(1).\n", stderr);
			free_buff_exit(Buff, 1);
		}
		case HT__CTRL_I:
		{
			// Currently converts the tab to two spaces.
			for(uint8_t tab_width = 0; tab_width < 2; tab_width++)
			{
				Buff = printable_char(Buff, ' ');
			}
			break;
		}
		case DEL__BACKSPACE:
		{
			Buff = backspace(Buff);
			break;
		}
		case DC1__CTRL_Q:
		{
			free_buff_exit(Buff, 1);
		}
		case DC3__CTRL_S:
		{
			Buff = save_file(Buff);
			break;
		}
		case SI__CTRL_O:
		{
			Buff->live_fname_edit = true;
			break;
		}
		case EOT__CTRL_D:
		{
			Buff = delete_line(Buff);
			break;
		}
		default:
		{
			Buff = printable_char(Buff, key);
		}
	}

#ifdef DEBUG
	printf("pressed_key %d, cusr_x %d, cusr_y %d.\n",
	key, Buff->cusr_x, Buff->cusr_y);
#endif

	return Buff;
}

f_mtdt* printable_char(f_mtdt* Buff, char key)
{
	const bool nul_sz = 1;

	/* Only printable chars will be added. Combinations that aren't specified
	above will be omited. Set "if(key)" to enable them. */
	if((key == NUL__CTRL_SHIFT_2) || (key == LF__CTRL_J) || (key >= 32))
	{
		if(Buff->chars_i < BUFF_MAX)
		{
			Buff->chars_i++;
			ACT_LINE_LEN_I++;

			ACT_LINE = extend_line_mem(Buff, ACT_LINE_I);

			if(Buff->cusr_x > 0)
			{
				Buff = shift_text_horizonally(Buff, 'r');
			}
			ACT_LINE[CURSOR_VERTICAL_I - nul_sz] = key;
			ACT_LINE[ACT_LINE_LEN_I] = NUL__CTRL_SHIFT_2;

			// Initializer handling.
			if(key == NUL__CTRL_SHIFT_2 && ACT_LINE_LEN_I > 0)
			{
				Buff->chars_i--;
				ACT_LINE_LEN_I--;
			}
			else if(key == LF__CTRL_J)
			{
				Buff = linefeed(Buff);
			}
			if(key != NUL__CTRL_SHIFT_2)
			{
				SET_STATUS("edited\0");
			}
		}
		else
		{
			SET_STATUS("can't read or insert more chars\0");
		}
	}
	else
	{
		SET_STATUS("WARNING - unsupported octet(s)\0");
	}
	return Buff;
}

f_mtdt* linefeed(f_mtdt* Buff)
{
	if(Buff->lines_i < BUFF_MAX)
	{
		Buff->lines_i++;
		Buff = extend_lines_array_mem(Buff);

		if(Buff->cusr_x > 0)
		{
			PREV_LINE_LEN_I -= Buff->cusr_x;

			// Move more lines vertically with the part of the current line.
			if(Buff->cusr_y > 0)
			{
				Buff = copy_lines_forward(Buff);
				ACT_LINE_LEN_I = 0;
			}

			/* Move the right part (separated by the cursor) of the line to the
			next. */
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
		}
		// Cursor is at the end of the line. Shifted vertically.
		else if(Buff->cusr_y > 0)
		{
			Buff = copy_lines_forward(Buff);
		}
		ACT_LINE[ACT_LINE_LEN_I] = NUL__CTRL_SHIFT_2;
	}
	return Buff;
}

f_mtdt* backspace(f_mtdt* Buff)
{
	const bool next = 1;

	if(ACT_LINE_LEN_I > 0)
	{
		// If the cursor at the maximum left position, char won't be deleted.
		if(Buff->cusr_x != ACT_LINE_LEN_I)
		{
			Buff = shift_text_horizonally(Buff, 'l');
			ACT_LINE = shrink_act_line_mem(Buff);

			ACT_LINE_LEN_I--;
			Buff->chars_i--;
		}
		// Deletes the non-empty line and copy chars to previous.
		else if(ACT_LINE_I > 0)
		{
			PREV_LINE_LEN_I--;
			Buff->chars_i--;

			for(buff_t char_i = 0; char_i <= ACT_LINE_LEN_I; char_i++)
			{
				PREV_LINE[PREV_LINE_LEN_I] = ACT_LINE[char_i];

				if(ACT_LINE[char_i] != NUL__CTRL_SHIFT_2)
				{
					PREV_LINE_LEN_I++;
				}
				PREV_LINE = extend_line_mem(Buff, PREV_LINE_I);
			}

			// Shift lines vertically.
			if(Buff->cusr_y > 0)
			{
				Buff = copy_lines_backward(Buff);
			}
			Buff = delete_last_line(Buff);
		}
	}
	// Deletes the last empty line.
	else if((ACT_LINE_LEN_I == 0) && (ACT_LINE_I > 0) && (Buff->cusr_y == 0))
	{
		safer_free(ACT_LINE);

		Buff->lines_i--;
		ACT_LINE = shrink_act_line_mem(Buff);

		ACT_LINE_LEN_I--;
		Buff->chars_i--;

		Buff = shrink_lines_array_mem(Buff);
	}
	else if((Buff->cusr_x == ACT_LINE_LEN_I) && (Buff->cusr_y > 0))
	{
		for(buff_t char_i = 0; char_i <= ACT_LINE_LEN_I; char_i++)
		{
			PREV_LINE[PREV_LINE_LEN_I] = PREV_LINE[char_i];
			PREV_LINE_LEN_I++;
			PREV_LINE = extend_line_mem(Buff, PREV_LINE_I);

			ACT_LINE_LEN_I--;
		}

		for(buff_t line_i = ACT_LINE_I + next; line_i < Buff->lines_i; line_i++)
		{
			Buff->text[line_i] = Buff->text[line_i + next];
			Buff->line_len_i[line_i] = Buff->line_len_i[line_i + next];
		}
		safer_free(LAST_LINE);

		Buff->lines_i--;
		Buff->chars_i--;
	}
	// Replaces the linefeed with the terminator.
	ACT_LINE[ACT_LINE_LEN_I] = NUL__CTRL_SHIFT_2;
	SET_STATUS("edited\0");

	return Buff;
}
