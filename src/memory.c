#include "fiflo.h"
#include "memory.h"

f_mtdt* extend_act_line(f_mtdt* Buff)
{
	if(ACT_LN_LEN == INIT_MEMBLK)
	{
		// Chars index == INIT_MEMBLK, extend to MEMBLK.
		ACT_LN = realloc(ACT_LN, MEMBLK);
	}
	else if((ACT_LN_LEN > INIT_MEMBLK) && (ACT_LN_LEN % MEMBLK == 0))
	{
		// If simply there is even more chars, append the new memblock.
		ACT_LN = realloc(ACT_LN, ((ACT_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(ACT_LN, "extend the memblock for the current line\0", Buff);

	return Buff;
}

f_mtdt* extend_prev_line(f_mtdt* Buff)
{
	if(PREV_LN_LEN == INIT_MEMBLK)
	{
		// If there are 4/8 chars, extend to MEMBLK.
		PREV_LN = realloc(PREV_LN, MEMBLK);
	}
	else if((PREV_LN_LEN > INIT_MEMBLK) && (PREV_LN_LEN % MEMBLK == 0))
	{
		PREV_LN = realloc(PREV_LN, ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "extend the memblock for the previous line\0", Buff);

	return Buff;
}

f_mtdt* shrink_act_line(f_mtdt* Buff)
{
	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_act_line". */
	if(ACT_LN_LEN == INIT_MEMBLK)
	{
		// Shrink to INIT_MEMBLOCK bytes.
		ACT_LN = realloc(ACT_LN, INIT_MEMBLK);
	}
	else if(ACT_LN_LEN == MEMBLK)
	{
		// Shrink to size of the MEMBLK.
		ACT_LN = realloc(ACT_LN, MEMBLK);
	}
	else if((ACT_LN_LEN > MEMBLK) && (ACT_LN_LEN % MEMBLK == 0))
	{
		// Remove the newest memblock because isn't needed now.
		ACT_LN = realloc(ACT_LN, (ACT_LN_LEN / MEMBLK) * MEMBLK);
	}
	chk_ptr(ACT_LN, "shrink the memblock with the current line\0", Buff);

	return Buff;
}

f_mtdt* shrink_prev_line(f_mtdt* Buff)
{
	if(PREV_LN_LEN < INIT_MEMBLK)
	{
		// Set the previus' line memblock to initializing memblock.
		PREV_LN = realloc(PREV_LN, INIT_MEMBLK);
	}
	else if((PREV_LN_LEN >= INIT_MEMBLK) && (PREV_LN_LEN < MEMBLK))
	{
		// Set to the full memory block.
		PREV_LN = realloc(PREV_LN, MEMBLK);
	}
	else if(PREV_LN_LEN >= MEMBLK)
	{
		// Set the size of some MEMBLKs.
		PREV_LN = realloc(PREV_LN, ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "shrink the memblock with the previous line\0", Buff);

	return Buff;
}

f_mtdt* extend_lines_array(f_mtdt* Buff)
{
	// Enhance the array that contains pointers to lines.
	Buff->text = realloc(Buff->text,
	(Buff->lines + INDEX) * sizeof(Buff->text));

	chk_ptr(Buff->text, "extend the array with lines\0", Buff);

	// Enhance the array that contains lines length indicators.
	Buff->line_len = realloc(Buff->line_len,
	(Buff->lines + INDEX) * sizeof(buff_t));

	chk_ptr(Buff->line_len, "extend the array with lines length\0", Buff);

	// The new line is allocated with only 4 or 8 bytes bytes.
	ACT_LN = malloc(INIT_MEMBLK);
	chk_ptr(ACT_LN, "malloc the new line\0", Buff);

	return Buff;
}

f_mtdt* shrink_lines_array(f_mtdt* Buff)
{
	Buff->text = realloc(Buff->text,
	(Buff->lines + INDEX) * sizeof(Buff->text));

	chk_ptr(Buff->text, "shrink the array with lines\0", Buff);

	Buff->line_len = realloc(Buff->line_len,
	(Buff->lines + INDEX) * sizeof(buff_t));

	chk_ptr(Buff->line_len, "shrink the array with lines length\0", Buff);

	return Buff;
}

