#include "include/buffer.h"

f_mtdt* init(f_mtdt* Buff)
{
	Buff->text = malloc(ADDR_SZ);

	if(Buff->text == NULL)
	{
		fputs("Can't malloc the array with lines.\n", stderr);
		buffer.free_exit(Buff, 1);
	}
	Buff->line_len_i = malloc(((sizeof(buff_t) / ADDR_SZ) * ADDR_SZ) + ADDR_SZ);

	if(Buff->line_len_i == NULL)
	{
		fputs("Can't malloc the array with lines length.\n", stderr);
		buffer.free_exit(Buff, 1);
	}
	Buff->chars_i = 0;
	Buff->lines_i = 0;
	Buff->cusr_x = 0;
	Buff->cusr_y = 0;
	ACT_LINE_LEN_I = 0;

	Buff->live_fname_edit = false;
	Buff->key_sequence = false;
	Buff->pane_toggled = false;

	ACT_LINE = malloc(ADDR_SZ);

	if(ACT_LINE == NULL)
	{
		fputs("Can't malloc the array with lines length.\n", stderr);
		buffer.free_exit(Buff, 1);
	}
	return Buff;
}

noreturn void free_exit(f_mtdt* Buff, const bool status)
{
	for(buff_t line_i = 0; line_i <= Buff->lines_i; line_i++)
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
	if(Buff != NULL)
	{
		free(Buff);
	}
	exit(status);
}
