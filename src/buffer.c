#include "buffer.h"

bool buffer__init(Buff_t* Buffer)
{
	Buffer->text = malloc(ADDRESS_SZ);
	if(Buffer->text == NULL)
	{
		fprintf(stderr, "Can't alloc a memory the array with lines.\n");
		return false;
	}

	Buffer->lines_length_idx = malloc(((sizeof(idx_t) / ADDRESS_SZ)
	                                  * ADDRESS_SZ) + ADDRESS_SZ);
	if(Buffer->lines_length_idx == NULL)
	{
		fprintf(stderr, "Can't alloc a memory the array with lines length.\n");
		return false;
	}

	Buffer->chars_idx              = 0;
	Buffer->lines_idx              = 0;
	Buffer->preffered_cursor_rev_x = 0;
	Buffer->cursor_rev_x           = 0;
	Buffer->cursor_rev_y           = 0;
	CURRENT_LINE_LENGTH_IDX        = 0;

	Buffer->live_fname_edit = false;
	Buffer->chars_sequence  = false;
	Buffer->pane_toggled    = false;

	CURRENT_LINE = malloc(ADDRESS_SZ);
	if(CURRENT_LINE == NULL)
	{
		fprintf(stderr, "Can't alloc a memory for the first line.\n");
		return false;
	}
	return true;
}

void buffer__free(Buff_t* Buffer)
{
	for(idx_t line_idx = 0; line_idx <= Buffer->lines_idx; line_idx++)
	{
		if(Buffer->text[line_idx] != NULL)
		{
			free(Buffer->text[line_idx]);
		}
	}
	if(Buffer->lines_length_idx != NULL)
	{
		free(Buffer->lines_length_idx);
	}
	if(Buffer->text != NULL)
	{
		free(Buffer->text);
	}
}
