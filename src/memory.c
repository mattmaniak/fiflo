#include "buffer.h"
#include "memory.h"

bool memory__extend_line(Buff_t* Buff, idx_t line_i)
{
	idx_t memblock = MEMBLOCK;

	if(Buff->line_len_i[line_i] == INITIAL_MEMBLOCK)
	{
		// There are 4/8 chars, extend to MEMBLOCK.
		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Extend_line %d with mem of %d B.\n",
		       line_i + IDX, memblock);
#endif

	}
	else if((Buff->line_len_i[line_i] > INITIAL_MEMBLOCK)
	&& (Buff->line_len_i[line_i] % MEMBLOCK == 0))
	{
		// There are more chars so append the new memblock.
		memblock = ((Buff->line_len_i[line_i] / MEMBLOCK) * MEMBLOCK) + MEMBLOCK;
		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Extend_line %d with mem of %d B.\n",
		       line_i + IDX, memblock);
#endif

	}
	if(Buff->text[line_i] == NULL)
	{
		fprintf(stderr, "Can't extend a memory block for the line.\n");
		return false;
	}
	return true;
}

bool memory__shrink_act_line(Buff_t* Buff)
{
	idx_t memblock = INITIAL_MEMBLOCK;

	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_act_line". */
	if((CURRENT_LINE_LENGTH_IDX >= INITIAL_MEMBLOCK) && (CURRENT_LINE_LENGTH_IDX < MEMBLOCK))
	{
		// Shrink to size of the MEMBLOCK.
		memblock = MEMBLOCK;
	}
	else if(CURRENT_LINE_LENGTH_IDX >= MEMBLOCK)
	{
		// Remove the newest memblock because isn't needed now.
		memblock = ((CURRENT_LINE_LENGTH_IDX / MEMBLOCK) * MEMBLOCK) + MEMBLOCK;
	}
	CURRENT_LINE = realloc(CURRENT_LINE, memblock);
	if(CURRENT_LINE == NULL)
	{
		fprintf(stderr, "Can't shrink the memblock with the current line.\n");
		return false;
	}

#ifdef DEBUG_MEMORY
	printf("Shrink_act_line %d with mem of %d B.\n",
	       CURRENT_LINE_IDX + IDX, memblock);
#endif
	return true;
}

bool memory__shrink_prev_line(Buff_t* Buff)
{
	idx_t memblock = INITIAL_MEMBLOCK;

	if((PREVIOUS_LINE_LENGTH_IDX >= INITIAL_MEMBLOCK) && (PREVIOUS_LINE_LENGTH_IDX < MEMBLOCK))
	{
		memblock = MEMBLOCK;
	}
	else if((PREVIOUS_LINE_LENGTH_IDX >= MEMBLOCK))
	{
		// Set the size of some MEMBLKs.
		memblock = ((PREVIOUS_LINE_LENGTH_IDX / MEMBLOCK) * MEMBLOCK) + MEMBLOCK;
	}
	PREVIOUS_LINE = realloc(PREVIOUS_LINE, memblock);

#ifdef DEBUG_MEMORY
	printf("Shrink_prev_line %d with mem of %d B\n",
	       PREVIOUS_LINE_IDX + IDX, memblock);
#endif

	if(PREVIOUS_LINE == NULL)
	{
		fprintf(stderr, "Can't shrink the memblock with the previous line.\n");
		return false;
	}
	return true;
}

bool memory__extend_lines_array(Buff_t* Buff)
{
	// Enhance the array that contains pointers to lines.
	Buff->text = realloc(Buff->text, (Buff->lines_i + IDX) * ADDRESS_SZ);
	if(Buff->text == NULL)
	{
		fprintf(stderr, "Can't extend the array with lines.\n");
		return false;
	}

	// Enhance the array that contains lines length indicators.
	Buff->line_len_i = realloc(Buff->line_len_i,
	                           (Buff->lines_i + IDX) * ADDRESS_SZ);
	if(Buff->line_len_i == NULL)
	{
		fprintf(stderr, "Can't extend the array with lines length.\n");
		return false;
	}

	// The new line is allocated with only 4 or 8 bytes bytes.
	LAST_LINE = malloc(INITIAL_MEMBLOCK);
	if(LAST_LINE == NULL)
	{
		fprintf(stderr, "Can't alloc a memory for the new line.\n");
		return false;
	}

	// Naturally the new line doesn't contains any chars - only terminator.
	LAST_LINE_LENGTH_IDX = 0;

	return true;
}

bool memory__shrink_lines_array(Buff_t* Buff)
{
	Buff->text = realloc(Buff->text, (Buff->lines_i + IDX) * ADDRESS_SZ);
	if(Buff->text == NULL)
	{
		fprintf(stderr, "Can't shrink the array with lines.\n");
		return false;
	}

	Buff->line_len_i = realloc(Buff->line_len_i,
	                           (Buff->lines_i + IDX) * ADDRESS_SZ);
	if(Buff->line_len_i == NULL)
	{
		fprintf(stderr, "Can't shrink the array with lines length.\n");
		return false;
	}
	return true;
}

bool memory__copy_lines_forward(Buff_t* Buff)
{
	const unsigned int prev = 1;

	for(idx_t line_i = Buff->lines_i; line_i > CURRENT_LINE_IDX; line_i--)
	{
		idx_t memblock =
		((Buff->line_len_i[line_i - prev] / MEMBLOCK) * MEMBLOCK) + MEMBLOCK;

		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Copied line %d forward to %d with new mem of %d B.\n",
		       line_i + IDX - prev, line_i + IDX, memblock);
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

bool memory__copy_lines_backward(Buff_t* Buff)
{
	const unsigned int next = 1;

	for(idx_t line_i = CURRENT_LINE_IDX; line_i < Buff->lines_i; line_i++)
	{
		idx_t memblock =
		((Buff->line_len_i[line_i + next] / MEMBLOCK) * MEMBLOCK) + MEMBLOCK;

		Buff->text[line_i] = realloc(Buff->text[line_i], memblock);

#ifdef DEBUG_MEMORY
		printf("Copied line %d backward to %d with new mem of %d B.\n",
		       line_i + IDX + next, line_i + IDX, memblock);
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
