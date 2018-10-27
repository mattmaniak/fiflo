#include "fiflo.h"
#include "keys.h"

meta* non_control_chr(char key, meta* Dt)
{
	if(Dt->chrs <= MAX_CHRS)
	{
		Dt->chrs++;
		ACT_LN_LEN++;

		Dt = extend_curr_ln_mem(Dt);

		/* If the cursor is moved to the left and a char is inserted, rest of
		the text will be shifted to the right side. */
		if(Dt->cusr_x > 0)
		{
			Dt = shift_txt_horizonally('r', Dt);
		}
		ACT_LN[ACT_LN_LEN - Dt->cusr_x - NTERM_SZ] = key;
		ACT_LN[ACT_LN_LEN] = NTERM;

		if(key == NTERM && ACT_LN_LEN > 0)
		{
			Dt->chrs--;
			ACT_LN_LEN--;
		}
	}
	return Dt;
}

meta* linefeed(meta* Dt)
{
	if(Dt->lns < MAX_LNS)
	{
		Dt->lns++;
		Dt = extend_lns_array(Dt);

		// Naturally the new line doesn't contains any chars - only terminator.
		ACT_LN_LEN = 0;

		/* If user moved the cursor before hitting ENTER, txt on the right will
		be moved to the new line. */
		if(Dt->cusr_x > 0)
		{
			for(term_t x = PREV_LN_LEN - Dt->cusr_x; x < PREV_LN_LEN; x++)
			{
				ACT_LN[ACT_LN_LEN] = PREV_LN[x];
				ACT_LN_LEN++;
				Dt = extend_curr_ln_mem(Dt);
			}

			// Now the length of the upper line will be shortened after copying.
			PREV_LN_LEN -= Dt->cusr_x;
			PREV_LN[PREV_LN_LEN] = NTERM;

			Dt = shrink_prev_ln_mem(Dt);
		}
		ACT_LN[ACT_LN_LEN] = NTERM;
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
			Dt = shift_txt_horizonally('l', Dt);
			Dt = shrink_curr_ln_mem(Dt);

			ACT_LN_LEN--;
			Dt->chrs--;
		}
		else if(Dt->lns > 0)
		{
			PREV_LN_LEN--;
			Dt->chrs--;

			for(buf_t x = 0; x <= ACT_LN_LEN; x++)
			{
				PREV_LN[PREV_LN_LEN] = ACT_LN[x];
				if(ACT_LN[x] != NTERM)
				{
					PREV_LN_LEN++;
				}

				if(PREV_LN_LEN == INIT_MEMBLK)
				{
					// If there are 4/8 chars + terminator, extend to MEMBLK.
					PREV_LN = realloc(PREV_LN, MEMBLK);
					puts("ALLOC_EIGHT");
				}
				else if(PREV_LN_LEN > INIT_MEMBLK && PREV_LN_LEN % MEMBLK == 0)
				{
					PREV_LN = realloc(PREV_LN,
					((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);

					printf("ALLOC: %d\n", ((PREV_LN_LEN / MEMBLK) * MEMBLK)
					+ MEMBLK);
				}
				chk_ptr(PREV_LN, "extend a memblock for the current line\0",
				Dt);
			}
			free(ACT_LN);
			ACT_LN = NULL;

			Dt->lns--;
			Dt = shrink_lns_array(Dt);
		}
	}
	// Delete the last line.
	else if(ACT_LN_LEN == 0 && Dt->lns > 0)
	{
		free(ACT_LN);
		ACT_LN = NULL;
		Dt->lns--;

		Dt = shrink_curr_ln_mem(Dt);

		ACT_LN_LEN--;
		Dt->chrs--;

		Dt = shrink_lns_array(Dt);
	}
	// Replaces the LF with the terminator.
	ACT_LN[ACT_LN_LEN] = NTERM;

	return Dt;
}

meta* ctrl_h(meta* Dt)
{
	// Cursor can be moved right if is shifted left. 0 - default right position.
	if(Dt->cusr_x > 0)
	{
		// Move the cursor right.
		Dt->cusr_x--;
	}
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

