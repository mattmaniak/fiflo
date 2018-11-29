#include "fiflo.h"
#include "text_processing.h"

f_mtdt* recognize_key(f_mtdt* Buff, char key)
{
	switch(key)
	{
		case NEG:
		{
			fputs("Pipe isn't supported, exit(1).\n", stderr);
			free_all_exit(Buff, 1);
		}
		default:
		{
			Buff = text_char(Buff, key);
			break;
		}
		case ESC__CTRL_LEFT_SQR_BRACKET:
		{
			Buff = ansi_escape_code_from_keyboard(Buff, key);
			break;
		}
		case HT__CTRL_I:
		{
			// Currently converts the tab to two spaces.
			for(uint8_t tab_width = 0; tab_width < 2; tab_width++)
			{
				Buff = text_char(Buff, ' ');
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
			free_all_exit(Buff, 1);
		}
		case DC3__CTRL_S:
		{
			Buff = save_file(Buff);
			break;
		}
		case BEL__CTRL_G:
		{
			Buff = cursor_left(Buff);
			break;
		}
		case BS__CTRL_H:
		{
			Buff = cursor_right(Buff);
			break;
		}
		case EM__CTRL_Y:
		{
			Buff = cursor_up(Buff);
			break;
		}
		case SOT__CTRL_B:
		{
			Buff = cursor_down(Buff);
			break;
		}
		case EOT__CTRL_D:
		{
			Buff = delete_line(Buff);
		}
	}

#ifdef DEBUG
	printf("Last key: %d, cusr_x: %d, cusr_y: %d.\n",
	key, Buff->cusr_x, Buff->cusr_y);
#endif

	return Buff;
}

f_mtdt* text_char(f_mtdt* Buff, char key)
{
	/* Only printable chars will be added. Combinations that aren't specified
	above will be omited. Set "if(key)" to enable them. */
	if((key == NUL__CTRL_SHIFT_2) || (key == LF__CTRL_J) || (key >= 32))
	{
		if(Buff->chars_i < BUFF_MAX)
		{
			Buff->chars_i++;
			ACT_LINE_LEN_I++;

			ACT_LINE = extend_line(Buff, ACT_LINE_I);

			/* If the cursor is moved to the left and a char is inserted, rest
			of the text will be shifted to the right side. */
			if(Buff->cusr_x > 0)
			{
				Buff = shift_text_horizonally(Buff, 'r');
			}
			ACT_LINE[ACT_LINE_LEN_I - Buff->cusr_x - NUL_SZ] = key;
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
		Buff = extend_lines_array(Buff);

		/* If user moved the cursor before hitting ENTER, text on the right
		will be moved to the new line. */
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
				ACT_LINE = extend_line(Buff, ACT_LINE_I);
			}

			// Now the length of the upper line will be shortened after copying.
			PREV_LINE[PREV_LINE_LEN_I] = NUL__CTRL_SHIFT_2;
			PREV_LINE = shrink_prev_line(Buff);
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
	if(ACT_LINE_LEN_I > 0)
	{
		// If the cursor at the maximum left position, char won't be deleted.
		if(Buff->cusr_x != ACT_LINE_LEN_I)
		{
			Buff = shift_text_horizonally(Buff, 'l');
			ACT_LINE = shrink_act_line(Buff);

			ACT_LINE_LEN_I--;
			Buff->chars_i--;
		}
		// Deletes the non-empty line and copy chars to previous.
		else if(ACT_LINE_I > 0)
		{
			PREV_LINE_LEN_I--;
			Buff->chars_i--;

			for(buff_t x = 0; x <= ACT_LINE_LEN_I; x++)
			{
				PREV_LINE[PREV_LINE_LEN_I] = ACT_LINE[x];

				if(ACT_LINE[x] != NUL__CTRL_SHIFT_2)
				{
					PREV_LINE_LEN_I++;
				}
				PREV_LINE = extend_line(Buff, PREV_LINE_I);
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
	else if((ACT_LINE_LEN_I == 0) && (ACT_LINE_I > 0))
	{
		if(Buff->cusr_y == 0)
		{
			free(ACT_LINE);
			ACT_LINE = NULL;

			Buff->lines_i--;
			ACT_LINE = shrink_act_line(Buff);

			ACT_LINE_LEN_I--;
			Buff->chars_i--;

			Buff = shrink_lines_array(Buff);
		}
	}
	else if((Buff->cusr_x == ACT_LINE_LEN_I) && (Buff->cusr_y > 0))
	{
		for(buff_t char_i = 0; char_i <= ACT_LINE_LEN_I; char_i++)
		{
			PREV_LINE[PREV_LINE_LEN_I] = PREV_LINE[char_i];
			PREV_LINE_LEN_I++;
			PREV_LINE = extend_line(Buff, PREV_LINE_I);

			ACT_LINE_LEN_I--;
		}

		for(buff_t line_i = ACT_LINE_I + INDEX; line_i < Buff->lines_i; line_i++)
		{
			Buff->text[line_i] = Buff->text[line_i + 1];
			Buff->line_len_i[line_i] = Buff->line_len_i[line_i + 1];
		}
		free(LAST_LINE);
		LAST_LINE = NULL;

		Buff->lines_i--;
		Buff->chars_i--;
	}
	// Replaces the linefeed with the terminator.
	ACT_LINE[ACT_LINE_LEN_I] = NUL__CTRL_SHIFT_2;
	SET_STATUS("edited\0");

	return Buff;
}

f_mtdt* delete_last_line(f_mtdt* Buff)
{
	free(LAST_LINE);
	LAST_LINE = NULL;

	Buff->lines_i--;
	Buff = shrink_lines_array(Buff);

	return Buff;
}

f_mtdt* delete_line(f_mtdt* Buff)
{
	if(Buff->lines_i > 0)
	{
		if(Buff->cusr_y == 0)
		{
			Buff = delete_last_line(Buff);

			/* With the last line deletion there is a need to remove the
			linefeed in the previous line. */
			LAST_LINE_LEN_I--;
			LAST_LINE[LAST_LINE_LEN_I] = NUL__CTRL_SHIFT_2;

			Buff->cusr_x = 0;
		}
		else
		{
			Buff = copy_lines_backward(Buff);
			Buff = delete_last_line(Buff);

			Buff->cusr_x = 1;
			Buff->cusr_y--;
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
	const _Bool prev = 1;

	switch(direction)
	{
		case 'l':
		{
			for(buff_t char_i = ACT_LINE_LEN_I - Buff->cusr_x;
			char_i <= ACT_LINE_LEN_I; char_i++)
			{
				ACT_LINE[char_i - prev] = ACT_LINE[char_i];
			}
			break;
		}
		case 'r':
		{
			for(buff_t char_i = ACT_LINE_LEN_I;
			char_i >= ACT_LINE_LEN_I - Buff->cusr_x; char_i--)
			{
				ACT_LINE[char_i] = ACT_LINE[char_i - prev];
			}
		}
	}
	return Buff;
}
