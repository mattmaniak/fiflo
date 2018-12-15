#include "buffer.h"
#include "ui.h"
#include "render.h"

buff_t set_start_line(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t scrolled_lines = 0;

	if(ACT_LINE_I >= Ui.text_y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled_lines = Buff->lines_i + INDEX - Ui.text_y - Buff->cusr_y;
	}
	return scrolled_lines;
}

void scroll_line_horizontally(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t char_i = CURSOR_VERTICAL_I + CUR_SZ - Ui.text_x;

	// Text will be scrolled. Not cursor.
	for(; char_i < CURSOR_VERTICAL_I; char_i++)
	{
		putchar(ACT_LINE[char_i]);
	}
	/* Sometimes is needed because the "fill" function renders the smallest
	amount of linefeeds. In other cases the linefeed is provided by the char in
	a line. */
	(CURSOR_Y_SCROLLED) ? WRAP_LINE() : 0;
}

void print_actual_line(f_mtdt* Buff, win_mtdt Ui)
{
	// There is small amount of chars. Horizontal scroll isn't required.
	if(ACT_LINE_LEN_I < Ui.text_x)
	{
		printf("%s", ACT_LINE);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LINE_LEN_I - Ui.text_x) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_horizontally(Buff, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui.text_x - CUR_SZ, ACT_LINE);

		// For proper rendering.
		(((ACT_LINE_I + INDEX) < Ui.text_y) && (ACT_LINE_I != Buff->lines_i))
		? WRAP_LINE() : 0;
	}
}

void fit_lines(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t line_i;

	for(line_i = 0; line_i < ACT_LINE_I; line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		(Buff->line_len_i[line_i] > Ui.text_x) ? WRAP_LINE() : 0;
	}
	print_line_num(ACT_LINE_I, Ui.line_num_len, LAST_RENDERED_LINE);
	print_actual_line(Buff, Ui);

	if(CURSOR_Y_SCROLLED)
	{
		for(line_i = ACT_LINE_I + INDEX; line_i < Buff->lines_i; line_i++)
		{
			print_line_num(line_i, Ui.line_num_len, ANOTHER_LINE);
			printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

			(Buff->line_len_i[line_i] > Ui.text_x) ? WRAP_LINE() : 0;
		}
		print_line_num(Buff->lines_i, Ui.line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui.text_x - CUR_SZ, LAST_LINE);
	}
}

void shrink_lines(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t line_i;

	// Previous lines. If scrolled. Only beginning is shown.
	for(line_i = 0; line_i < ACT_LINE_I; line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		(Buff->line_len_i[line_i] > Ui.text_x) ? WRAP_LINE() : 0;
	}
	print_line_num(ACT_LINE_I, Ui.line_num_len, LAST_RENDERED_LINE);
	print_actual_line(Buff, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	line_i = ACT_LINE_I + INDEX;

	for(; line_i < (buff_t) (Ui.text_y - INDEX); line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		(Buff->line_len_i[line_i] > Ui.text_x) ? WRAP_LINE() : 0;
	}
	print_line_num((buff_t) (Ui.text_y - INDEX), Ui.line_num_len, ANOTHER_LINE);

	if(Buff->line_len_i[Ui.text_y - INDEX] < Ui.text_x)
	{
		printf("%.*s", Buff->line_len_i[Ui.text_y - INDEX] - CUR_SZ,
		Buff->text[Ui.text_y - INDEX]);
	}
	else
	{
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[Ui.text_y - INDEX]);
	}
}

void scroll_lines(f_mtdt* Buff, win_mtdt Ui)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line_i = set_start_line(Buff, Ui); line_i < ACT_LINE_I; line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		(Buff->line_len_i[line_i] > Ui.text_x) ? WRAP_LINE() : 0;
	}
	print_line_num(ACT_LINE_I, Ui.line_num_len, LAST_RENDERED_LINE);

	// Display the last line without the linefeed.
	if(ACT_LINE_LEN_I < Ui.text_x)
	{
		printf("%.*s",
		(CURSOR_Y_SCROLLED) ? ACT_LINE_LEN_I - LF_SZ : ACT_LINE_LEN_I,
		ACT_LINE);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LINE_LEN_I - Ui.text_x) >= Buff->cusr_x)
	{
		// Text will be scrolled. Not cursor.
		for(buff_t char_i = CURSOR_VERTICAL_I + CUR_SZ - Ui.text_x;
		char_i < CURSOR_VERTICAL_I; char_i++)
		{
			putchar(ACT_LINE[char_i]);
		}
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui.text_x - CUR_SZ, ACT_LINE);
	}
}

void display_text(f_mtdt* Buff, win_mtdt Ui)
{
	if(Buff->lines_i < Ui.text_y)
	{
		fit_lines(Buff, Ui);
	}
	else if((ACT_LINE_I + INDEX) < Ui.text_y)
	{
		shrink_lines(Buff, Ui);
	}
	else
	{
		scroll_lines(Buff, Ui);
	}
}
