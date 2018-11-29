#include "fiflo.h"
#include "text_rendering.h"

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
	buff_t text_offset = ACT_LINE_LEN_I - Buff->cusr_x;
	buff_t char_i      = text_offset + CUR_SZ - Ui.text_x;

	// Text will be scrolled. Not cursor.
	for(; char_i < text_offset; char_i++)
	{
		putchar(ACT_LINE[char_i]);
	}
	/* Sometimes is needed because the "fill" function renders the smallest
	amount of linefeeds. In other cases the linefeed is provided by the char in
	a line. */
	if(Buff->cusr_y > 0)
	{
		putchar(LF);
	}
}

void print_actual_line(f_mtdt* Buff, win_mtdt Ui, const bool mode)
{
	if(ACT_LINE_LEN_I < Ui.text_x)
	{
		// There is small amount of chars. Horizontal scroll isn't required.
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
		if(mode == LAST_RENDERED_LINE)
		{
			putchar(LF);
		}
	}
}

void fit_lines(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t line_i = 0;

	for(; line_i < ACT_LINE_I; line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, THIN_LINE_NUM);

		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		if(Buff->line_len_i[line_i] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(ACT_LINE_I, Ui.line_num_len, BOLD_LINE_NUM);

	print_actual_line(Buff, Ui, ANOTHER_RENDERED_LINE);

	if(Buff->cusr_y > 0)
	{
		for(line_i = ACT_LINE_I + INDEX; line_i <= Buff->lines_i; line_i++)
		{
			print_line_num(line_i, Ui.line_num_len, THIN_LINE_NUM);
			printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

			if(Buff->line_len_i[line_i] > Ui.text_x)
			{
				/* Just because there is place for the cursor and LF isn't
				printed. */
				putchar(LF);
			}
		}
	}
}

void shrink_lines(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t line_i = 0;

	// Previous lines. If scrolled. Only beginning is shown.
	for(; line_i < ACT_LINE_I; line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, THIN_LINE_NUM);

		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		if(Buff->line_len_i[line_i] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(ACT_LINE_I, Ui.line_num_len, BOLD_LINE_NUM);

	print_actual_line(Buff, Ui, LAST_RENDERED_LINE);

	// Next lines. If scrolled. Only beginning is shown.
	line_i = ACT_LINE_I + INDEX;
	for(; line_i < (buff_t) (Ui.text_y - INDEX); line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, THIN_LINE_NUM);

		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		if(Buff->line_len_i[line_i] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(Ui.text_y - INDEX, Ui.line_num_len, THIN_LINE_NUM);

	printf("%.*s", Buff->line_len_i[Ui.text_y - INDEX] - LF_SZ,
	Buff->text[Ui.text_y - INDEX]);
}

void scroll_lines(f_mtdt* Buff, win_mtdt Ui)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line_i = set_start_line(Buff, Ui);
	line_i < ACT_LINE_I; line_i++)
	{
		print_line_num(line_i, Ui.line_num_len, THIN_LINE_NUM);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line_i]);

		if(Buff->line_len_i[line_i] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(ACT_LINE_I, Ui.line_num_len, BOLD_LINE_NUM);

	// Display the last line without the linefeed.
	if(ACT_LINE_LEN_I < Ui.text_x)
	{
		if(Buff->cusr_y == 0)
		{
			printf("%s", ACT_LINE);
		}
		else
		{
			// There is small amount of chars. Horizontal scroll isn't required.
			printf("%.*s", ACT_LINE_LEN_I - LF_SZ, ACT_LINE);
		}
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
