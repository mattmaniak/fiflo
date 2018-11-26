#include "fiflo.h"
#include "memory.h"

char* extend_line(f_mtdt* Buff, buff_t line_i)
{
	buff_t memblock = MEMBLK;

	if(Buff->line_len_i[line_i] == INIT_MEMBLK)
	{
		// There are 4/8 chars, extend to MEMBLK.
		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG
		printf("Extend_line %d with mem of %d B.\n", line_i + INDEX, memblock);
#endif

	}
	else if((Buff->line_len_i[line_i] > INIT_MEMBLK)
	&&      (Buff->line_len_i[line_i] % MEMBLK == 0))
	{
		// There are more chars so append the new memblock.
		memblock = ((Buff->line_len_i[line_i] / MEMBLK) * MEMBLK) + MEMBLK;
		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG
		printf("Extend_line %d with mem of %d B.\n", line_i + INDEX, memblock);
#endif

	}
	chk_ptr(Buff, Buff->text[line_i], "extend the memblock for the line\0");

	return Buff->text[line_i];
}

char* shrink_act_line(f_mtdt* Buff)
{
	buff_t memblock = INIT_MEMBLK;

	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_act_line". */
	if((ACT_LINE_LEN_I >= INIT_MEMBLK) && (ACT_LINE_LEN_I < MEMBLK))
	{
		// Shrink to size of the MEMBLK.
		memblock = MEMBLK;
	}
	else if(ACT_LINE_LEN_I >= MEMBLK)// && (ACT_LINE_LEN_I % MEMBLK == 0))
	{
		// Remove the newest memblock because isn't needed now.
		memblock = ((ACT_LINE_LEN_I / MEMBLK) * MEMBLK) + MEMBLK;
	}
	ACT_LINE = realloc(ACT_LINE, memblock);

#ifdef DEBUG
	printf("Shrink_act_line %d with mem of %d B.\n",
	ACT_LINE_I + INDEX, memblock);
#endif

	chk_ptr(Buff, ACT_LINE, "shrink the memblock with the current line\0");

	return ACT_LINE;
}

char* shrink_prev_line(f_mtdt* Buff)
{
	buff_t memblock = INIT_MEMBLK;

	if((PREV_LINE_LEN_I >= INIT_MEMBLK) && (PREV_LINE_LEN_I < MEMBLK))
	{
		memblock = MEMBLK;
	}
	else if((PREV_LINE_LEN_I >= MEMBLK))
	{
		// Set the size of some MEMBLKs.
		memblock = ((PREV_LINE_LEN_I / MEMBLK) * MEMBLK) + MEMBLK;
	}
	PREV_LINE = realloc(PREV_LINE, memblock);

#ifdef DEBUG
	printf("Shrink_prev_line %d with mem of %d B\n",
	PREV_LINE_I + INDEX, memblock);
#endif

	chk_ptr(Buff, PREV_LINE, "shrink the memblock with the previous line\0");

	return PREV_LINE;
}

f_mtdt* extend_lines_array(f_mtdt* Buff)
{
	// Enhance the array that contains pointers to lines.
	Buff->text = realloc(Buff->text, (Buff->lines_i + INDEX) * ADDR_SZ);

	chk_ptr(Buff, Buff->text, "extend the array with lines\0");

	// Enhance the array that contains lines length indicators.
	Buff->line_len_i = realloc(Buff->line_len_i,
	(Buff->lines_i + INDEX) * sizeof(buff_t));

	chk_ptr(Buff, Buff->line_len_i, "extend the array with lines length\0");

	// The new line is allocated with only 4 or 8 bytes bytes.
	LAST_LINE = malloc(INIT_MEMBLK);
	chk_ptr(Buff, LAST_LINE, "malloc the new line\0");

	// Naturally the new line doesn't contains any chars - only terminator.
	LAST_LINE_LEN_I = 0;

	return Buff;
}

f_mtdt* shrink_lines_array(f_mtdt* Buff)
{
	Buff->text = realloc(Buff->text, (Buff->lines_i + INDEX) * ADDR_SZ);

	chk_ptr(Buff, Buff->text, "shrink the array with lines\0");

	Buff->line_len_i = realloc(Buff->line_len_i,
	(Buff->lines_i + INDEX) * sizeof(buff_t));

	chk_ptr(Buff, Buff->line_len_i, "shrink the array with lines length\0");

	return Buff;
}

f_mtdt* copy_lines_forward(f_mtdt* Buff)
{
	const _Bool prev = 1;

	for(buff_t line_i = Buff->lines_i; line_i > ACT_LINE_I; line_i--)
	{
		buff_t memblock =
		((Buff->line_len_i[line_i - prev] / MEMBLK) * MEMBLK) + MEMBLK;

		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG
		printf("Copied line %d forward to %d with new mem of %d B.\n",
		line_i + INDEX - prev, line_i + INDEX, memblock);
#endif

		chk_ptr(Buff, Buff->text[line_i], "copy the line forward\0");

		Buff->text[line_i] = strcpy(Buff->text[line_i], Buff->text[line_i - prev]);

		Buff->line_len_i[line_i] = Buff->line_len_i[line_i - prev];
	}
	return Buff;
}

f_mtdt* copy_lines_backward(f_mtdt* Buff)
{
	const _Bool next = 1;

	for(buff_t line_i = ACT_LINE_I; line_i < Buff->lines_i; line_i++)
	{
		buff_t memblock =
		((Buff->line_len_i[line_i + next] / MEMBLK) * MEMBLK) + MEMBLK;

		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG
		printf("Copied line %d backward to %d with new mem of %d B.\n",
		line_i + INDEX + next, line_i + INDEX, memblock);
#endif

		chk_ptr(Buff, Buff->text[line_i], "copy the line backward\0");

		Buff->text[line_i] = strcpy(Buff->text[line_i], Buff->text[line_i + next]);

		Buff->line_len_i[line_i] = Buff->line_len_i[line_i + next];
	}
	return Buff;
}
