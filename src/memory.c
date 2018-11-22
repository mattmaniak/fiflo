#include "fiflo.h"
#include "memory.h"

char* extend_line(f_mtdt* Buff, buff_t line)
{
	buff_t memblock = INIT_MEMBLK;

	if(Buff->line_len[line] == INIT_MEMBLK)
	{
		// There are 4/8 chars, extend to MEMBLK.
		Buff->text[line] = realloc(Buff->text[line], MEMBLK);

#ifdef DEBUG
		printf("Extend_line: %d, with mem: %d B.\n", line + INDEX, memblock);
#endif
	}
	else if((Buff->line_len[line] > INIT_MEMBLK)
	&& (Buff->line_len[line] % MEMBLK == 0))
	{
		// There are more chars so append the new memblock.
		memblock = ((Buff->line_len[line] / ADDR_SZ) * ADDR_SZ) + MEMBLK;
		Buff->text[line] = realloc(Buff->text[line], memblock);

#ifdef DEBUG
		printf("Extend_line: %d, with mem: %d B.\n", line + INDEX, memblock);
#endif
	}
	chk_ptr(Buff, Buff->text[line], "extend the memblock for the line\0");

	return Buff->text[line];
}

char* shrink_act_line(f_mtdt* Buff)
{
	buff_t memblock = INIT_MEMBLK;

	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_act_line". */
	if(ACT_LN_LEN == MEMBLK)
	{
		// Shrink to size of the MEMBLK.
		memblock = MEMBLK;
	}
	else if((ACT_LN_LEN > MEMBLK) && (ACT_LN_LEN % MEMBLK == 0))
	{
		// Remove the newest memblock because isn't needed now.
		memblock = (ACT_LN_LEN / ADDR_SZ) * ADDR_SZ;
	}
	ACT_LN = realloc(ACT_LN, memblock);

#ifdef DEBUG
	printf("Shrink_act_line: %d, with mem %d B.\n",
	ACT_LN_INDEX + INDEX, memblock);
#endif

	chk_ptr(Buff, ACT_LN, "shrink the memblock with the current line\0");

	return ACT_LN;
}

char* shrink_prev_line(f_mtdt* Buff)
{
	buff_t memblock = INIT_MEMBLK;

	if((PREV_LN_LEN >= INIT_MEMBLK) && (PREV_LN_LEN < MEMBLK))
	{
		memblock = MEMBLK;
	}
	else if((PREV_LN_LEN >= MEMBLK))
	{
		// Set the size of some MEMBLKs.
		memblock = ((PREV_LN_LEN / ADDR_SZ) * ADDR_SZ) + MEMBLK;
	}
	PREV_LN = realloc(PREV_LN, memblock);

#ifdef DEBUG
	printf("shrink_prev_line: %d with mem: %d B\n",
	PREV_LN_INDEX + INDEX, memblock);
#endif

	chk_ptr(Buff, PREV_LN, "shrink the memblock with the previous line\0");

	return PREV_LN;
}

f_mtdt* extend_lines_array(f_mtdt* Buff)
{
	// Enhance the array that contains pointers to lines.
	Buff->text = realloc(Buff->text, (Buff->lines + INDEX) * ADDR_SZ);

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
	Buff->text = realloc(Buff->text, (Buff->lines + INDEX) * ADDR_SZ);

	chk_ptr(Buff, Buff->text, "shrink the array with lines\0");

	Buff->line_len = realloc(Buff->line_len,
	(Buff->lines + INDEX) * sizeof(buff_t));

	chk_ptr(Buff, Buff->line_len, "shrink the array with lines length\0");

	return Buff;
}
