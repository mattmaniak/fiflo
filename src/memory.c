#include "fiflo.h"
#include "memory.h"

char* extend_line(f_mtdt* Buff, buff_t line)
{
	if(Buff->line_len[line] == INIT_MEMBLK)
	{
		// There are 4/8 chars, extend to MEMBLK.
		Buff->text[line] = realloc(Buff->text[line], MEMBLK);
	}
	else if((Buff->line_len[line] > INIT_MEMBLK)
	&& (Buff->line_len[line] % MEMBLK == 0))
	{
		// There are more chars so append the new memblock.
		Buff->text[line] = realloc(Buff->text[line],
		((Buff->line_len[line] / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(Buff, Buff->text[line], "extend the memblock for the line\0");

	printf("extended line: %d with mem: %d\n", line + 1,
	((Buff->line_len[line] / MEMBLK) * MEMBLK) + MEMBLK);

	return Buff->text[line];
}

char* shrink_act_line(f_mtdt* Buff)
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
	chk_ptr(Buff, ACT_LN, "shrink the memblock with the current line\0");

	return ACT_LN;
}

char* shrink_prev_line(f_mtdt* Buff)
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
	chk_ptr(Buff, PREV_LN, "shrink the memblock with the previous line\0");

	printf("prev_line shrink: %d line %d\n", ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK, PREV_LN_INDEX + 1);

	return PREV_LN;
}

f_mtdt* extend_lines_array(f_mtdt* Buff)
{
	// Enhance the array that contains pointers to lines.
	Buff->text = realloc(Buff->text,
	(Buff->lines + INDEX) * sizeof(Buff->text));

	chk_ptr(Buff, Buff->text, "extend the array with lines\0");

	// Enhance the array that contains lines length indicators.
	Buff->line_len = realloc(Buff->line_len,
	(Buff->lines + INDEX) * sizeof(buff_t));

	chk_ptr(Buff, Buff->line_len, "extend the array with lines length\0");

	// The new line is allocated with only 4 or 8 bytes bytes.
	LAST_LN = malloc(INIT_MEMBLK);
	chk_ptr(Buff, LAST_LN, "malloc the new line\0");

	// Naturally the new line doesn't contains any chars - only terminator.
	LAST_LN_LEN = 0;

	return Buff;
}

f_mtdt* shrink_lines_array(f_mtdt* Buff)
{
	Buff->text = realloc(Buff->text,
	(Buff->lines + INDEX) * sizeof(Buff->text));

	chk_ptr(Buff, Buff->text, "shrink the array with lines\0");

	Buff->line_len = realloc(Buff->line_len,
	(Buff->lines + INDEX) * sizeof(buff_t));

	chk_ptr(Buff, Buff->line_len, "shrink the array with lines length\0");

	return Buff;
}

