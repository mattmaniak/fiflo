#include "fiflo.h"
#include "text_rendering.h"

void scroll_line_horizontally(f_mtdt* Buff, win_mtdt Ui)
{
	_Bool mv_right = 0;

	if(ACT_LN[ACT_LN_LEN - NUL_SZ] == LF)
	{
		// Shifts the line right because the linefeed is also rendered.
		mv_right = 1;
	}
	buff_t text_offset = ACT_LN_LEN - Buff->cusr_x - mv_right;

	// Text will be scrolled. Not cursor.
	for(buff_t x = text_offset + CUR_SZ - Ui.text_x; x < text_offset; x++)
	{
		putchar(ACT_LN[x]);
	}
	/* Sometimes is needed because the "fill" function renders the smallest
	amount of linefeeds. In other cases the linefeed is provided by the char in
	a line. */
	if(Buff->cusr_y > 0)
	{
		putchar(LF);
	}
}

buff_t set_start_line(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t scrolled = 0;

	if(ACT_LN_INDEX >= Ui.text_y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Buff->lines + INDEX - Ui.text_y - Buff->cusr_y;
	}
	return scrolled;
}

void print_actual_line(f_mtdt* Buff, win_mtdt Ui)
{
	if(ACT_LN_LEN < Ui.text_x)
	{
		// There is small amount of chars. Horizontal scroll isn't required.
		printf("%s", ACT_LN);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LN_LEN - Ui.text_x) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_horizontally(Buff, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s\n", Ui.text_x - CUR_SZ, ACT_LN);
	}
}

void fit_lines(f_mtdt* Buff, win_mtdt Ui)
{
	for(buff_t line = 0; line < ACT_LN_INDEX; line++)
	{
		print_line_num(line, Ui.line_num_len, THIN_LN_NUM);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(ACT_LN_INDEX, Ui.line_num_len, BOLD_LN_NUM);
	print_actual_line(Buff, Ui);

	if(Buff->cusr_y > 0)
	{
		for(buff_t line = ACT_LN_INDEX + 1; line <= Buff->lines; line++)
		{
			print_line_num(line, Ui.line_num_len, THIN_LN_NUM);
			printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

			if(Buff->line_len[line] > Ui.text_x)
			{
				// Just because there is place for the cursor and LF isn't printed.
				putchar(LF);
			}
		}
	}
}

void shrink_lines(f_mtdt* Buff, win_mtdt Ui)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line = 0; line < ACT_LN_INDEX; line++)
	{
		print_line_num(line, Ui.line_num_len, THIN_LN_NUM);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(ACT_LN_INDEX, Ui.line_num_len, BOLD_LN_NUM);
	print_actual_line(Buff, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	for(buff_t line = ACT_LN_INDEX + 1; line < (buff_t) (Ui.text_y - 1); line++)
	{
		print_line_num(line, Ui.line_num_len, THIN_LN_NUM);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(Ui.text_y - 1, Ui.line_num_len, THIN_LN_NUM);

	printf("%.*s", Buff->line_len[Ui.text_y - 1] - LF_SZ,
	Buff->text[Ui.text_y - 1]);
}

void scroll_lines(f_mtdt* Buff, win_mtdt Ui)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line = set_start_line(Buff, Ui);
	line < ACT_LN_INDEX; line++)
	{
		print_line_num(line, Ui.line_num_len, THIN_LN_NUM);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(ACT_LN_INDEX, Ui.line_num_len, BOLD_LN_NUM);

	// Display the last line without the linefeed.
	if(ACT_LN_LEN < Ui.text_x)
	{
		if(Buff->cusr_y == 0)
		{
			printf("%s", ACT_LN);
		}
		else
		{
			// There is small amount of chars. Horizontal scroll isn't required.
			printf("%.*s", ACT_LN_LEN - LF_SZ, ACT_LN);
		}
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LN_LEN - Ui.text_x) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_horizontally(Buff, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui.text_x - CUR_SZ, ACT_LN);
	}
}

void display_text(f_mtdt* Buff, win_mtdt Ui)
{
	// There is an small amount of lines.
	if(Buff->lines < Ui.text_y)
	{
		fit_lines(Buff, Ui);
	}
	// There is more lines but they are displayed from the first one.
	else if((ACT_LN_INDEX + INDEX) < Ui.text_y)
	{
		shrink_lines(Buff, Ui);
	}
	// Lines start from defined by user offset and end somewhere further.
	else
	{
		scroll_lines(Buff, Ui);
	}
}
