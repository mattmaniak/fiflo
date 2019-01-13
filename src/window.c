#include "buffer.h"
#include "ui.h"
#include "window.h"

size_t window_get_terminal_size(char axis)
{
	const int    line_height = 1;
	const size_t w_min       = STATUS_MAX + (2 * SPACE_SZ); // Generally works but TODO.
	const size_t h_min       = UBAR_SZ + line_height + TOGGLED_PANE_SZ;
	const size_t sz_max      = USHRT_MAX;

	struct winsize terminal;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == -1)
	{
		fprintf(stderr, "Can't get the terminal's size.\n");
		return false;
	}

	// Terminal size check. TODO: MAX SIZE.
	if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
	{
		fprintf(stderr, "Minimal terminal's size: %ldx%ld.\n", w_min, h_min);
		return false;
	}
	else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
	{
		fprintf(stderr, "Maximum terminal's size: %ldx%ld.\n", sz_max, sz_max);
		return false;
	}

	switch(axis)
	{
		case 'X':
		return terminal.ws_col;

		case 'Y':
		return terminal.ws_row;
	}
	return false;
}

void window_flush(void)
{
	// Restore to the left lower corner and clean the lowest line.
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LINE();

	// Then from move up and clean the next lines till the window ends.
	for(term_t line = 1; line < window_get_terminal_size('Y'); line++) // TODO
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LINE();
	}
	fflush(NULL);
}

void window_upper_bar(Buff_t* Buff, Ui_t* Ui)
{
	const idx_t punch_card  = 80;
	const char  git_str[]   = "git |\\ ";
	char        punch_card_str[16];

	sprintf(punch_card_str, "%u", punch_card);

	/* Sometimes the empty space of width Ui->line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r ");

	if((ssize_t) (Buff->fname_len_i + SPACE_SZ) < Ui->win_w)
	{
		puts(Buff->fname);
	}
	else
	{
		for(term_t char_i = (term_t) (Buff->fname_len_i - (size_t) (Ui->win_w
		    + CUR_SZ + SPACE_SZ)); char_i < Buff->fname_len_i; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		putchar(' ');
		WRAP_LINE();
	}

	printf(" %s%s\n", git_str, Buff->git_branch);

	// The lower part with the "chars in the current line" indicator.
	printf(" %.*s%*s", STATUS_MAX, Buff->status,
	       (int) (STATUS_MAX - strlen(Buff->status) - SPACE_SZ), " ");

	if(Ui->text_x > punch_card) // TODO: SIMPLIFY.
	{
		printf("%*s", punch_card - STATUS_MAX, " ");

		if((ACT_LINE_LEN_I > punch_card)
		&& (ACT_LINE[ACT_LINE_LEN_I - 1] != '\n')) // TODO: LF AND RED COLOR.
		{
			ANSI_RED();
		}
		else
		{
			ANSI_GREEN();
		}
		printf("%d^", punch_card);
		ANSI_RESET();
	}
	else
	{
		printf("%*s", (int) (Ui->win_w - STATUS_MAX), " ");
	}
	WRAP_LINE();
	ANSI_RESET();
}

void window_lower_bar(const bool pane_toggled)
{
	const char key_binding[8][STATUS_MAX] =
	{
		"CTRL^D - delete line",
		"CTRL^O - save as",
		"CTRL^Q - exit",
		"CTRL^O - save",
		"CTRL^\\ - keyboard shortcuts"
	};
	WRAP_LINE();

	if(pane_toggled)
	{
		for(size_t y = 0; y < TOGGLED_PANE_SZ - LBAR_SZ; y++)
		{
			printf(" %s", key_binding[y]);
			WRAP_LINE();
		}
	}
	printf(" %s", key_binding[TOGGLED_PANE_SZ - LBAR_SZ]);
	ANSI_RESET();
}

void window_fill(Buff_t* Buff, Ui_t* Ui)
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

void window_set_cursor_position(Buff_t* Buff, Ui_t* Ui)
{
	// Set by default to a filename edit.
	term_t move_right = (term_t) (Buff->fname_len_i + SPACE_SZ);
	term_t move_up    = (term_t) (Ui->win_h - LBAR_SZ);

	if(move_right >= Ui->win_w)
	{
		move_right = (term_t) (Ui->win_w - CUR_SZ);
	}

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT(Ui->win_w);
	ANSI_SAVE_CUR_POS();

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
	ANSI_CUR_RIGHT(move_right);
	ANSI_CUR_UP(move_up);
}

bool window_render(Buff_t* Buff)
{
	Ui_t Ui;

	sprintf(Ui.line_num_str, "%u", Buff->lines_i + INDEX);

	if(!(Ui.win_w = window_get_terminal_size('X')))
	{
		return false;
	}
	if(!(Ui.win_h = window_get_terminal_size('Y')))
	{
		return false;
	}

	Ui.pane_h = TOGGLED_PANE_SZ;
	Ui.lbar_h = (Buff->pane_toggled) ? Ui.pane_h : LBAR_SZ;

	Ui.line_num_len = (term_t) (strlen(Ui.line_num_str) + (2 * SPACE_SZ));
	Ui.text_x       = (term_t) (Ui.win_w - (term_t) Ui.line_num_len);
	Ui.text_y       = (term_t) (Ui.win_h - UBAR_SZ - Ui.lbar_h);

	ANSI_RESET();
	window_upper_bar(Buff, &Ui);

	textprint_display_text(Buff, &Ui);
	ANSI_RESET();
	window_fill(Buff, &Ui);
	window_lower_bar(Buff->pane_toggled);

	window_set_cursor_position(Buff, &Ui);

	return true;
}
