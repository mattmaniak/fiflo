#include "include/buffer.h"
#include "include/ascii.h"
#include "include/charmap.h"

#include "include/file.h"

f_mtdt* key_action(f_mtdt* Buff, char key)
{
	switch(key)
	{
		default:
			return printable_char(Buff, key);

		case '\t':
			for(uint8_t tab_width = 0; tab_width < 2; tab_width++)
			{
				Buff = printable_char(Buff, ' ');
			}
			break;

		case BACKSPACE:
			return backspace(Buff);

		case CTRL_Q:
			buffer.free_exit(Buff, 1); // TODO.
			break;

		case CTRL_S:
			return file.save(Buff);

		case CTRL_BACKSLASH:
			Buff->pane_toggled = !Buff->pane_toggled;
			break;

		case CTRL_D:
			return delete_line(Buff);

		case CTRL_O:
			Buff->live_fname_edit = true;
	}
	return Buff;
}

f_mtdt* printable_char(f_mtdt* Buff, char key)
{
	const bool nul_sz = 1;

#ifndef ALL_KEYS
	const bool in_keymap = (key == '\0') || (key == '\n') || (key >= 32);
#else
	const bool in_keymap = true;
#endif

	if(in_keymap)
	{
		if(BUFFER_NOT_FULL)
		{
			Buff->chars_i++;
			ACT_LINE_LEN_I++;

			ACT_LINE = extend_line_mem(Buff, ACT_LINE_I);

			if(CURSOR_X_SCROLLED)
			{
				Buff = shift_text_horizonally(Buff, 'r');
			}
			ACT_LINE[CURSOR_VERTICAL_I - nul_sz] = key;
			ACT_LINE[ACT_LINE_LEN_I] = '\0';

			// Initializer handling.
			if((key == '\0') && !EMPTY_LINE)
			{
				Buff->chars_i--;
				ACT_LINE_LEN_I--;
			}
			else if(key == '\n')
			{
				Buff = linefeed(Buff);
			}
			(key != '\0') ? SET_STATUS("edited") : 0;
		}
		else
		{
			SET_STATUS("can't read or insert more data");
		}
	}
	else
	{
		SET_STATUS("unsupported char(s)");
	}
	return Buff;
}

f_mtdt* linefeed(f_mtdt* Buff)
{
	if(BUFFER_NOT_FULL)
	{
		Buff->lines_i++;
		Buff = extend_lines_array_mem(Buff);

		if(CURSOR_X_SCROLLED)
		{
			Buff = move_lines_forward(Buff);
		}
		else if(CURSOR_Y_SCROLLED) // Cursor is to the end of the line.
		{
			Buff = copy_lines_mem_forward(Buff);
		}
		ACT_LINE[ACT_LINE_LEN_I] = '\0';
	}
	return Buff;
}

f_mtdt* backspace(f_mtdt* Buff)
{
	if(!EMPTY_LINE)
	{
		Buff = delete_char(Buff);
	}
	else if(!FIRST_LINE && !CURSOR_Y_SCROLLED)
	{
		Buff = delete_last_empty_line(Buff);
	}
	ACT_LINE[ACT_LINE_LEN_I] = '\0'; // Linefeed to the terminator.
	SET_STATUS("edited");

	return Buff;
}
