#include "fiflo.h"
#include "keys.h"

meta* recognize_key(char key, meta* Dt)
{
	switch(key)
	{
		case NEG:
			fputs("Pipe isn't supported by the fiflo, exit(1).\n", stderr);
			free_all_exit(1, Dt);

		default:
			Dt = text_char(key, Dt);
			break;

		case HT:
			// Currently converts the tab to one space.
			for(uint8_t tab_width = 0; tab_width < 2; tab_width++)
			{
				Dt = text_char(' ', Dt);
			}
			break;

		case DEL:
			Dt = backspace(Dt);
			break;

		case CTRL_X:
			free_all_exit(0, Dt);

		case CTRL_D:
			save_file(Dt);
			break;

		case CTRL_G:
			Dt = ctrl_g(Dt);
			break;

		case CTRL_H:
			Dt->cusr_x = ctrl_h(Dt->cusr_x);
	}
	printf("\rlast: %d cusr_x: %d\n", key, Dt->cusr_x); // DEBUG
	return Dt;
}

meta* text_char(char key, meta* Dt)
{
	/* Only printable chars will be added. Combinations that aren't specified
	above will be omited. Set "key != ESC" to enable. */
	if(key == NUL || key == LF || key >= 32)
	{
		if(Dt->chars < BUF_MAX)
		{
			Dt->chars++;
			ACT_LN_LEN++;

			Dt = extend_act_line_mem(Dt);

			/* If the cursor is moved to the left and a char is inserted, rest
			of the text will be shifted to the right side. */
			if(Dt->cusr_x > 0)
			{
				Dt = shift_text_horizonally('r', Dt);
			}
			ACT_LN[ACT_LN_LEN - Dt->cusr_x - NUL_SZ] = key;
			ACT_LN[ACT_LN_LEN] = NUL;

			// Initializer handling.
			if(key == NUL && ACT_LN_LEN > 0)
			{
				Dt->chars--;
				ACT_LN_LEN--;
			}
			else if(key == LF)
			{
				Dt = linefeed(Dt);
			}
			if(Dt->chars > 0)
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
		SET_STATUS("opened the binary file\0");
	}
	return Dt;
}

meta* linefeed(meta* Dt)
{
	if(Dt->lines < BUF_MAX)
	{
		Dt->lines++;
		Dt = extend_lines_array(Dt);

		// Naturally the new line doesn't contains any chars - only terminator.
		ACT_LN_LEN = 0;

		/* If user moved the cursor before hitting ENTER, text on the right
		will be moved to the new line. */
		if(Dt->cusr_x > 0)
		{
			for(buf_t x = PREV_LN_LEN - Dt->cusr_x; x < PREV_LN_LEN; x++)
			{
				ACT_LN[ACT_LN_LEN] = PREV_LN[x];
				ACT_LN_LEN++;
				Dt = extend_act_line_mem(Dt);
			}

			// Now the length of the upper line will be shortened after copying.
			PREV_LN_LEN -= Dt->cusr_x;
			PREV_LN[PREV_LN_LEN] = NUL;

			Dt = shrink_prev_line_mem(Dt);
		}
		ACT_LN[ACT_LN_LEN] = NUL;
	}
	return Dt;
}

meta* backspace(meta* Dt)
{
	// Isn't possible to delete nothing.
	if(ACT_LN_LEN > 0)
	{
		// If the cursor at the maximum left position, char won't be deleted.
		if(Dt->cusr_x != ACT_LN_LEN)
		{
			Dt = shift_text_horizonally('l', Dt);
			Dt = shrink_act_line_mem(Dt);

			ACT_LN_LEN--;
			Dt->chars--;
		}
		else if(Dt->lines > 0)
		{
			PREV_LN_LEN--;
			Dt->chars--;

			for(buf_t x = 0; x <= ACT_LN_LEN; x++)
			{
				PREV_LN[PREV_LN_LEN] = ACT_LN[x];

				if(ACT_LN[x] != NUL)
				{
					PREV_LN_LEN++;
				}
				Dt = extend_prev_line_mem(Dt);
			}
			free(ACT_LN);
			ACT_LN = NULL;

			Dt->lines--;
			Dt = shrink_lines_array(Dt);
		}
	}
	// Delete the last line.
	else if(ACT_LN_LEN == 0 && Dt->lines > 0)
	{
		free(ACT_LN);
		ACT_LN = NULL;
		Dt->lines--;

		Dt = shrink_act_line_mem(Dt);

		ACT_LN_LEN--;
		Dt->chars--;

		Dt = shrink_lines_array(Dt);
	}
	// Replaces the LF with the terminator.
	ACT_LN[ACT_LN_LEN] = NUL;

	return Dt;
}

meta* ctrl_g(meta* Dt)
{
	// Move only when the cursor isn't at the start of the line.
	if(Dt->cusr_x < ACT_LN_LEN)
	{
		// Move the cursor left.
		Dt->cusr_x++;
	}
	return Dt;
}

buf_t ctrl_h(buf_t cusr_x)
{
	// Cursor can be moved right if is shifted left. 0 - default right position.
	if(cusr_x > 0)
	{
		// Move the cursor right.
		cusr_x--;
	}
	return cusr_x;
}

