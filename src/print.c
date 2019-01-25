#include "buffer.h"
#include "config.h"
#include "ui.h"
#include "print.h"

void print__line_with_tabs(void)
{

}

idx_t print__set_start_line(Buff_t* Buff, Ui_t* Ui)
{
	idx_t scrolled_lines = 0;

	if(CURRENT_LINE_IDX >=  Ui->text_y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled_lines = Buff->lines_i + IDX - Ui->text_y - Buff->cusr_y;
	}
	return scrolled_lines;
}

void print__actual_line(Buff_t* Buff, Ui_t* Ui)
{
	// There is small amount of chars. Horizontal scroll isn't required.
	if(CURRENT_LINE_LENGTH_IDX < Ui->text_x)
	{
		for(idx_t ch = 0; ch < CURRENT_LINE_LENGTH_IDX; ch++)
		{
			if(CURRENT_LINE[ch] == '\t')
			{
				putchar(' ');
			}
			else
			{
				putchar(CURRENT_LINE[ch]);
			}
		}
		// printf("%s", CURRENT_LINE);
	}
	// Chars won't fits in the horizontal space.
	else if((CURRENT_LINE_LENGTH_IDX - Ui->text_x) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		print__scroll_line_horizontally(Buff, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		for(idx_t ch = 0; ch < CURRENT_LINE_LENGTH_IDX; ch++)
		{
			if(CURRENT_LINE[ch] == '\t')
			{
				putchar(' ');
			}
			else
			{
				putchar(CURRENT_LINE[ch]);
			}
		}
		// printf("%.*s", Ui->text_x - CUR_SZ, CURRENT_LINE);

		// For proper rendering.
		if(((CURRENT_LINE_IDX + IDX) < Ui->text_y) && (CURRENT_LINE_IDX != Buff->lines_i))
		{
			WRAP_LINE();
		}
	}
}

void print__another_line(Buff_t* Buff, Ui_t* Ui, Conf_t* Config, idx_t line_i)
{
	// const unsigned int cursor_or_linefeed_sz = 1;

	ui__print_line_number(Buff, Config, line_i, Ui->line_num_len);

	for(idx_t ch = 0; ch < Buff->line_len_i[line_i]; ch++)
	{
		if(Buff->text[line_i][ch] == '\t')
		{
			putchar(' ');
		}
		else
		{
			putchar(Buff->text[line_i][ch]);
		}
	}
	// printf("%.*s", Ui->text_x - cursor_or_linefeed_sz, Buff->text[line_i]);

	if(Buff->line_len_i[line_i] >= Ui->text_x)
	{
		WRAP_LINE();
	}
}

void print__scroll_line_horizontally(Buff_t* Buff, Ui_t* Ui)
{
	// Text will be scrolled. Not cursor.
	for(idx_t char_i = CURSOR_X_POSITION + CUR_SZ - Ui->text_x;
	    char_i < CURSOR_X_POSITION; char_i++)
	{
		if(CURRENT_LINE[char_i] == '\t')
		{
			putchar(' ');
		}
		else
		{
			putchar(CURRENT_LINE[char_i]);
		}
	}
	/* Sometimes is needed because the "fill" function renders the smallest
	amount of linefeeds. In other cases the linefeed is provided by the char in
	a line. */
	if(CURSOR_Y_SCROLLED)
	{
		WRAP_LINE();
	}
}

void print__fit_lines(Buff_t* Buff, Ui_t* Ui, Conf_t* Config)
{
	const idx_t actual_line_size = 1;
	idx_t       line_i;

	for(line_i = 0; line_i < CURRENT_LINE_IDX; line_i++)
	{
		print__another_line(Buff, Ui, Config, line_i);
	}
	ui__print_line_number(Buff, Config, CURRENT_LINE_IDX, Ui->line_num_len);
	print__actual_line(Buff, Ui);

	if(CURSOR_Y_SCROLLED)
	{
		for(line_i += actual_line_size; line_i < Buff->lines_i; line_i++)
		{
			print__another_line(Buff, Ui, Config, line_i);
		}
		ui__print_line_number(Buff, Config, Buff->lines_i, Ui->line_num_len);
		printf("%.*s", Ui->text_x - LF_SZ, LAST_LINE);
	}
}

void print__shrink_lines(Buff_t* Buff, Ui_t* Ui, Conf_t* Config)
{
	idx_t last_ln = (idx_t) Ui->text_y - IDX;
	idx_t line_i;

	// Previous lines. If scrolled. Only beginning is shown.
	for(line_i = 0; line_i < CURRENT_LINE_IDX; line_i++)
	{
		print__another_line(Buff, Ui, Config, line_i);
	}
	ui__print_line_number(Buff, Config, CURRENT_LINE_IDX, Ui->line_num_len);
	print__actual_line(Buff, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	for(line_i = CURRENT_LINE_IDX + IDX; line_i < last_ln; line_i++)
	{
		print__another_line(Buff, Ui, Config, line_i);
	}
	ui__print_line_number(Buff, Config, last_ln, Ui->line_num_len);

	if(Buff->line_len_i[last_ln] < Ui->text_x)
	{
		for(idx_t ch = 0; ch < (Buff->line_len_i[last_ln] - LF_SZ); ch++)
		{
			if(Buff->text[last_ln][ch] == '\t')
			{
				putchar(' ');
			}
			else
			{
				putchar(Buff->text[last_ln][ch]);
			}
		}
		// printf("%.*s", Buff->line_len_i[last_ln] - LF_SZ, Buff->text[last_ln]);
	}
	else
	{
		for(idx_t ch = 0; ch < (idx_t) (Ui->text_x - LF_SZ); ch++)
		{
			if(Buff->text[last_ln][ch] == '\t')
			{
				putchar(' ');
			}
			else
			{
				putchar(Buff->text[last_ln][ch]);
			}
		}
		printf("%.*s", Ui->text_x - LF_SZ, Buff->text[last_ln]);
	}
}

void print__scroll_lines(Buff_t* Buff, Ui_t* Ui, Conf_t* Config)
{
	idx_t chars_end_offset = CURRENT_LINE_LENGTH_IDX;

	// Previous lines. If scrolled. Only beginning is shown.
	for(idx_t line_i = print__set_start_line(Buff, Ui); line_i < CURRENT_LINE_IDX; line_i++)
	{
		print__another_line(Buff, Ui, Config, line_i);
	}

	// Display the last line without the linefeed.
	ui__print_line_number(Buff, Config, CURRENT_LINE_IDX, Ui->line_num_len);

	if(CURRENT_LINE_LENGTH_IDX < Ui->text_x)
	{
		// printf("%.*s",
		//        (CURSOR_Y_SCROLLED) ? CURRENT_LINE_LENGTH_IDX - LF_SZ : CURRENT_LINE_LENGTH_IDX,
		//        CURRENT_LINE);

		if((CURRENT_LINE_LENGTH_IDX > 0) && (CURRENT_LINE[CURRENT_LINE_LENGTH_IDX - 1] == '\n'))
		{
			chars_end_offset--;
		}

		for(idx_t char_i = 0; char_i < chars_end_offset; char_i++)
		{
			if(CURRENT_LINE[char_i] == '\t')
			{
				putchar(' ');
			}
			else
			{
				putchar(CURRENT_LINE[char_i]);
			}
			// printf("exec %d\n", char_i);
		}
	}
	// Chars won't fits in the horizontal space.
	else if((CURRENT_LINE_LENGTH_IDX - Ui->text_x) >= Buff->cusr_x)
	{
		// Text will be scrolled. Not cursor.
		for(idx_t char_i = CURSOR_X_POSITION + CUR_SZ - Ui->text_x;
		    char_i < CURSOR_X_POSITION; char_i++)
		{
			if(CURRENT_LINE[char_i] == '\t')
			{
				putchar(' ');
			}
			else
			{
				putchar(CURRENT_LINE[char_i]);
			}
			// putchar(CURRENT_LINE[char_i]);
		}
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui->text_x - LF_SZ, CURRENT_LINE);
	}
}

void print__display_text(Buff_t* Buff, Ui_t* Ui, Conf_t* Config)
{
	if(Buff->lines_i < Ui->text_y)
	{
		print__fit_lines(Buff, Ui, Config);
	}
	else if((CURRENT_LINE_IDX + IDX) < Ui->text_y)
	{
		print__shrink_lines(Buff, Ui, Config);
	}
	else
	{
		print__scroll_lines(Buff, Ui, Config);
	}
}
