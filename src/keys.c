#include "keys.h"

meta* ctrl_h(meta* dt)
{
	if(dt->cusr_x > 0)
	{
		// Move the cursor right.
		dt->cusr_x--;
	}
	return dt;
}

meta* ctrl_g(meta* dt)
{
	if(dt->cusr_x < dt->ln_len)
	{
		// Move the cursor left.
		dt->cusr_x++;
	}
	return dt;
}

meta* ctrl_y(meta* dt)
{
	if(dt->cusr_y < dt->lns)
	{
		// Move the cursor up.
		dt->cusr_y++;
		// Must contain at least newline.
		if(strlen(LN_ABOVE) == 1)
		{
			dt->chrs = 1;
		}
		else
		{
			dt->ln_len = (buf_t) (strlen(LN_ABOVE) - INDEX);
		}
	}
	return dt;
}

meta* ctrl_b(meta* dt)
{
	if(dt->cusr_y > 0)
	{
		// Move the cursor down.
		dt->cusr_y--;
	}
	return dt;
}

meta* backspace(meta* dt)
{
	// Prevent first char deletion in line.
	if((dt->cusr_x != dt->ln_len) || dt->ln_len == 0)
	{
		dt = shift_txt(dt, 'l');
		dt = dealloc_block(dt);
	}
	return dt;
}

