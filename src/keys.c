#include "buffer.h"
#include "config.h"
#include "ascii.h"
#include "keys.h"

bool keys__linefeed(Buff_t* Buffer)
{
	if(BUFFER_NOT_FULL)
	{
		Buffer->lines_idx++;
		if(!memory__extend_lines_array(Buffer))
		{
			return false;
		}

		if(CURSOR_X_SCROLLED)
		{
			if(!edit__move_lines_forward(Buffer))
			{
				return false;
			}
		}
		else if(CURSOR_Y_SCROLLED) // Cursor is to the end of the line.
		{
			if(!memory__copy_lines_forward(Buffer))
			{
				return false;
			}
		}
		CURRENT_LINE[CURRENT_LINE_LENGTH_IDX] = '\0';
	}
	return true;
}

bool keys__printable_char(Buff_t* Buffer, char key)
{

#ifdef DEBUG_KEYS
	const bool in_keymap = true;
#else
	const bool in_keymap = (key == '\0') || (key == '\t') || (key == '\n')
	                       || (key >= 32);
#endif

	if(in_keymap)
	{
		if(BUFFER_NOT_FULL)
		{
			Buffer->chars_idx++;
			CURRENT_LINE_LENGTH_IDX++;

			if(!memory__extend_line(Buffer, CURRENT_LINE_IDX))
			{
				return false;
			}

			if(CURSOR_X_SCROLLED)
			{
				edit__shift_text_horizonally(Buffer, 'r');
			}
			CURRENT_LINE[CURSOR_X_POS - NUL_SZ] = key;
			CURRENT_LINE[CURRENT_LINE_LENGTH_IDX] = '\0';

			// Initializing nul handling.
			if((key == '\0') && !EMPTY_LINE)
			{
				Buffer->chars_idx--;
				CURRENT_LINE_LENGTH_IDX--;
			}
			else if(key == '\n')
			{
				if(!keys__linefeed(Buffer))
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

bool keys__backspace(Buff_t* Buffer, Conf_t* Config)
{
	idx_t remembered_line_idx = CURRENT_LINE_IDX;

	for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value; tab_idx++)
	{
		if(!EMPTY_LINE)
		{
			if(!edit__delete_char(Buffer))
			{
				return false;
			}
		}
		else if(!FIRST_LINE && !CURSOR_Y_SCROLLED)
		{
			if(!edit__delete_last_empty_line(Buffer))
			{
				return false;
			}
			// break; // Ignore the tab loop when removing a line.
		}
		CURRENT_LINE[CURRENT_LINE_LENGTH_IDX] = '\0'; // Linefeed to the '\0'.

		// Tab as the last char.
		if((CURRENT_LINE_LENGTH_IDX > 0)
		   && (CURRENT_LINE[CURRENT_LINE_LENGTH_IDX - NUL_SZ] != '\t')
		   && Buffer->cursor_rev_x == 0)
		{
			puts("BREAK_1");
			break;
		}
		// Scenario when there is the tab and some text further.
		else if((CURRENT_LINE_LENGTH_IDX > 2) // TODO.
		        && (CURRENT_LINE[CURSOR_X_POS - NUL_SZ] != '\t')
		        && Buffer->cursor_rev_x > 0)
		{
			if(CURRENT_LINE_LENGTH_IDX == 1)
			{
				break;
			}
			puts("BREAK_2");
			break;
		}

		/* Prevents deleting [tab_width] lines at once with max. scrolled cursor
 		in X. */
		if(remembered_line_idx != CURRENT_LINE_IDX)
		{
			puts("BREAK_3");
			break;
		}
		puts("LOLO");
	}
	SET_STATUS("edited");

	return true;
}

bool keys__key_action(Buff_t* Buffer, Conf_t* Config, char key)
{
	switch(key)
	{
		default:
		return keys__printable_char(Buffer, key);

		case '\t':
		/* When the TAB key is pressed, it will insert e.g. 4 '\t' into the
		buffer. They will be converted during the rendering, loading and saving
		the file. */
		for(int char_idx = 0; char_idx < Config->Tab_width.value; char_idx++)
		{
			if(!keys__printable_char(Buffer, '\t'))
			{
				return false;
			}
		}
		break;

		case BACKSPACE:
		return keys__backspace(Buffer, Config);

		case CTRL_Q:
		return false;

		case CTRL_S:
		file__save(Buffer, Config);
		break;

		case CTRL_BACKSLASH:
		Buffer->pane_toggled = !Buffer->pane_toggled;
		break;

		case CTRL_D:
		return edit__delete_line(Buffer);

		case CTRL_O:
		Buffer->live_fname_edit = true;
	}
	return true;
}

void keys__arrow_left(Buff_t* Buffer, Conf_t* Config)
{
	bool more_than_one_line = Buffer->lines_idx > 0;

	if(!CURSOR_AT_LINE_START)
	{
		// Skip the e.g "\t\t\t\t" as the one tab.
		for(idx_t tab_idx = 1; tab_idx < (idx_t) Config->Tab_width.value;
		    tab_idx++)
		{
			if(CURRENT_LINE[CURSOR_X_POS - tab_idx] != '\t')
			{
				Buffer->cursor_rev_x++;
				break; // No tab, so don't overskip anything.
			}
			else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
			{
				Buffer->cursor_rev_x += (idx_t) Config->Tab_width.value;
			}
		}

	}
	else if(more_than_one_line && !CURSOR_AT_TOP)
	{
		// Set to the right ignoring the keys__linefeed.
		Buffer->cursor_rev_x = 1;
		Buffer->cursor_rev_y++;
	}
}

void keys__arrow_right(Buff_t* Buffer, Conf_t* Config)
{
	if(CURSOR_X_SCROLLED)
	{
		// Skip the e.g "\t\t\t\t" as the one tab.
		for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value;
		    tab_idx++)
		{
			if(CURRENT_LINE[CURSOR_X_POS + tab_idx] != '\t')
			{
				Buffer->cursor_rev_x--;
				break; // No tab, so don't overskip anything.
			}
			else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
			{
				Buffer->cursor_rev_x -= (idx_t) Config->Tab_width.value;
			}
		}
		if(!CURSOR_X_SCROLLED && CURSOR_Y_SCROLLED)
		{
			Buffer->cursor_rev_y--;
			Buffer->cursor_rev_x = CURRENT_LINE_LENGTH_IDX;
		}
		/* Last line doesn't contain keys__linefeed so ignoring that isn't
		necessary. */
		else if(!CURSOR_X_SCROLLED && (Buffer->cursor_rev_y == 1))
		{
			Buffer->cursor_rev_y--;
		}
	}
}

void keys__arrow_up(Buff_t* Buffer)
{
	if(!CURSOR_AT_TOP)
	{
		/* Cursor at the left side: doesn't go at the end of a line. Always
		at the beginning or ignore the keys__linefeed. */
		Buffer->cursor_rev_x = (CURSOR_AT_LINE_START)
		                       ? PREVIOUS_LINE_LENGTH_IDX : 1;
		Buffer->cursor_rev_y++;
	}
}

void keys__arrow_down(Buff_t* Buffer)
{
	bool cursor_at_previous_line_start = CURSOR_AT_LINE_START;

	if(CURSOR_Y_SCROLLED)
	{
		Buffer->cursor_rev_y--;
		if(cursor_at_previous_line_start)
		{
			/* Cursor at the left side: doesn't go at the end of a line. Always
			at the beginning. */
			Buffer->cursor_rev_x = CURRENT_LINE_LENGTH_IDX;
		}
		else
		{
			// Ignore the LF or not.
			Buffer->cursor_rev_x = (CURSOR_Y_SCROLLED) ? 1 : 0;
		}
	}
}
