#include "fiflo.h"
#include "cursor.h"

f_mtdt* cursor_left(f_mtdt* Buff)
{
	if(Buff->cusr_x < ACT_LINE_LEN_I)
	{
		Buff->cusr_x++;
	}
	else if((Buff->lines_i > 0) && (Buff->cusr_y < Buff->lines_i))
	{
		// Set to the right ignoring the linefeed.
		Buff->cusr_x = 1;
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* cursor_right(f_mtdt* Buff)
{
	if(Buff->cusr_x > 0)
	{
		Buff->cusr_x--;
		if((Buff->cusr_y > 0) && (Buff->cusr_x == 0))
		{
			Buff->cusr_x = ACT_LINE_LEN_I;
			Buff->cusr_y--;
		}
		// Last line doesn't contain linefeed so ignoring that isn't necessary.
		else if((Buff->cusr_y == 1) && (Buff->cusr_x == 0))
		{
			Buff->cusr_y--;
		}
	}
	return Buff;
}

f_mtdt* cursor_up(f_mtdt* Buff)
{
	if(Buff->cusr_y < Buff->lines_i)
	{
		// Ignore the linefeed.
		Buff->cusr_x = 1;
		Buff->cusr_y++;
	}
	return Buff;
}

f_mtdt* cursor_down(f_mtdt* Buff)
{
	if(Buff->cusr_y > 0)
	{
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

f_mtdt* ansi_escape_code_from_keyboard(f_mtdt* Buff, char key)
{
	const _Bool prev = 1;

	if(ACT_LINE[ACT_LINE_LEN_I - NUL_SZ - prev] == '[')
	{
		puts("ANSI");
	}
	return Buff;
}
