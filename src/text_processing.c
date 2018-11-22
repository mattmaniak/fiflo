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

		case CAN__CTRL_X:
		{
			free_all_exit(Buff, 1);
		}

		case ETB__CTRL_W:
		{
			Buff = save_file(Buff);
			break;
		}

		case BEL__CTRL_G:
		{
			Buff = ctrl_g(Buff);
			break;
		}

		case BS__CTRL_H:
		{
			Buff = ctrl_h(Buff);
			break;
		}

		case EM__CTRL_Y:
		{
			Buff = ctrl_y(Buff);
			break;
		}

		case SOT__CTRL_B:
		{
			Buff = ctrl_b(Buff);
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
		if(Buff->chars < BUFF_MAX)
		{
			Buff->chars++;
			ACT_LN_LEN++;

			ACT_LN = extend_line(Buff, ACT_LN_INDEX);

			/* If the cursor is moved to the left and a char is inserted, rest
			of the text will be shifted to the right side. */
			if(Buff->cusr_x > 0)
			{
				Buff = shift_text_horizonally(Buff, 'r');
			}
			ACT_LN[ACT_LN_LEN - Buff->cusr_x - NUL_SZ] = key;
			ACT_LN[ACT_LN_LEN] = NUL__CTRL_SHIFT_2;

			// Initializer handling.
			if(key == NUL__CTRL_SHIFT_2 && ACT_LN_LEN > 0)
			{
				Buff->chars--;
				ACT_LN_LEN--;
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
	if(Buff->lines < BUFF_MAX)
	{
		Buff->lines++;

		// Also mallocs the last line.
		Buff = extend_lines_array(Buff);

		/* If user moved the cursor before hitting ENTER, text on the right
		will be moved to the new line. */
		if(Buff->cusr_x > 0)
		{
			PREV_LN_LEN -= Buff->cusr_x;

			// Move more lines vertically with the part of the current line.
			if(Buff->cusr_y > 0)
			{
				for(buff_t y = Buff->lines; y > ACT_LN_INDEX; y--)
				{
					Buff->text[y] = realloc(Buff->text[y],
					((Buff->line_len[y - 1] / 16) * 16) + 16);

					chk_ptr(Buff, Buff->text[y], "move the line forward\0");

					Buff->text[y] = strcpy(Buff->text[y], Buff->text[y - 1]);
					Buff->line_len[y] = Buff->line_len[y - 1];
				}
				ACT_LN_LEN = 0;
			}
			for(buff_t x = PREV_LN_LEN; x < PREV_LN_LEN + Buff->cusr_x; x++)
			{
				ACT_LN[ACT_LN_LEN] = PREV_LN[x];
				ACT_LN_LEN++;
				ACT_LN = extend_line(Buff, ACT_LN_INDEX);
			}

			// Now the length of the upper line will be shortened after copying.
			PREV_LN[PREV_LN_LEN] = NUL__CTRL_SHIFT_2;

			PREV_LN = shrink_prev_line(Buff);
		}
		// Cursor is at the end of the line. Shifted vertically.
		else if(Buff->cusr_y > 0)
		{
			for(buff_t y = Buff->lines; y > ACT_LN_INDEX; y--)
			{
				Buff->text[y] = realloc(Buff->text[y],
				((Buff->line_len[y - 1] / 16) * 16) + 16);

				chk_ptr(Buff, Buff->text[y], "move the line forward\0");

				Buff->text[y] = strcpy(Buff->text[y], Buff->text[y - 1]);
				Buff->line_len[y] = Buff->line_len[y - 1];
			}
		}
		ACT_LN[ACT_LN_LEN] = NUL__CTRL_SHIFT_2;
	}
	return Buff;
}

// TODO
f_mtdt* backspace(f_mtdt* Buff)
{
	// Isn't possible to delete nothing.
	if(ACT_LN_LEN > 0)
	{
		// If the cursor at the maximum left position, char won't be deleted.
		if(Buff->cusr_x != ACT_LN_LEN)
		{
			Buff = shift_text_horizonally(Buff, 'l');
			ACT_LN = shrink_act_line(Buff);

			ACT_LN_LEN--;
			Buff->chars--;
		}
		// Deletes the non-empty line and copy chars to previous.
		else if(ACT_LN_INDEX > 0)
		{
			PREV_LN_LEN--;
			Buff->chars--;

			for(buff_t x = 0; x <= ACT_LN_LEN; x++)
			{
				PREV_LN[PREV_LN_LEN] = ACT_LN[x];

				if(ACT_LN[x] != NUL__CTRL_SHIFT_2)
				{
					PREV_LN_LEN++;
				}
				PREV_LN = extend_line(Buff, PREV_LN_INDEX);
			}
			// Shift lines vertically.
			if(Buff->cusr_y > 0)
			{
				for(buff_t y = ACT_LN_INDEX; y < Buff->lines; y++)
				{
					Buff->text[y] = realloc(Buff->text[y],
					((Buff->line_len[y + 1] * 16) / 16) + 16);

					Buff->text[y] = strcpy(Buff->text[y], Buff->text[y + 1]);
					Buff->line_len[y] = Buff->line_len[y + 1];
				}
			}
			free(LAST_LN);
			LAST_LN = NULL;

			Buff->lines--;
			Buff = shrink_lines_array(Buff);
		}
	}
	// Deletes the last empty line.
	else if((ACT_LN_LEN == 0) && (ACT_LN_INDEX > 0))
	{
		if(Buff->cusr_y == 0)
		{
			free(ACT_LN);
			ACT_LN = NULL;
			Buff->lines--;

			ACT_LN = shrink_act_line(Buff);

			ACT_LN_LEN--;
			Buff->chars--;

			Buff = shrink_lines_array(Buff);
		}
	}
	else if((Buff->cusr_x == ACT_LN_LEN) && (Buff->cusr_y > 0))
	{
		for(buff_t x = 0; x <= ACT_LN_LEN; x++)
		{
			PREV_LN[PREV_LN_LEN] = PREV_LN[x];
			PREV_LN_LEN++;
			ACT_LN_LEN--;
			PREV_LN = extend_line(Buff, PREV_LN_INDEX);
		}

		for(buff_t y = ACT_LN_INDEX + 1; y < Buff->lines; y++)
		{
			Buff->text[y] = Buff->text[y + 1];
			Buff->line_len[y] = Buff->line_len[y + 1];
		}
		free(LAST_LN);
		LAST_LN = NULL;
		Buff->lines--;
		Buff->chars--;
	}
	// Replaces the linefeed with the terminator.
	ACT_LN[ACT_LN_LEN] = NUL__CTRL_SHIFT_2;

	SET_STATUS("edited\0");
	return Buff;
}

f_mtdt* ctrl_g(f_mtdt* Buff)
{
	// Move only when the cursor isn't at the start of the line.
	if(Buff->cusr_x < ACT_LN_LEN)
	{
		// Move the cursor left.
		Buff->cusr_x++;
	}
	else if((Buff->lines > 0) && (Buff->cusr_y < Buff->lines))
	{
		// Set to the right ignoring the linefeed.
		Buff->cusr_x = 1;
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* ctrl_h(f_mtdt* Buff)
{
	// Cursor can be moved right if is shifted left. 0 - default right position.
	if(Buff->cusr_x > 0)
	{
		// Move the cursor right.
		Buff->cusr_x--;
		if((Buff->cusr_y > 0) && (Buff->cusr_x == 0))
		{
			Buff->cusr_y--;
			Buff->cusr_x = ACT_LN_LEN;
		}
		// Last line doesn't contain linefeed so ignoring that isn't necessary.
		else if((Buff->cusr_y == 1) && (Buff->cusr_x == 0))
		{
			Buff->cusr_y--;
		}
	}
	return Buff;
}

f_mtdt* ctrl_y(f_mtdt* Buff)
{
	if(Buff->cusr_y < Buff->lines)
	{
		// Move the cursor up.
		Buff->cusr_y++;

		// Ignore the linefeed.
		Buff->cusr_x = 1;
	}
	return Buff;
}

f_mtdt* ctrl_b(f_mtdt* Buff)
{
	if(Buff->cusr_y > 0)
	{
		// Move the cursor down.
		Buff->cusr_y--;

		if(Buff->cusr_y > 0)
		{
			Buff->cusr_x = 1;
		}
		else
		{
			Buff->cusr_x = 0;
		}
	}
	return Buff;
}

f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction)
{
	switch(direction)
	{
		case 'l':
		{
			if((Buff->cusr_x > (ACT_LN_LEN - NUL_SZ)) && (ACT_LN_LEN > 0))
			{
				Buff->cusr_x = ACT_LN_LEN - NUL_SZ;
			}
			if(ACT_LN_LEN > 0)
			{
				for(buff_t x = ACT_LN_LEN - Buff->cusr_x; x <= ACT_LN_LEN; x++)
				{
					ACT_LN[x - INDEX] = ACT_LN[x];
				}
			}
			break;
		}

		case 'r':
		{
			for(buff_t x = ACT_LN_LEN; x >= ACT_LN_LEN - Buff->cusr_x; x--)
			{
				ACT_LN[x] = ACT_LN[x - INDEX];
			}
		}
	}
	return Buff;
}
