#include "buffer.h"
#include "window.h"

#include "textprint.h"

static void print_another_line(Buff_t* Buff, Ui_t* Ui, idx_t line_i)
{
	const bool cursor_or_linefeed_sz = 1;

	window.print_line_num(line_i, Ui->line_num_len, ANOTHER_LINE);
	printf("%.*s", Ui->text_x - cursor_or_linefeed_sz, Buff->text[line_i]);

	if(Buff->line_len_i[line_i] >= Ui->text_x)
	{
		WRAP_LINE();
	}
}

static idx_t set_start_line(Buff_t* Buff, Ui_t* Ui)
{
	idx_t scrolled_lines = 0;

	if(ACT_LINE_I >= Ui->text_y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled_lines = Buff->lines_i + INDEX - Ui->text_y - Buff->cusr_y;
	}
	return scrolled_lines;
}

static void scroll_line_horizontally(Buff_t* Buff, Ui_t* Ui)
{
	idx_t char_i = CURSOR_VERTICAL_I + CUR_SZ - Ui->text_x;

	// Text will be scrolled. Not cursor.
	for(; char_i < CURSOR_VERTICAL_I; char_i++)
	{
		putchar(ACT_LINE[char_i]);
	}
	/* Sometimes is needed because the "fill" function renders the smallest
	amount of linefeeds. In other cases the linefeed is provided by the char in
	a line. */
	if(CURSOR_Y_SCROLLED)
	{
		WRAP_LINE();
	}
}

static void print_actual_line(Buff_t* Buff, Ui_t* Ui)
{
	// There is small amount of chars. Horizontal scroll isn't required.
	if(ACT_LINE_LEN_I < Ui->text_x)
	{
		printf("%s", ACT_LINE);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LINE_LEN_I - Ui->text_x) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_horizontally(Buff, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui->text_x - CUR_SZ, ACT_LINE);

		// For proper rendering.
		if(((ACT_LINE_I + INDEX) < Ui->text_y) && (ACT_LINE_I != Buff->lines_i))
		{
			WRAP_LINE();
		}
	}
}

static void fit_lines(Buff_t* Buff, Ui_t* Ui)
{
	idx_t line_i;

	for(line_i = 0; line_i < ACT_LINE_I; line_i++)
	{
		print_another_line(Buff, Ui, line_i);
	}
	window.print_line_num(ACT_LINE_I, Ui->line_num_len, CURRENT_LINE);
	print_actual_line(Buff, Ui);

	if(CURSOR_Y_SCROLLED)
	{
		for(line_i = ACT_LINE_I + INDEX; line_i < Buff->lines_i; line_i++)
		{
			print_another_line(Buff, Ui, line_i);
		}
		window.print_line_num(Buff->lines_i, Ui->line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui->text_x - LF_SZ, LAST_LINE);
	}
}

static void shrink_lines(Buff_t* Buff, Ui_t* Ui)
{
	idx_t last_ln = (idx_t) Ui->text_y - INDEX;
	idx_t line_i;

	// Previous lines. If scrolled. Only beginning is shown.
	for(line_i = 0; line_i < ACT_LINE_I; line_i++)
	{
		print_another_line(Buff, Ui, line_i);
	}
	window.print_line_num(ACT_LINE_I, Ui->line_num_len, CURRENT_LINE);
	print_actual_line(Buff, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	for(line_i = ACT_LINE_I + INDEX; line_i < last_ln; line_i++)
	{
		print_another_line(Buff, Ui, line_i);
	}
	window.print_line_num((last_ln), Ui->line_num_len, ANOTHER_LINE);

	if(Buff->line_len_i[last_ln] < Ui->text_x)
	{
		printf("%.*s", Buff->line_len_i[last_ln] - LF_SZ, Buff->text[last_ln]);
	}
	else
	{
		printf("%.*s", Ui->text_x - LF_SZ, Buff->text[last_ln]);
	}
}

static void scroll_lines(Buff_t* Buff, Ui_t* Ui)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(idx_t line_i = set_start_line(Buff, Ui); line_i < ACT_LINE_I; line_i++)
	{
		print_another_line(Buff, Ui, line_i);
	}

	// Display the last line without the linefeed.
	window.print_line_num(ACT_LINE_I, Ui->line_num_len, CURRENT_LINE);

	if(ACT_LINE_LEN_I < Ui->text_x)
	{
		printf("%.*s",
		(CURSOR_Y_SCROLLED) ? ACT_LINE_LEN_I - LF_SZ : ACT_LINE_LEN_I,
		ACT_LINE);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LINE_LEN_I - Ui->text_x) >= Buff->cusr_x)
	{
		// Text will be scrolled. Not cursor.
		for(idx_t char_i = CURSOR_VERTICAL_I + CUR_SZ - Ui->text_x;
		char_i < CURSOR_VERTICAL_I; char_i++)
		{
			putchar(ACT_LINE[char_i]);
		}
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui->text_x - LF_SZ, ACT_LINE);
	}
}

static void display_text(Buff_t* Buff, Ui_t* Ui)
{
	if(Buff->lines_i < Ui->text_y)
	{
		fit_lines(Buff, Ui);
	}
	else if((ACT_LINE_I + INDEX) < Ui->text_y)
	{
		shrink_lines(Buff, Ui);
	}
	else
	{
		scroll_lines(Buff, Ui);
	}
}

namespace_textprint textprint =
{
	print_another_line,
	set_start_line,
	scroll_line_horizontally,
	print_actual_line,
	fit_lines,
	shrink_lines,
	scroll_lines,
	display_text
};
