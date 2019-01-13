#include "buffer.h"
#include "memory.h"

bool memory_extend_line(Buff_t* Buff, idx_t line_i)
{
	idx_t memblock = MEMBLK;

	if(Buff->line_len_i[line_i] == INIT_MEMBLK)
	{
		// There are 4/8 chars, extend to MEMBLK.
		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Extend_line %d with mem of %d B.\n",
		       line_i + INDEX, memblock);
#endif

	}
	else if((Buff->line_len_i[line_i] > INIT_MEMBLK)
	&& (Buff->line_len_i[line_i] % MEMBLK == 0))
	{
		// There are more chars so append the new memblock.
		memblock = ((Buff->line_len_i[line_i] / MEMBLK) * MEMBLK) + MEMBLK;
		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Extend_line %d with mem of %d B.\n",
		       line_i + INDEX, memblock);
#endif

	}
	if(Buff->text[line_i] == NULL)
	{
		fprintf(stderr, "Can't extend a memory block for the line.\n");
		return false;
	}
	return true;
}

bool memory_shrink_act_line(Buff_t* Buff)
{
	idx_t memblock = INIT_MEMBLK;

	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_act_line". */
	if((ACT_LINE_LEN_I >= INIT_MEMBLK) && (ACT_LINE_LEN_I < MEMBLK))
	{
		// Shrink to size of the MEMBLK.
		memblock = MEMBLK;
	}
	else if(ACT_LINE_LEN_I >= MEMBLK)
	{
		// Remove the newest memblock because isn't needed now.
		memblock = ((ACT_LINE_LEN_I / MEMBLK) * MEMBLK) + MEMBLK;
	}
	ACT_LINE = realloc(ACT_LINE, memblock);
	if(ACT_LINE == NULL)
	{
		fprintf(stderr, "Can't shrink the memblock with the current line.\n");
		return false;
	}

#ifdef DEBUG_MEMORY
	printf("Shrink_act_line %d with mem of %d B.\n",
	       ACT_LINE_I + INDEX, memblock);
#endif
	return true;
}

bool memory_shrink_prev_line(Buff_t* Buff)
{
	idx_t memblock = INIT_MEMBLK;

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

#ifdef DEBUG_MEMORY
	printf("Shrink_prev_line %d with mem of %d B\n",
	       PREV_LINE_I + INDEX, memblock);
#endif

	if(PREV_LINE == NULL)
	{
		fprintf(stderr, "Can't shrink the memblock with the previous line.\n");
		return false;
	}
	return true;
}

bool memory_extend_lines_array(Buff_t* Buff)
{
	// Enhance the array that contains pointers to lines.
	Buff->text = realloc(Buff->text, (Buff->lines_i + INDEX) * ADDR_SZ);
	if(Buff->text == NULL)
	{
		fprintf(stderr, "Can't extend the array with lines.\n");
		return false;
	}

	// Enhance the array that contains lines length indicators.
	Buff->line_len_i = realloc(Buff->line_len_i,
	                           (Buff->lines_i + INDEX) * ADDR_SZ);
	if(Buff->line_len_i == NULL)
	{
		fprintf(stderr, "Can't extend the array with lines length.\n");
		return false;
	}

	// The new line is allocated with only 4 or 8 bytes bytes.
	LAST_LINE = malloc(INIT_MEMBLK);
	if(LAST_LINE == NULL)
	{
		fprintf(stderr, "Can't alloc a memory for the new line.\n");
		return false;
	}

	// Naturally the new line doesn't contains any chars - only terminator.
	LAST_LINE_LEN_I = 0;

	return true;
}

bool memory_shrink_lines_array(Buff_t* Buff)
{
	Buff->text = realloc(Buff->text, (Buff->lines_i + INDEX) * ADDR_SZ);
	if(Buff->text == NULL)
	{
		fprintf(stderr, "Can't shrink the array with lines.\n");
		return false;
	}

	Buff->line_len_i = realloc(Buff->line_len_i,
	                           (Buff->lines_i + INDEX) * ADDR_SZ);
	if(Buff->line_len_i == NULL)
	{
		fprintf(stderr, "Can't shrink the array with lines length.\n");
		return false;
	}
	return true;
}

bool memory_copy_lines_forward(Buff_t* Buff)
{
	const unsigned int prev = 1;

	for(idx_t line_i = Buff->lines_i; line_i > ACT_LINE_I; line_i--)
	{
		idx_t memblock =
		((Buff->line_len_i[line_i - prev] / MEMBLK) * MEMBLK) + MEMBLK;

		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Copied line %d forward to %d with new mem of %d B.\n",
		       line_i + INDEX - prev, line_i + INDEX, memblock);
#endif

		if(Buff->text[line_i] == NULL)
		{
			fprintf(stderr, "Can't resize the line forward.\n");
			return false;
		}

		strcpy(Buff->text[line_i], Buff->text[line_i - prev]);
		Buff->line_len_i[line_i] = Buff->line_len_i[line_i - prev];
	}
	return true;
}

bool memory_copy_lines_backward(Buff_t* Buff)
{
	const unsigned int next = 1;

	for(idx_t line_i = ACT_LINE_I; line_i < Buff->lines_i; line_i++)
	{
		idx_t memblock =
		((Buff->line_len_i[line_i + next] / MEMBLK) * MEMBLK) + MEMBLK;

		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Copied line %d backward to %d with new mem of %d B.\n",
		       line_i + INDEX + next, line_i + INDEX, memblock);
#endif

		if(Buff->text[line_i] == NULL)
		{
			fprintf(stderr, "Can't copy a line backward.\n");
			return false;
		}
		strcpy(Buff->text[line_i], Buff->text[line_i + next]);

		Buff->line_len_i[line_i] = Buff->line_len_i[line_i + next];
	}
	return true;
}
