#include "buffer.h"
#include "config.h"
#include "ui.h"
#include "print.h"

void print__line_with_tabs(Buff_t* Buffer, idx_t line_idx,
                           idx_t start_chars_idx, idx_t end_char_idx)
{
	for(idx_t char_idx = start_chars_idx; char_idx < end_char_idx; char_idx++)
	{
		if(Buffer->text[line_idx][char_idx] == '\t')
		{
			putchar(' ');
		}
		else
		{
			putchar(Buffer->text[line_idx][char_idx]);
		}
	}
}

idx_t print__set_start_line(Buff_t* Buffer, Ui_t* Ui)
{
	idx_t scrolled_lines = 0;

	if(CURRENT_LINE_IDX >=  Ui->text_y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled_lines = Buffer->lines_idx + IDX - Ui->text_y
		                 - Buffer->cursor_rev_y;
	}
	return scrolled_lines;
}

void print__actual_line(Buff_t* Buffer, Ui_t* Ui) // TODO: SIMPLIFY.
{
	// There is small amount of chars. Horizontal scroll isn't required.
	if(CURRENT_LINE_LENGTH_IDX < Ui->text_x)
	{
		print__line_with_tabs(Buffer, CURRENT_LINE_IDX, 0,
		                      CURRENT_LINE_LENGTH_IDX);
		// printf("DEBUG");
	}
	// Chars won't fits in the horizontal space.
	else if((CURRENT_LINE_LENGTH_IDX - Ui->text_x) >= Buffer->cursor_rev_x)
	{
		// Render only right part of the line.
		print__scroll_line_horizontally(Buffer, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		if(Buffer->lines_length_idx[Buffer->lines_idx] < Ui->text_x)
		{
			print__line_with_tabs(Buffer, CURRENT_LINE_IDX, 0,
			                      CURRENT_LINE_LENGTH_IDX);
		}
		else
		{
			print__line_with_tabs(Buffer, CURRENT_LINE_IDX, 0,
			                      (idx_t) Ui->text_x - LF_SZ);
		}

		// print__line_with_tabs(Buffer, CURRENT_LINE_IDX, 0,
		//                       CURRENT_LINE_LENGTH_IDX);

		// For proper rendering.
		if(((CURRENT_LINE_IDX + IDX) < Ui->text_y)
		   && (CURRENT_LINE_IDX != Buffer->lines_idx))
		{
			WRAP_LINE();
		}
	}
}

void print__another_line(Buff_t* Buffer, Ui_t* Ui, Conf_t* Config,
                         idx_t line_idx)
{
	ui__print_line_number(Buffer, Config, line_idx, Ui->line_num_len);

	if(Buffer->lines_length_idx[line_idx] < Ui->text_x)
	{
		print__line_with_tabs(Buffer, line_idx, 0,
		                      Buffer->lines_length_idx[line_idx]);
	}
	else
	{
		print__line_with_tabs(Buffer, line_idx, 0, (idx_t) Ui->text_x - LF_SZ);
	}
	// print__line_with_tabs(Buffer, line_idx, 0,
	//                       Buffer->lines_length_idx[line_idx]);

	if(Buffer->lines_length_idx[line_idx] >= Ui->text_x)
	{
		WRAP_LINE();
	}
}

void print__scroll_line_horizontally(Buff_t* Buffer, Ui_t* Ui)
{
	// Text will be scrolled. Not cursor.
	print__line_with_tabs(Buffer, CURRENT_LINE_IDX,
	                      CURSOR_X_POS + CUR_SZ - Ui->text_x,
                          CURSOR_X_POS);

	/* Sometimes is needed because the "fill" function renders the smallest
	amount of linefeeds. In other cases the linefeed is provided by the char in
	a line. */
	if(CURSOR_Y_SCROLLED)
	{
		WRAP_LINE();
	}
}

void print__fit_lines(Buff_t* Buffer, Ui_t* Ui, Conf_t* Config)
{
	const idx_t actual_line_size = 1;
	idx_t       line_i;

	for(line_i = 0; line_i < CURRENT_LINE_IDX; line_i++)
	{
		print__another_line(Buffer, Ui, Config, line_i);
	}
	ui__print_line_number(Buffer, Config, CURRENT_LINE_IDX, Ui->line_num_len);
	print__actual_line(Buffer, Ui);

	if(CURSOR_Y_SCROLLED)
	{
		for(line_i += actual_line_size; line_i < Buffer->lines_idx; line_i++)
		{
			print__another_line(Buffer, Ui, Config, line_i);
		}
		ui__print_line_number(Buffer, Config, Buffer->lines_idx,
		                      Ui->line_num_len);


		if(Buffer->lines_length_idx[Buffer->lines_idx] < Ui->text_x)
		{
			print__line_with_tabs(Buffer, Buffer->lines_idx, 0,
			                      Buffer->lines_length_idx[Buffer->lines_idx]);
		}
		else
		{
			print__line_with_tabs(Buffer, Buffer->lines_idx, 0,
			                      (idx_t) Ui->text_x - LF_SZ);
		}
	}
}

void print__shrink_lines(Buff_t* Buffer, Ui_t* Ui, Conf_t* Config)
{
	idx_t last_line_idx = (idx_t) Ui->text_y - IDX;
	idx_t line_idx      = 0;

	// Previous lines. If scrolled. Only beginning is shown.
	for(; line_idx < CURRENT_LINE_IDX; line_idx++)
	{
		print__another_line(Buffer, Ui, Config, line_idx);
	}
	ui__print_line_number(Buffer, Config, CURRENT_LINE_IDX, Ui->line_num_len);
	print__actual_line(Buffer, Ui);

	// Next lines. If scrolled. Only beginning is shown.
	for(line_idx = CURRENT_LINE_IDX + IDX; line_idx < last_line_idx; line_idx++)
	{
		print__another_line(Buffer, Ui, Config, line_idx);
	}
	ui__print_line_number(Buffer, Config, last_line_idx, Ui->line_num_len);

	if(Buffer->lines_length_idx[last_line_idx] < Ui->text_x)
	{
		print__line_with_tabs(Buffer, last_line_idx, 0,
		                      Buffer->lines_length_idx[last_line_idx] - LF_SZ);
	}
	else
	{
		print__line_with_tabs(Buffer, last_line_idx, 0,
		                      (idx_t )Ui->text_x - LF_SZ);
	}
}

void print__scroll_lines(Buff_t* Buffer, Ui_t* Ui, Conf_t* Config)
{
	idx_t chars_end_offset = CURRENT_LINE_LENGTH_IDX;

	// Previous lines. If scrolled. Only beginning is shown.
	for(idx_t line_idx = print__set_start_line(Buffer, Ui);
	    line_idx < CURRENT_LINE_IDX; line_idx++)
	{
		print__another_line(Buffer, Ui, Config, line_idx);
	}

	// Display the last line without the linefeed.
	ui__print_line_number(Buffer, Config, CURRENT_LINE_IDX, Ui->line_num_len);

	if(CURRENT_LINE_LENGTH_IDX < Ui->text_x)
	{
		if((CURRENT_LINE_LENGTH_IDX > 0)
		&& (CURRENT_LINE[CURRENT_LINE_LENGTH_IDX - NUL_SZ] == '\n'))
		{
			chars_end_offset--;
		}
		print__line_with_tabs(Buffer, CURRENT_LINE_IDX, 0, chars_end_offset);

	}
	// Chars won't fits in the horizontal space.
	else if((CURRENT_LINE_LENGTH_IDX - Ui->text_x) >= Buffer->cursor_rev_x)
	{
		// Text will be scrolled. Not cursor.
		print__line_with_tabs(Buffer, CURRENT_LINE_IDX,
		                      CURSOR_X_POS + CUR_SZ - Ui->text_x,
		                      CURSOR_X_POS);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		print__line_with_tabs(Buffer, CURRENT_LINE_IDX, 0,
		                      (idx_t) Ui->text_x - LF_SZ);
		// printf("%.*s", Ui->text_x - LF_SZ, CURRENT_LINE);
	}
}

void print__display_text(Buff_t* Buffer, Ui_t* Ui, Conf_t* Config)
{
	if(Buffer->lines_idx < Ui->text_y)
	{
		print__fit_lines(Buffer, Ui, Config);
	}
	else if((CURRENT_LINE_IDX + IDX) < Ui->text_y)
	{
		print__shrink_lines(Buffer, Ui, Config);
	}
	else
	{
		print__scroll_lines(Buffer, Ui, Config);
	}
}
