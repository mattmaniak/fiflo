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
		if(strlen(PRE_LAST_LN) == 1)
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

meta* backspace(meta* Dt)
{
	// Prevent first char deletion in line.
	if((Dt->cusr_x != LAST_LN_LEN) || LAST_LN_LEN == 0)
	{
		Dt = txtshift(Dt, '<');
		Dt = freeblk(Dt);
	}
	return Dt;
}

meta* linefeed(meta* Dt)
{
	Dt = allocblk(Dt, 'l');
	if(Dt->cusr_x > 0)
	{
		for(term_t x = (term_t) (strlen(PRE_LAST_LN) - Dt->cusr_x);
		x <= strlen(PRE_LAST_LN); x++)
		{
			LAST_LN[LAST_LN_LEN] = PRE_LAST_LN[x];
			LAST_LN_LEN++;
		}
		PRE_LAST_LN[strlen(PRE_LAST_LN) - Dt->cusr_x] = NTERM;
		LAST_LN[LAST_LN_LEN - 1] = NTERM;

		// Padding to the null terminator.
		LAST_LN_LEN--;
		Dt->cusr_x = 0;
	}
	return Dt;
}

