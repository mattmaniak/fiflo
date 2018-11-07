#include "fiflo.h"
#include "memory.h"

meta* extend_act_line(meta* Dt)
{
	if(ACT_LN_LEN == INIT_MEMBLK)
	{
		// Chars index == INIT_MEMBLK, extend to MEMBLK.
		ACT_LN = realloc(ACT_LN, MEMBLK);
	}
	else if(ACT_LN_LEN > INIT_MEMBLK && ACT_LN_LEN % MEMBLK == 0)
	{
		// If simply there is even more chars, append the new memblock.
		ACT_LN = realloc(ACT_LN, ((ACT_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(ACT_LN, "extend the memblock for the current line\0", Dt);

	return Dt;
}

meta* extend_prev_line(meta* Dt)
{
	if(PREV_LN_LEN == INIT_MEMBLK)
	{
		// If there are 4/8 chars, extend to MEMBLK.
		PREV_LN = realloc(PREV_LN, MEMBLK);
	}
	else if(PREV_LN_LEN > INIT_MEMBLK && PREV_LN_LEN % MEMBLK == 0)
	{
		PREV_LN = realloc(PREV_LN, ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "extend the memblock for the previous line\0", Dt);

	return Dt;
}

meta* shrink_act_line(meta* Dt)
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
	else if(ACT_LN_LEN > MEMBLK && ACT_LN_LEN % MEMBLK == 0)
	{
		// Remove the newest memblock because isn't needed now.
		ACT_LN = realloc(ACT_LN, (ACT_LN_LEN / MEMBLK) * MEMBLK);
	}
	chk_ptr(ACT_LN, "shrink the memblock with the current line\0", Dt);

	return Dt;
}

meta* shrink_prev_line(meta* Dt)
{
	if(PREV_LN_LEN < INIT_MEMBLK)
	{
		// Set the previus' line memblock to initializing memblock.
		PREV_LN = realloc(PREV_LN, INIT_MEMBLK);
	}
	else if(PREV_LN_LEN >= INIT_MEMBLK && PREV_LN_LEN < MEMBLK)
	{
		// Set to the full memory block.
		PREV_LN = realloc(PREV_LN, MEMBLK);
	}
	else if(PREV_LN_LEN >= MEMBLK)
	{
		// Set the size of some MEMBLKs.
		PREV_LN = realloc(PREV_LN, ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "shrink the memblock with the previous line\0", Dt);

	return Dt;
}

meta* extend_lines_array(meta* Dt)
{
	// Enhance the array that contains pointers to lines.
	Dt->text = realloc(Dt->text, (Dt->lines + INDEX) * sizeof(Dt->text));
	chk_ptr(Dt->text, "extend the array with lines\0", Dt);

	// Enhance the array that contains lines length indicators.
	Dt->line_len = realloc(Dt->line_len, (Dt->lines + INDEX) * sizeof(buf_t));
	chk_ptr(Dt->line_len, "extend the array with lines length\0", Dt);

	// The new line is allocated with only 4 or 8 bytes bytes.
	ACT_LN = malloc(INIT_MEMBLK);
	chk_ptr(ACT_LN, "alloc the initial block for the created line\0", Dt);

	return Dt;
}

meta* shrink_lines_array(meta* Dt)
{
	Dt->text = realloc(Dt->text, (Dt->lines + INDEX) * sizeof(Dt->text));
	chk_ptr(Dt->text, "shrink the array with lines\0", Dt);

	Dt->line_len = realloc(Dt->line_len, (Dt->lines + INDEX) * sizeof(buf_t));
	chk_ptr(Dt->line_len, "shrink the array with lines length\0", Dt);

	return Dt;
}

