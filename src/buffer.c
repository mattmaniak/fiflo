#include "buffer.h"
#include "file.h"
#include "memory.h"

f_mtdt* init_buffer(f_mtdt* Buff, const char* arg)
{
	Buff = set_fname(Buff, arg);

	Buff->text = malloc(ADDR_SZ);
	chk_ptr(Buff, Buff->text, "malloc the array with lines\0");

	Buff->line_len_i = malloc(sizeof(Buff->line_len_i));
	chk_ptr(Buff, Buff->line_len_i, "malloc the array with lines length\0");

	Buff->chars_i = 0;
	Buff->lines_i = 0;
	Buff->cusr_x  = 0;
	Buff->cusr_y  = 0;

	ACT_LINE_LEN_I = 0;
	ACT_LINE = malloc(ADDR_SZ);

	Buff->live_fname_edit = false;

	return Buff;
}

_Noreturn void free_buff_exit(f_mtdt* Buff, const bool code)
{
	for(buff_t line_i = 0; line_i <= Buff->lines_i; line_i++)
	{
		safer_free(Buff->text[line_i]);
	}
	safer_free(Buff->text);
	safer_free(Buff->line_len_i);
	safer_free(Buff);

	exit(code);
}
