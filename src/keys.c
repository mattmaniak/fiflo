#include "fiflo.h"
#include "keys.h"

meta* add_chr_as_txt(char key, meta* Dt)
{
	if(Dt->chrs <= MAX_CHRS)
	{
		Dt->chrs++;
		CURR_LN_LEN++;

		Dt = add_mem_for_chrs(Dt);
		/* If the cursor is moved to the left and a char is inserted, rest of
		the text will be shifted to the right side. */
		if(Dt->cusr_x > 0)
		{
			Dt = shift_txt_horizonally('r', Dt);
		}
		CURR_LN[CURR_LN_LEN - Dt->cusr_x - NTERM_SZ] = key;
		CURR_LN[CURR_LN_LEN] = NTERM;

		// Fixes incremented value by the "pressed" initializer in "run".
		if(key == NTERM && CURR_LN_LEN > 0)
		{
			Dt->chrs--;
			CURR_LN_LEN--;
		}
	}
	return Dt;
}

meta* linefeed(meta* Dt)
{
	if(Dt->lns < MAX_LNS)
	{
		Dt->lns++;
		Dt = alloc_mem_for_lns(Dt);

		// Naturally the new line doesn't contains any chars - only terminator.
		CURR_LN[CURR_LN_LEN = 0] = NTERM;

		/* If user moved the cursor before hitting ENTER, txt on the right will
		be moved to the new line. */
		if(Dt->cusr_x > 0)
		{
			for(term_t x = PRE_CURR_LN_LEN - Dt->cusr_x; x < PRE_CURR_LN_LEN; x++)
			{
				CURR_LN[CURR_LN_LEN] = PRE_CURR_LN[x];
				CURR_LN_LEN++;
				Dt = add_mem_for_chrs(Dt);
			}
			// Now the length of the upper line will be shortened after copying.
			PRE_CURR_LN_LEN -= Dt->cusr_x;
			PRE_CURR_LN[PRE_CURR_LN_LEN] = NTERM;

			if(PRE_CURR_LN_LEN < 2)
			{
				PRE_CURR_LN = realloc(PRE_CURR_LN, 1 + NTERM_SZ);
//				puts("FREE UPPER: 2");
			}
			else if(PRE_CURR_LN_LEN >= 2 && PRE_CURR_LN_LEN < MEMBLK)
			{
				PRE_CURR_LN = realloc(PRE_CURR_LN, MEMBLK);
//				puts("FREE UPPER: 8");
			}
			else if(PRE_CURR_LN_LEN >= MEMBLK)
			{
				PRE_CURR_LN = realloc(PRE_CURR_LN, ((PRE_CURR_LN_LEN / MEMBLK) * MEMBLK)  + MEMBLK);
//				printf("FREE UPPER %d\n", ((PRE_CURR_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
			}
			chk_ptr(PRE_CURR_LN, "shrink the upper line's memory space\0", Dt);

			CURR_LN[CURR_LN_LEN] = NTERM;
		}
	}
	return Dt;
}

meta* backspace(meta* Dt)
{
	if(Dt->chrs > 0)
	{
		// Prevent first char deletion in line.
		if(Dt->cusr_x != CURR_LN_LEN)
		{
			Dt = shift_txt_horizonally('l', Dt);
		}
		Dt = free_mem_for_chrs(Dt);

		if(Dt->cusr_x != (CURR_LN_LEN + INDEX))
		{
			CURR_LN_LEN--;
			Dt->chrs--;
		}
		CURR_LN[CURR_LN_LEN] = NTERM;
	}
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
	if(Dt->cusr_x < CURR_LN_LEN)
	{
		// Move the cursor left.
		Dt->cusr_x++;
	}
	return Dt;
}

