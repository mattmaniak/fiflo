#include "fiflo.h"
#include "text.h"

f_mtdt* recognize_key(char key, f_mtdt* Buff)
{
	switch(key)
	{
		case NEG:
			fputs("Pipe isn't supported, exit(1).\n", stderr);
			free_all_exit(1, Buff);

		default:
			Buff = text_char(key, Buff);
			break;

		case HT__CTRL_I:
			// Currently converts the tab to two spaces.
			for(uint8_t tab_width = 0; tab_width < 2; tab_width++)
			{
				Buff = text_char(' ', Buff);
			}
			break;

		case DEL__BACKSPACE:
			Buff = backspace(Buff);
			break;

		case CAN__CTRL_X:
			free_all_exit(0, Buff);

		case ETB__CTRL_W:
			save_file(Buff);
			break;

		case BEL__CTRL_G:
			Buff = ctrl_g(Buff);
			break;

		case BS__CTRL_H:
			Buff->cusr_x = ctrl_h(Buff->cusr_x);
	}
	return Buff;
}

f_mtdt* text_char(char key, f_mtdt* Buff)
{
	/* Only printable chars will be added. Combinations that aren't specified
	above will be omited. Set "if(key)" to enable them. */
	if(key == NUL__CTRL_SHIFT_2 || key == LF__CTRL_J || key >= 32)
	{
		if(Buff->chars < BUF_MAX)
		{
			Buff->chars++;
			ACT_LN_LEN++;

			Buff = extend_act_line(Buff);

			/* If the cursor is moved to the left and a char is inserted, rest
			of the text will be shifted to the right side. */
			if(Buff->cusr_x > 0)
			{
				Buff = shift_text_horizonally('r', Buff);
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
	if(Buff->lines < BUF_MAX)
	{
		Buff->lines++;
		Buff = extend_lines_array(Buff);

		// Naturally the new line doesn't contains any chars - only terminator.
		ACT_LN_LEN = 0;

		/* If user moved the cursor before hitting ENTER, text on the right
		will be moved to the new line. */
		if(Buff->cusr_x > 0)
		{
			for(buff_t x = PREV_LN_LEN - Buff->cusr_x; x < PREV_LN_LEN; x++)
			{
				ACT_LN[ACT_LN_LEN] = PREV_LN[x];
				ACT_LN_LEN++;
				Buff = extend_act_line(Buff);
			}

			// Now the length of the upper line will be shortened after copying.
			PREV_LN_LEN -= Buff->cusr_x;
			PREV_LN[PREV_LN_LEN] = NUL__CTRL_SHIFT_2;

			Buff = shrink_prev_line(Buff);
		}
		ACT_LN[ACT_LN_LEN] = NUL__CTRL_SHIFT_2;
	}
	return Buff;
}

f_mtdt* backspace(f_mtdt* Buff)
{
	// Isn't possible to delete nothing.
	if(ACT_LN_LEN > 0)
	{
		// If the cursor at the maximum left position, char won't be deleted.
		if(Buff->cusr_x != ACT_LN_LEN)
		{
			Buff = shift_text_horizonally('l', Buff);
			Buff = shrink_act_line(Buff);

			ACT_LN_LEN--;
			Buff->chars--;
		}
		// Deletes the non-empty line and copy chars to previous..
		else if(Buff->lines > 0)
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
				Buff = extend_prev_line(Buff);
			}
			free(ACT_LN);
			ACT_LN = NULL;

			Buff->lines--;
			Buff = shrink_lines_array(Buff);
		}
	}
	// Deletes the last empty line.
	else if(ACT_LN_LEN == 0 && Buff->lines > 0)
	{
		free(ACT_LN);
		ACT_LN = NULL;
		Buff->lines--;

		Buff = shrink_act_line(Buff);

		ACT_LN_LEN--;
		Buff->chars--;

		Buff = shrink_lines_array(Buff);
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
	return Buff;
}

buff_t ctrl_h(buff_t cusr_x)
{
	// Cursor can be moved right if is shifted left. 0 - default right position.
	if(cusr_x > 0)
	{
		// Move the cursor right.
		cusr_x--;
	}
	return cusr_x;
}

f_mtdt* shift_text_horizonally(char direction, f_mtdt* Buff)
{
	switch(direction)
	{
		case 'l':
			if(Buff->cusr_x > ACT_LN_LEN - NUL_SZ && ACT_LN_LEN > 0)
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

		case 'r':
			for(buff_t x = ACT_LN_LEN; x >= ACT_LN_LEN - Buff->cusr_x; x--)
			{
				ACT_LN[x] = ACT_LN[x - INDEX];
			}
	}
	return Buff;
}

