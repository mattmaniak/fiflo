#include "buffer.h"
#include "config.h"
#include "ui.h"
#include "window.h"

term_t window__get_terminal_size(char axis)
{
	const unsigned int line_height = 1;
	const int          w_min       = STATUS_MAX + SPACE_SZ; // Generally works but TODO.
	const int          h_min       = UBAR_SZ + line_height + TOGGLED_PANE_SZ;
	const int          sz_max      = USHRT_MAX;

	struct winsize terminal;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == -1)
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
	ANSI_RESTORE_CURSOR_POSITION();
	ANSI_CLEAN_WHOLE_LINE();

	// Then from move up and clean the next lines till the window ends.
	for(term_t line = LBAR_SZ; line < window__get_terminal_size('Y'); line++) // TODO
	{
		ANSI_CURSOR_UP(1);
		ANSI_CLEAN_WHOLE_LINE();
	}
	fflush(NULL);
}

void window__fill(Buff_t* Buff, Ui_t* Ui)
{
	// Fill the empty area below the text to position the lower bar.
	if((Buff->lines_i + INDEX) < (idx_t) Ui->text_y)
	{
		for(idx_t line = Buff->lines_i;
		    line < (idx_t) (Ui->text_y - LBAR_SZ); line++)
		{
			WRAP_LINE();
		}
	}
	// Else the lower bar will by positioned by the text.
}

void window__set_cursor_position(Buff_t* Buff, Ui_t* Ui)
{
	// Set by default to a filename edit.
	term_t move_right = (term_t) (Buff->fname_len_i);
	term_t move_up    = (term_t) (Ui->win_h - LBAR_SZ);

	if(move_right >= Ui->win_w)
	{
		move_right = (term_t) (Ui->win_w - CUR_SZ);
	}

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CURSOR_LEFT(Ui->win_w);
	ANSI_SAVE_CURSOR_POSITION();

	if(!Buff->live_fname_edit)
	{
		if(ACT_LINE_LEN_I < Ui->text_x)
		{
			// No horizontal scrolling.
			move_right = (term_t) (Ui->line_num_len + CURSOR_VERTICAL_I);
		}
		else if((ACT_LINE_LEN_I - Ui->text_x) >= Buff->cusr_x)
		{
			// Last Ui->text_x chars are seen. Current line is scrolled, not cursor.
			move_right = (term_t) (Ui->win_w - CUR_SZ);
		}
		else
		{
			// Text is scrolled horizontally to the start. Cursor can be moved.
			move_right = (term_t) (Ui->line_num_len + CURSOR_VERTICAL_I);
		}
		move_up = (ACT_LINE_I < Ui->text_y) ?
		(term_t) (Ui->text_y - ACT_LINE_I - INDEX + Ui->lbar_h) : Ui->lbar_h;
	}
	ANSI_CURSOR_RIGHT(move_right);
	ANSI_CURSOR_UP(move_up);
}

bool window__render(Buff_t* Buff, Conf_t* Config)
{
	Ui_t Ui;

	sprintf(Ui.line_num_str, "%u", Buff->lines_i + INDEX);

	if(!(Ui.win_w = window__get_terminal_size('X')))
	{
		return false;
	}
	if(!(Ui.win_h = window__get_terminal_size('Y')))
	{
		return false;
	}

	Ui.pane_h = TOGGLED_PANE_SZ;
	Ui.lbar_h = (Buff->pane_toggled) ? Ui.pane_h : LBAR_SZ;

	Ui.line_num_len = (term_t) (strlen(Ui.line_num_str) + SPACE_SZ);
	Ui.text_x       = (term_t) (Ui.win_w - Ui.line_num_len);
	Ui.text_y       = (term_t) (Ui.win_h - UBAR_SZ - Ui.lbar_h);

	config__set_color(NULL);
	ui__upper_bar(Buff, Config, &Ui);

	print__display_text(Buff, &Ui, Config);
	config__set_color(NULL);

	window__fill(Buff, &Ui);
	ui__lower_bar(Buff, Config, &Ui);

	window__set_cursor_position(Buff, &Ui);

	return true;
}
