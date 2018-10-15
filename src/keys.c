#include "fiflo.h"
#include "keys.h"

meta* ctrlh(meta* Dt)
{
	// Cursor can be moved right if is shifted left. 0 - default right position.
	if(Dt->cusr_x > 0)
	{
		// Move the cursor right.
		Dt->cusr_x--;
	}
	return Dt;
}

meta* ctrlg(meta* Dt)
{
	// Move only when the cursor isn't at the start of the line.
	if(Dt->cusr_x < LAST_LN_LEN)
	{
		// Move the cursor left.
		Dt->cusr_x++;
	}
	return Dt;
}

meta* ctrly(meta* Dt)
{
	if(Dt->cusr_y < Dt->lns)
	{
		// Move the cursor up.
		Dt->cusr_y++;

		// Must contain at least newline.
		if(PRE_LAST_LN_LEN == 1)
		{
			Dt->chrs = 1;
		}
		else
		{
			LAST_LN_LEN = (buf_t) (strlen(PRE_LAST_LN) - INDEX);
		}
	}
	return Dt;
}

meta* ctrlb(meta* Dt)
{
	if(Dt->cusr_y > 0)
	{
		// Move the cursor down.
		Dt->cusr_y--;
	}
	return Dt;
}

meta* addchar(char key, meta* Dt)
{
	if(Dt->chrs <= MAX_CHRS)
	{
		Dt->chrs++;
		CURR_LN_LEN++;

		// Fixes incremented value by terminator. TODO
		if(key == NTERM && Dt->chrs == 1)
		{
			Dt->chrs--;
			CURR_LN_LEN--;
		}
		// Eg. allocation for memblk = 4: ++------, ++++----, ++++++++.
		if(CURR_LN_LEN == 2)
		{
			// If there are 2 chars + terminator, extend to MEMBLK.
			CURR_LN = realloc(CURR_LN, MEMBLK);
		}
		else if(CURR_LN_LEN > 2 && CURR_LN_LEN % MEMBLK == 0)
		{
			CURR_LN = realloc(CURR_LN, CURR_LN_LEN + MEMBLK);
		}
		ptrcheck(CURR_LN, "extend memblock for the current line\0", Dt);

		/* If the cursor is moved to the left and a char is inserted, rest of
		the text will be shifted to the right side. */
		if(Dt->cusr_x > 0)
		{
			txtshift('r', Dt);
		}
		CURR_LN[CURR_LN_LEN - Dt->cusr_x - NTERM_SZ] = key;
		CURR_LN[CURR_LN_LEN] = NTERM;
	}
	return Dt;
}

meta* linefeed(meta* Dt)
{
	Dt->lns++;
	LAST_LN_LEN = 0;
	if(Dt->lns >= MAX_LNS)
	{
		// TODO: COMMENTS.
		Dt->lns = MAX_LNS;
	}
	else
	{	// TODO: WERID ALLOC.
		if(Dt->lns % MEMBLK == 0) // Allocates with a one line reserve.
		{
			Dt->txt = realloc(Dt->txt, sizeof(Dt->txt) * (Dt->lns + MEMBLK));
			ptrcheck(Dt->txt, "realloc array with lines\0", Dt);

			Dt->ln_len = realloc(Dt->ln_len, Dt->lns + INDEX + MEMBLK);
			ptrcheck(Dt->ln_len, "realloc array with lines length\0", Dt);

			for(buf_t lns = 0; lns < MEMBLK; lns++)
			{
				Dt->txt[lns] = realloc(Dt->txt[lns], 1 + NTERM_SZ);
				puts("MALLOC");
			}
			puts("ENTER ALLOC");
		}
		LAST_LN = malloc(1 + NTERM_SZ);
		ptrcheck(LAST_LN, "2 bytes for initial line\0", Dt);
	}
	LAST_LN[LAST_LN_LEN = 0] = NTERM;

	if(Dt->cusr_x > 0)
	{
		for(term_t x = PRE_LAST_LN_LEN - Dt->cusr_x; x <= PRE_LAST_LN_LEN; x++)
		{
			LAST_LN[LAST_LN_LEN] = PRE_LAST_LN[x];
			LAST_LN_LEN++;
		}
		PRE_LAST_LN[PRE_LAST_LN_LEN - Dt->cusr_x] = NTERM;
		LAST_LN[LAST_LN_LEN - 1] = NTERM;

		// Padding to the null terminator.
		LAST_LN_LEN--;
	}
	return Dt;
}

meta* backspace(meta* Dt)
{
	if(Dt->chrs > 0)
	{
		// Prevent first char deletion in line.
		if(Dt->cusr_x != LAST_LN_LEN)
		{
			Dt = txtshift('l', Dt);
		}

		if(CURR_LN_LEN == 2)
		{
			// If there are: char + terminator, shrink to 2 bytes.
			CURR_LN = realloc(CURR_LN, 1 + NTERM_SZ);
		}
		else if(CURR_LN_LEN == 8)
		{
			CURR_LN = realloc(CURR_LN, MEMBLK);
		}
		else if(CURR_LN_LEN > 8 && CURR_LN_LEN % MEMBLK == 0)
		{
			// There is more memory needed.
			CURR_LN = realloc(CURR_LN, CURR_LN_LEN);
		}
		ptrcheck(CURR_LN, "shrink memblock for the current line\0", Dt);



		if(Dt->cusr_x != (LAST_LN_LEN + INDEX))
		{
			LAST_LN_LEN--;
			Dt->chrs--;
		}
		else if(LAST_LN_LEN == 0)
		{
	//		line_back = 1;
			Dt->cusr_y = 0;
		}
		LAST_LN[LAST_LN_LEN] = NTERM;
	/*
		if(line_back == 1 && Dt->lns > 0 && PRE_LAST_LN[strlen(PRE_LAST_LN) - NTERM_SZ] == LF)
		{
			free(LAST_LN);
			LAST_LN = NULL;
			Dt->lns--;
			LAST_LN_LEN = (buf_t) strlen(LAST_LN) - NTERM_SZ;
			LAST_LN[LAST_LN_LEN] = NTERM;
			if(Dt->chrs > 0) // Just for the LF removal.
			{
				Dt->chrs--;
			}
		}*/




/*		else
		{
			printf("%d ", PRE_LAST_LN_LEN);

			for(term_t x = 0; x <= LAST_LN_LEN; x++)
			{
				PRE_LAST_LN[PRE_LAST_LN_LEN] = LAST_LN[x];
				PRE_LAST_LN_LEN++;
			}
			PRE_LAST_LN[PRE_LAST_LN_LEN] = NTERM;

			printf("%d\n", PRE_LAST_LN_LEN);

		//		Dt->chrs--;

			free(LAST_LN);
			LAST_LN = NULL;
			Dt->lns--;
		}
	}
*/
	}
	return Dt;
}

