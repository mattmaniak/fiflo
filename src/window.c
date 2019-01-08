#include "buffer.h"
#include "window.h"

#include "textprint.h"

static ssize_t get_terminal_size(char axis)
{
	const bool    line_height = 1;
	const ssize_t w_min       = STATUS_MAX + (2 * SPACE_SZ); // Generally works but TODO.
	const ssize_t h_min       = UBAR_SZ + line_height + TOGGLED_PANE_SZ;
	const ssize_t sz_max      = _POSIX_SSIZE_MAX;

	struct winsize terminal;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == -1)
	{
		fprintf(stderr, "Can't get the terminal's size.\n");
		return -1;
	}

	// Terminal size check. TODO: MAX SIZE.
	if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
	{
		fprintf(stderr, "Minimal terminal's size: %ldx%ld.\n", w_min, h_min);
		return -1;
	}
	else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
	{
		fprintf(stderr, "Maximum terminal's size: %ldx%ld.\n", sz_max, sz_max);
		return -1;
	}

	switch(axis)
	{
		case 'X': return terminal.ws_col;
		case 'Y': return terminal.ws_row;
	}
	return 0;
}

static void flush(void)
{
	// Restore to the left lower corner and clean the lowest line.
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LINE();

	// Then from move up and clean the next lines till the window ends.
	for(term_t line = 1; line < get_terminal_size('Y'); line++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LINE();
	}
	fflush(stderr);
	fflush(stdout);
}

static void upper_bar(Buff_t* Buff, Ui_t* Ui)
{
	const idx_t punch_card  = 80;
	const char  git_str[]   = "git |\\ ";
	const int   git_str_len = 7;
	char        punch_card_str[16];

	sprintf(punch_card_str, "%u", punch_card);

	ANSI_INVERT();

	/* Sometimes the empty space of width Ui->line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r ");

	if((ssize_t) (Buff->fname_len_i + SPACE_SZ) < Ui->win_w)
	{
		// Whole filename will be displayed.
		printf("%s%*s", Buff->fname, (int) (Ui->win_w - (int) Buff->fname_len_i
		       - SPACE_SZ), " ");

		WRAP_LINE();
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

	printf(" %s%s%*s\n", git_str, Buff->git_branch, (int) Ui->win_w
	       - git_str_len - (int) strlen(Buff->git_branch) - SPACE_SZ,  " ");

	// The lower part with the "chars in the current line" indicator.
	printf(" %.*s%*s", STATUS_MAX, Buff->status,
	       (int) (STATUS_MAX - strlen(Buff->status) - SPACE_SZ), " ");

	if(Ui->text_x > punch_card) // TODO: SIMPLIFY.
	{
		printf("%*s%d^%*s", punch_card - STATUS_MAX, " ", punch_card, (int)
		       ((size_t) Ui->win_w - punch_card  - SPACE_SZ
		       - strlen(punch_card_str)), " ");
	}
	else
	{
		printf("%*s", (int) (Ui->win_w - STATUS_MAX), " ");
	}
	WRAP_LINE();
	ANSI_RESET();
}

static void lower_bar(const bool pane_toggled, Ui_t* Ui)
{
	term_t horizontal_fill = (term_t) ((size_t) Ui->win_w - strlen(LBAR_STR)
	- SPACE_SZ);

	const char key_binding[4][STATUS_MAX] =
	{
		"CTRL^D - delete line",
		"CTRL^O - save as",
		"CTRL^Q - exit",
		"CTRL^O - save",
	};
	ANSI_INVERT();
	WRAP_LINE();

	if(pane_toggled)
	{
		for(uint8_t y = 0; y < TOGGLED_PANE_SZ - LBAR_SZ; y++)
		{
			printf(" %s%*s", key_binding[y],
			       (int) ((size_t) Ui->win_w - strlen(key_binding[y])), " ");

			WRAP_LINE();
		}
	}
	printf(" %s%*s", LBAR_STR, horizontal_fill, " ");
	ANSI_RESET();
}

static void fill(Buff_t* Buff, Ui_t* Ui)
{
	if((Buff->lines_i + INDEX) < (idx_t) Ui->text_y)
	{
		WRAP_LINE();
		ANSI_INVERT();

		// Fill the empty area below the text to position the lower bar.
		for(idx_t line = Buff->lines_i + INDEX;
		    line < (idx_t) (Ui->text_y - LBAR_SZ); line++)
		{
			// Just empty line num block but without the number.
			printf("%*s", Ui->line_num_len - SPACE_SZ, " ");
			WRAP_LINE();
		}
		printf("%*s", Ui->line_num_len - SPACE_SZ, " ");
		ANSI_RESET();
	}
	// Else the lower bar will by positioned by the text.
}

static void set_cursor_position(Buff_t* Buff, Ui_t* Ui)
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

static void render(Buff_t* Buff)
{
	Ui_t Ui;

	sprintf(Ui.line_num_str, "%u", Buff->lines_i + INDEX);

	Ui.win_w = get_terminal_size('X');
	Ui.win_h = get_terminal_size('Y');

	Ui.pane_h = TOGGLED_PANE_SZ;
	Ui.lbar_h = (Buff->pane_toggled) ? Ui.pane_h : LBAR_SZ;

	Ui.line_num_len = (term_t) (strlen(Ui.line_num_str) + (2 * SPACE_SZ));
	Ui.text_x       = (term_t) (Ui.win_w - Ui.line_num_len);
	Ui.text_y       = (term_t) (Ui.win_h - UBAR_SZ - Ui.lbar_h);

	ANSI_RESET();

	upper_bar(Buff, &Ui);

	textprint.display_text(Buff, &Ui);
	fill(Buff, &Ui);

	lower_bar(Buff->pane_toggled, &Ui);

	set_cursor_position(Buff, &Ui);
}

static void print_line_num(idx_t line_i, term_t line_num_len, const bool act_line)
{
	if(!act_line) // Higlight a current line.
	{
		ANSI_INVERT();
	}
	printf("%*d", line_num_len - SPACE_SZ, ++line_i); // Increment the index.

	ANSI_RESET();
	putchar(' '); // Whitespace padding.
}

namespace_window window =
{
	get_terminal_size,
	flush,
	upper_bar,
	lower_bar,
	fill,
	render,
	print_line_num,
	set_cursor_position
};
