#include "fiflo.h"
#include "keys.h"

meta* ctrlh(meta* Dat)
{
	if(Dat->cusr_x > 0)
	{
		// Move the cursor right.
		Dat->cusr_x--;
	}
	return Dat;
}

meta* ctrlg(meta* Dat)
{
	if(Dat->cusr_x < Dat->ln_len[Dat->lns])
	{
		// Move the cursor left.
		Dat->cusr_x++;
	}
	return Dat;
}

meta* ctrly(meta* Dat)
{
	if(Dat->cusr_y < Dat->lns)
	{
		// Move the cursor up.
		Dat->cusr_y++;
		// Must contain at least newline.
		if(strlen(LN_ABOVE) == 1)
		{
			Dat->chrs = 1;
		}
		else
		{
			Dat->ln_len[Dat->lns] = (buf_t) (strlen(LN_ABOVE) - INDEX);
		}
	}
	return Dat;
}

meta* ctrlb(meta* Dat)
{
	if(Dat->cusr_y > 0)
	{
		// Move the cursor down.
		Dat->cusr_y--;
	}
	return Dat;
}

meta* backspace(meta* Dat)
{
	// Prevent first char deletion in line.
	if((Dat->cusr_x != Dat->ln_len[Dat->lns]) || Dat->ln_len[Dat->lns] == 0)
	{
		Dat = txtshift(Dat, '<');
		Dat = freeblk(Dat);
	}
	return Dat;
}

