#include "buffer.h"
#include "charmap.h"
#include "edit.h"

f_mtdt* parse_key(f_mtdt* Buff, char key)
{
	enum           {seq_len = 8};
	static char    key_sequence[seq_len];
	static uint8_t char_i;

	if((key == ESC__CTRL_LEFT_SQR_BRACKET) && (!Buff->live_fname_edit))
	{

#ifndef VALUES_INSTEAD_OF_ACTIONS
		Buff->key_sequence = true;
#endif

		char_i = 0;
	}
	if(Buff->key_sequence)
	{
		key_sequence[char_i]          = key;
		key_sequence[char_i + NUL_SZ] = NUL__CTRL_SHIFT_2;

		if(char_i < (seq_len - NUL_SZ))
		{
			char_i++;
		}
		Buff = recognize_arrow(Buff, key_sequence);

		if(!Buff->key_sequence)
		{
			char_i = 0;
		}
	}
	else if(Buff->live_fname_edit)
	{
		Buff = edit_fname(Buff, key);
	}
	else if(!Buff->live_fname_edit)
	{
		Buff = key_action(Buff, key);
	}
	return Buff;
}

f_mtdt* delete_last_line(f_mtdt* Buff)
{
	safer_free(LAST_LINE);

	Buff->lines_i--;
	Buff = shrink_lines_array_mem(Buff);

	return Buff;
}

f_mtdt* delete_line(f_mtdt* Buff)
{
	if(Buff->lines_i > 0)
	{
		if(Buff->cusr_y == 0)
		{
			Buff = delete_last_line(Buff);

			/* With the last line deletion there is a need to remove the
			linefeed in the previous line. */
			LAST_LINE_LEN_I--;
			LAST_LINE[LAST_LINE_LEN_I] = NUL__CTRL_SHIFT_2;

			Buff->cusr_x = 0;
		}
		else
		{
			Buff = copy_lines_backward(Buff);
			Buff = delete_last_line(Buff);

			Buff->cusr_x = 1;
			Buff->cusr_y--;
		}
	}
	else
	{
		LAST_LINE_LEN_I = 0;
		LAST_LINE[LAST_LINE_LEN_I] = NUL__CTRL_SHIFT_2;

		LAST_LINE = realloc(LAST_LINE, sizeof(Buff->text));
		chk_ptr(Buff, LAST_LINE, "malloc after the first line removal");

		Buff->cusr_x = 0;
	}
	return Buff;
}

f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction)
{
	const bool prev = 1;
	buff_t     char_i;

	switch(direction)
	{
		case 'l':
		{
			for(char_i = CURSOR_VERTICAL_I; char_i <= ACT_LINE_LEN_I; char_i++)
			{
				ACT_LINE[char_i - prev] = ACT_LINE[char_i];
			}
			break;
		}
		case 'r':
		{
			for(char_i = ACT_LINE_LEN_I; char_i >= CURSOR_VERTICAL_I; char_i--)
			{
				ACT_LINE[char_i] = ACT_LINE[char_i - prev];
			}
		}
	}
	return Buff;
}
