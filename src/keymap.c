#include "buffer.h"
#include "ascii.h"
#include "keymap.h"

#include "file.h"
#include "memory.h"
#include "edit.h"

static bool linefeed(Buff_t* Buff)
{
	if(BUFFER_NOT_FULL)
	{
		Buff->lines_i++;
		if(!memory.extend_lines_array(Buff))
		{
			return false;
		}

		if(CURSOR_X_SCROLLED)
		{
			if(!edit.move_lines_forward(Buff))
			{
				return false;
			}
		}
		else if(CURSOR_Y_SCROLLED) // Cursor is to the end of the line.
		{
			if(!memory.copy_lines_forward(Buff))
			{
				return false;
			}
		}
		ACT_LINE[ACT_LINE_LEN_I] = '\0';
	}
	return true;
}

static bool printable_char(Buff_t* Buff, char key)
{
	const size_t nul_sz = 1;

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

			if(!memory.extend_line(Buff, ACT_LINE_I))
			{
				return false;
			}

			if(CURSOR_X_SCROLLED)
			{
				edit.shift_text_horizonally(Buff, 'r');
			}
			ACT_LINE[CURSOR_VERTICAL_I - nul_sz] = key;
			ACT_LINE[ACT_LINE_LEN_I] = '\0';

			// Initializing nul handling.
			if((key == '\0') && !EMPTY_LINE)
			{
				Buff->chars_i--;
				ACT_LINE_LEN_I--;
			}
			else if(key == '\n')
			{
				if(!linefeed(Buff))
				{
					return false;
				}
			}
			if(key != '\0')
			{
				SET_STATUS("edited");
			}
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
	return true;
}

static bool backspace(Buff_t* Buff)
{
	if(!EMPTY_LINE)
	{
		if(!edit.delete_char(Buff))
		{
			return false;
		}
	}
	else if(!FIRST_LINE && !CURSOR_Y_SCROLLED)
	{
		if(!edit.delete_last_empty_line(Buff))
		{
			return false;
		}
	}
	ACT_LINE[ACT_LINE_LEN_I] = '\0'; // Linefeed to the terminator.
	SET_STATUS("edited");

	return true;
}

static bool key_action(Buff_t* Buff, char key)
{
	const char record_separator_fake_tab = 30;

	switch(key)
	{
		default:
		return printable_char(Buff, key);

		case '\t':
		for(size_t tab = 0; tab < 4; tab++)
		{
			if(!printable_char(Buff, record_separator_fake_tab))
			{
				return false;
			}
		}
		break;

		case BACKSPACE:
		return backspace(Buff);

		case CTRL_Q:
		return false;

		case CTRL_S:
		file.save(Buff);
		break;

		case CTRL_BACKSLASH:
		Buff->pane_toggled = !Buff->pane_toggled;
		break;

		case CTRL_D:
		return edit.delete_line(Buff);

		case CTRL_O:
		Buff->live_fname_edit = true;
	}
	return true;
}

static void arrow_left(Buff_t* Buff)
{
	bool more_than_one_line = (Buff->lines_i > 0);

	if(!CURSOR_AT_LINE_START)
	{
		Buff->cusr_x++;
	}
	else if(more_than_one_line && !CURSOR_AT_TOP)
	{
		// Set to the right ignoring the linefeed.
		Buff->cusr_x = 1;
		Buff->cusr_y++;
	}
}

static void arrow_right(Buff_t* Buff)
{
	if(CURSOR_X_SCROLLED)
	{
		Buff->cusr_x--;
		if(!CURSOR_X_SCROLLED && CURSOR_Y_SCROLLED)
		{
			Buff->cusr_y--;
			Buff->cusr_x = ACT_LINE_LEN_I;
		}
		// Last line doesn't contain linefeed so ignoring that isn't necessary.
		else if(!CURSOR_X_SCROLLED && (Buff->cusr_y == 1))
		{
			Buff->cusr_y--;
		}
	}
}

static void arrow_up(Buff_t* Buff)
{
	if(!CURSOR_AT_TOP)
	{
		/* Cursor at the left side: doesn't go at the end of a line. Always
		at the beginning or ignore the linefeed. */
		Buff->cusr_x = (CURSOR_AT_LINE_START) ? PREV_LINE_LEN_I : 1;
		Buff->cusr_y++;
	}
}

static void arrow_down(Buff_t* Buff)
{
	bool cursor_at_previous_line_start = CURSOR_AT_LINE_START;

	if(CURSOR_Y_SCROLLED)
	{
		Buff->cusr_y--;
		if(cursor_at_previous_line_start)
		{
			/* Cursor at the left side: doesn't go at the end of a line. Always
			at the beginning. */
			Buff->cusr_x = ACT_LINE_LEN_I;
		}
		else
		{
			Buff->cusr_x = (CURSOR_Y_SCROLLED) ? 1 : 0; // Ignore the LF or not.
		}
	}
}

namespace_keymap keymap =
{
	key_action,
	printable_char,
	linefeed,
	backspace,
	arrow_left,
	arrow_right,
	arrow_up,
	arrow_down
};
