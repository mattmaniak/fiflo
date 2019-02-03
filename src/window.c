#include "buffer.h"
#include "config.h"
#include "ui.h"
#include "window.h"

term_t window__get_terminal_sz(char axis)
{
	const int line_height = 1;
	const int w_min       = STATUS_MAX + SPACE_SZ; // Generally works but TODO.
	const int h_min       = UBAR_SZ + line_height + TOGGLED_PANE_SZ;
	const int sz_max      = USHRT_MAX;

	struct winsize terminal;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == ERROR)
	{
		fprintf(stderr, "Can't get the terminal's size.\n");
		return 0;
	}

	// Terminal size check. TODO: MAX SIZE.
	if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
	{
		fprintf(stderr, "Minimal terminal's size: %ux%u.\n", w_min, h_min);
		return 0;
	}
	else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
	{
		fprintf(stderr, "Maximum terminal's size: %ux%u.\n", sz_max, sz_max);
		return 0;
	}

	switch(axis)
	{
		case 'X':
		return terminal.ws_col;

		case 'Y':
		return terminal.ws_row;
	}
	return 0;
}

void window__flush(void)
{
	// Restore to the left lower corner and clean the lowest line.
	ANSI_RESTORE_CURSOR_POS();
	ANSI_CLEAN_WHOLE_LINE();

	// Then from move up and clean the next lines till the window ends.
	for(term_t line = LBAR_SZ; line < window__get_terminal_sz('Y'); line++) // TODO
	{
		ANSI_CURSOR_UP(1);
		ANSI_CLEAN_WHOLE_LINE();
	}
	fflush(NULL);
}

void window__fill(Buff_t* Buffer, Ui_t* Ui)
{
	// Fill the empty area below the text to pos the lower bar.
	if((Buffer->lines_idx + IDX) < (idx_t) Ui->text_y)
	{
		for(idx_t line = Buffer->lines_idx;
		    line < (idx_t) (Ui->text_y - LBAR_SZ); line++)
		{
			WRAP_LINE();
		}
	}
	// Else the lower bar will by posed by the text.
}

void window__set_cursor_pos(Buff_t* Buffer, Ui_t* Ui)
{
	// Set by default to a filename edit.
	term_t move_right = (term_t) (Buffer->fname_length);
	term_t move_up    = (term_t) (Ui->win_h - LBAR_SZ);

	if(move_right >= Ui->win_w)
	{
		move_right = (term_t) (Ui->win_w - CURSOR_SZ);
	}

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CURSOR_LEFT(Ui->win_w);
	ANSI_SAVE_CURSOR_POS();

	if(!Buffer->live_fname_edit)
	{
		if(CURRENT_LINE_LENGTH_IDX < Ui->text_x)
		{
			// No horizontal scrolling.
			move_right = (term_t) (Ui->line_num_len + CURSOR_X);
		}
		else if((CURRENT_LINE_LENGTH_IDX - Ui->text_x) >= Buffer->cursor_rev_x)
		{
			/* Last Ui->text_x chars are seen. Current line is scrolled,
			not cursor. */
			move_right = (term_t) (Ui->win_w - CURSOR_SZ);
		}
		else
		{
			// Text is scrolled horizontally to the start. Cursor can be moved.
			move_right = (term_t) (Ui->line_num_len + CURSOR_X);
		}
		move_up = (CURRENT_LINE_IDX < Ui->text_y) ?
		(term_t) (Ui->text_y - CURRENT_LINE_IDX - IDX + Ui->lbar_h) : Ui->lbar_h;
	}
	ANSI_CURSOR_RIGHT(move_right);
	ANSI_CURSOR_UP(move_up);
}

bool window__render(Buff_t* Buffer, Conf_t* Config)
{
	Ui_t Ui;

	sprintf(Ui.line_num_str, "%u", Buffer->lines_idx + IDX);

	if(!(Ui.win_w = window__get_terminal_sz('X')))
	{
		return false;
	}
	if(!(Ui.win_h = window__get_terminal_sz('Y')))
	{
		return false;
	}

	Ui.pane_h = TOGGLED_PANE_SZ;
	Ui.lbar_h = (Buffer->pane_toggled) ? Ui.pane_h : LBAR_SZ;

	Ui.line_num_len = (term_t) (strlen(Ui.line_num_str) + SPACE_SZ);
	Ui.text_x       = (term_t) (Ui.win_w - Ui.line_num_len);
	Ui.text_y       = (term_t) (Ui.win_h - UBAR_SZ - Ui.lbar_h);

	ui__upper_bar(Buffer, Config, &Ui);
	print__display_text(Buffer, &Ui, Config);

	window__fill(Buffer, &Ui);
	ui__lower_bar(Buffer, Config, &Ui);

	window__set_cursor_pos(Buffer, &Ui);

	return true;
}
