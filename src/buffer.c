#include "buffer.h"

bool buffer__init(Buff_t* Buff)
{
	Buff->text = malloc(ADDRESS_SZ);
	if(Buff->text == NULL)
	{
		fprintf(stderr, "Can't alloc a memory the array with lines.\n");
		return false;
	}

	Buff->line_len_i = malloc(((sizeof(idx_t) / ADDRESS_SZ) * ADDRESS_SZ) + ADDRESS_SZ);
	if(Buff->line_len_i == NULL)
	{
		fprintf(stderr, "Can't alloc a memory the array with lines length.\n");
		return false;
	}

	Buff->chars_i  = 0;
	Buff->lines_i  = 0;
	Buff->cusr_x   = 0;
	Buff->cusr_y   = 0;
	CURRENT_LINE_LENGTH_IDX = 0;

	Buff->live_fname_edit = false;
	Buff->chars_sequence  = false;
	Buff->pane_toggled    = false;

	CURRENT_LINE = malloc(ADDRESS_SZ);
	if(CURRENT_LINE == NULL)
	{
		fprintf(stderr, "Can't alloc a memory for the first line.\n");
		return false;
	}
	return true;
}

void buffer__free(Buff_t* Buff)
{
	for(idx_t line_i = 0; line_i <= Buff->lines_i; line_i++)
	{
		if(Buff->text[line_i] != NULL)
		{
			free(Buff->text[line_i]);
		}
	}
	if(Buff->line_len_i != NULL)
	{
		free(Buff->line_len_i);
	}
	if(Buff->text != NULL)
	{
		free(Buff->text);
	}
}
