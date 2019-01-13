#include "buffer.h"

bool buffer_init(Buff_t* Buff)
{
	Buff->text = malloc(ADDR_SZ);
	if(Buff->text == NULL)
	{
		fprintf(stderr, "Can't alloc a memory the array with lines.\n");
		return false;
	}

	Buff->line_len_i = malloc(((sizeof(idx_t) / ADDR_SZ) * ADDR_SZ) + ADDR_SZ);
	if(Buff->line_len_i == NULL)
	{
		fprintf(stderr, "Can't alloc a memory the array with lines length.\n");
		return false;
	}

	Buff->chars_i  = 0;
	Buff->lines_i  = 0;
	Buff->cusr_x   = 0;
	Buff->cusr_y   = 0;
	ACT_LINE_LEN_I = 0;

	Buff->live_fname_edit = false;
	Buff->key_sequence    = false;
	Buff->pane_toggled    = false;

	ACT_LINE = malloc(ADDR_SZ);
	if(ACT_LINE == NULL)
	{
		fprintf(stderr, "Can't alloc a memory for the first line.\n");
		return false;
	}
	return true;
}

void buffer_free(Buff_t* Buff)
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
