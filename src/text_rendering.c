#include "fiflo.h"
#include "text_rendering.h"

void scroll_line_horizontally(f_mtdt* Buff, win_mtdt Ui) // TODO: OMIT THE LF.
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
	putchar(LF);
/*	if(mv_right == 1 && Buff->cusr_y == 0)
	{
		// Text is shifted so the last printable char (LF) isn't rendered.
		putchar(LF);
	}*/
}

buff_t set_start_line(f_mtdt* Buff, win_mtdt Ui)
{
	buff_t scrolled = 0;

	if((Buff->lines - Buff->cusr_y) >= Ui.text_y)
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
	for(buff_t line = 0; line < Buff->lines - Buff->cusr_y; line++)
	{
		print_line_num(line, Ui.line_num_len);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(Buff->lines - Buff->cusr_y, Ui.line_num_len);
	print_actual_line(Buff, Ui);

	if(Buff->cusr_y > 0)
	{
		for(buff_t line = Buff->lines - Buff->cusr_y + 1; line <= Buff->lines; line++)
		{
			print_line_num(line, Ui.line_num_len);
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
	for(buff_t line = 0; line < Buff->lines - Buff->cusr_y; line++)
	{
		print_line_num(line, Ui.line_num_len);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(Buff->lines - Buff->cusr_y, Ui.line_num_len);
	print_actual_line(Buff, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	for(buff_t line = Buff->lines - Buff->cusr_y + 1;
	line < Ui.text_y - 1; line++)
	{
		print_line_num(line, Ui.line_num_len);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(Ui.text_y - 1, Ui.line_num_len);

	printf("%.*s", Buff->line_len[Ui.text_y - 1] - LF_SZ,
	Buff->text[Ui.text_y - 1]);
}

void scroll_lines(f_mtdt* Buff, win_mtdt Ui)
{
	_Bool ignore_lf = 0;

	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line = set_start_line(Buff, Ui);
	line < Buff->lines - Buff->cusr_y; line++)
	{
		print_line_num(line, Ui.line_num_len);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			putchar(LF);
		}
	}
	print_line_num(Buff->lines - Buff->cusr_y, Ui.line_num_len);

	if(Buff->cusr_y > 0)
	{
		/* Lines aren't scrolled so the last will be fully displayed because
		doesn't contain linefeed at the end and won't break the rendering. */
		ignore_lf = 1;
	}
	// Display the last line without the linefeed when.
	printf("%.*s", ACT_LN_LEN - ignore_lf, ACT_LN);
}

void display_text(f_mtdt* Buff, win_mtdt Ui)
{
	// There is an small amount of lines.
	if(Buff->lines < Ui.text_y)
	{
		fit_lines(Buff, Ui);
	}
	/* There is more lines but they are displayed from the first and shrinked to
	the window. */
	else if((Buff->lines + INDEX - Buff->cusr_y) < Ui.text_y)
	{
		shrink_lines(Buff, Ui);
	}
	// Lines starts from defined by user offset and end somewhere further.
	else
	{
		scroll_lines(Buff, Ui);
	}
}

