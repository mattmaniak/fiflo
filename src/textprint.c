#include "buffer.h"
#include "ui.h"
#include "textprint.h"

void textprint_print_another_line(Buff_t* Buff, Ui_t* Ui, idx_t line_i)
{
	const size_t cursor_or_linefeed_sz = 1;

	ui_print_line_number(line_i, Ui->line_num_len, ANOTHER_LINE);
	printf("%.*s", (int) (Ui->text_x - cursor_or_linefeed_sz),
	       Buff->text[line_i]);

	if(Buff->line_len_i[line_i] >= Ui->text_x)
	{
		WRAP_LINE();
	}
}

idx_t textprint_set_start_line(Buff_t* Buff, Ui_t* Ui)
{
	idx_t scrolled_lines = 0;

	if(ACT_LINE_I >= Ui->text_y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled_lines = Buff->lines_i + INDEX - Ui->text_y - Buff->cusr_y;
	}
	return scrolled_lines;
}

void textprint_scroll_line_horizontally(Buff_t* Buff, Ui_t* Ui)
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

void textprint_print_actual_line(Buff_t* Buff, Ui_t* Ui)
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
		textprint_scroll_line_horizontally(Buff, Ui);
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

void textprint_fit_lines(Buff_t* Buff, Ui_t* Ui)
{
	idx_t line_i;

	for(line_i = 0; line_i < ACT_LINE_I; line_i++)
	{
		textprint_print_another_line(Buff, Ui, line_i);
	}
	ui_print_line_number(ACT_LINE_I, Ui->line_num_len, CURRENT_LINE);
	textprint_print_actual_line(Buff, Ui);

	if(CURSOR_Y_SCROLLED)
	{
		for(line_i = ACT_LINE_I + INDEX; line_i < Buff->lines_i; line_i++)
		{
			textprint_print_another_line(Buff, Ui, line_i);
		}
		ui_print_line_number(Buff->lines_i, Ui->line_num_len, ANOTHER_LINE);
		printf("%.*s", Ui->text_x - LF_SZ, LAST_LINE);
	}
}

void textprint_shrink_lines(Buff_t* Buff, Ui_t* Ui)
{
	idx_t last_ln = (idx_t) Ui->text_y - INDEX;
	idx_t line_i;

	// Previous lines. If scrolled. Only beginning is shown.
	for(line_i = 0; line_i < ACT_LINE_I; line_i++)
	{
		textprint_print_another_line(Buff, Ui, line_i);
	}
	ui_print_line_number(ACT_LINE_I, Ui->line_num_len, CURRENT_LINE);
	textprint_print_actual_line(Buff, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	for(line_i = ACT_LINE_I + INDEX; line_i < last_ln; line_i++)
	{
		textprint_print_another_line(Buff, Ui, line_i);
	}
	ui_print_line_number((last_ln), Ui->line_num_len, ANOTHER_LINE);

	if(Buff->line_len_i[last_ln] < Ui->text_x)
	{
		printf("%.*s", Buff->line_len_i[last_ln] - LF_SZ, Buff->text[last_ln]);
	}
	else
	{
		printf("%.*s", Ui->text_x - LF_SZ, Buff->text[last_ln]);
	}
}

void textprint_scroll_lines(Buff_t* Buff, Ui_t* Ui)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(idx_t line_i = textprint_set_start_line(Buff, Ui); line_i < ACT_LINE_I; line_i++)
	{
		textprint_print_another_line(Buff, Ui, line_i);
	}

	// Display the last line without the linefeed.
	ui_print_line_number(ACT_LINE_I, Ui->line_num_len, CURRENT_LINE);

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

void textprint_display_text(Buff_t* Buff, Ui_t* Ui)
{
	if(Buff->lines_i < Ui->text_y)
	{
		textprint_fit_lines(Buff, Ui);
	}
	else if((ACT_LINE_I + INDEX) < Ui->text_y)
	{
		textprint_shrink_lines(Buff, Ui);
	}
	else
	{
		textprint_scroll_lines(Buff, Ui);
	}
}