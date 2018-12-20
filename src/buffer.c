#include "include/buffer.h"

noreturn void free_all_exit(f_mtdt* Buff, const bool status)
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
