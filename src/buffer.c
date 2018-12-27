#include "include/buffer.h"

#include "include/memory.h"

static Buff_t* init(Buff_t* Buff)
{
	Buff->text = malloc(ADDR_SZ);
	memory.chk_ptr(Buff, Buff->text, " malloc the array with lines");

	Buff->line_len_i = malloc(((sizeof(idx_t) / ADDR_SZ) * ADDR_SZ) + ADDR_SZ);
	memory.chk_ptr(Buff, Buff->line_len_i, "malloc the array with lines length");

	Buff->chars_i = 0;
	Buff->lines_i = 0;
	Buff->cusr_x = 0;
	Buff->cusr_y = 0;
	ACT_LINE_LEN_I = 0;

	Buff->live_fname_edit = false;
	Buff->key_sequence = false;
	Buff->pane_toggled = false;

	ACT_LINE = malloc(ADDR_SZ);
	memory.chk_ptr(Buff, ACT_LINE, "malloc the first line");

	return Buff;
}

static void free_all(Buff_t* Buff)
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

static noreturn void throw_error(Buff_t* Buff, const char* err_msg)
{
	fprintf(stderr, "%s\n", err_msg);
	free_all(Buff);
	exit(1);
}

namespace_buffer buffer =
{
	init,
	free_all,
	throw_error
};
