#include "buffer.h"
#include "window.h"

#include "render.h"

static term_t get_term_sz(Buff_t* Buff, char axis)
{
	const int8_t  error       = -1;
	const bool    line_height = 1;
	const term_t  w_min       = STATUS_MAX + 16; // Generally works but TODO.
	const uint8_t h_min       = UBAR_SZ + line_height + TOGGLED_PANE_SZ;
	const term_t  sz_max      = USHRT_MAX;

	struct winsize terminal;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == error)
	{
		buffer.throw_error(Buff, "Can't get the terminal's size.");
	}

	// Terminal size check.
	if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
	{
		fprintf(stderr, "Minimal terminal size: %ux%u.\n", w_min, h_min);
		buffer.free_all(Buff);
		exit(1);
	}
	else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
	{
		fprintf(stderr, "Maximum terminal size: %ux%u.\n", sz_max, sz_max);
		buffer.free_all(Buff);
		exit(1);
	}

	switch(axis)
	{
		case 'X': return terminal.ws_col;
		case 'Y': return terminal.ws_row;
	}
	return 1;
}

static void flush(Buff_t* Buff)
{
	// Restore to the left lower corner and clean the lowest line.
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LINE();

	// Then from move up and clean the next lines till the window ends.
	for(term_t line = 1; line < get_term_sz(Buff, 'Y'); line++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LINE();
	}
	fflush(stderr);
	fflush(stdout);
}

static void upper_bar(Buff_t* Buff, Ui_t* Ui)
{
	term_t fname_max = (term_t) (get_term_sz(Buff, 'X') - ICON_LEN);
	idx_t indicator_width =
	(idx_t) (get_term_sz(Buff, 'X') - (2*  SPACE_SZ) - ICON_LEN - STATUS_MAX);

	ANSI_INVERT();

	/* Sometimes the empty space of width Ui->line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r%s", ICON);

	if(Buff->fname_len_i < fname_max)
	{
		// Whole filename will be displayed.
		printf("%s%*s", Buff->fname, get_term_sz(Buff, 'X')
		- ICON_LEN - Buff->fname_len_i, " ");

		WRAP_LINE();
	}
	else
	{
		for(term_t char_i = (term_t) (Buff->fname_len_i - get_term_sz(Buff, 'X')
		+ CUR_SZ + ICON_LEN); char_i < Buff->fname_len_i; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		putchar(' ');
		WRAP_LINE();
	}

	// The lower part with the "chars in the current line" indicator.
	printf("%s%.*s%*s", ICON, STATUS_MAX, Buff->status,
	(int) (STATUS_MAX - strlen(Buff->status)), " ");

	if((ACT_LINE_LEN_I < Ui->text_x) || (CURSOR_VERTICAL_I < Ui->text_x))
	{
		printf("%*d^ ", indicator_width,
		get_term_sz(Buff, 'X') - Ui->line_num_len - CUR_SZ);
	}
	else
	{
		printf("%*d^ ", indicator_width, CURSOR_VERTICAL_I);
	}
	WRAP_LINE();
	ANSI_RESET();
}

static void lower_bar(Buff_t* Buff)
{
	term_t horizontal_fill = (term_t) (get_term_sz(Buff, 'X') - strlen(LBAR_STR));
	const char key_binding[4][STATUS_MAX] =
	{
		"CTRL^D - delete a current line",
		"CTRL^O - save as",
		"CTRL^Q - abort changes and exit",
		"CTRL^O - save",
	};
	ANSI_INVERT();
	WRAP_LINE();

	if(Buff->pane_toggled)
	{
		for(uint8_t y = 0; y < TOGGLED_PANE_SZ - LBAR_SZ; y++)
		{
			printf("%s%*s", key_binding[y],
			(int) (get_term_sz(Buff, 'X') - strlen(key_binding[y])), " ");

			WRAP_LINE();
		}
	}
	printf("%s%*s", LBAR_STR, horizontal_fill, " ");
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

static void set_cursor_pos(Buff_t* Buff, Ui_t* Ui)
{
	// Set by default to a filename edit.
	term_t move_up    = (term_t) (get_term_sz(Buff, 'Y') - LBAR_SZ);
	term_t move_right = (term_t) (ICON_LEN + Buff->fname_len_i);

	(move_right >= get_term_sz(Buff, 'X')) ?
	move_right = (term_t) (get_term_sz(Buff, 'X') - CUR_SZ) : 0;

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT(get_term_sz(Buff, 'X'));
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
			move_right = (term_t) (get_term_sz(Buff, 'X') - CUR_SZ);
		}
		else
		{
			// Text is scrolled horizontally to the start. Cursor can be moved.
			move_right = (term_t) (Ui->line_num_len + CURSOR_VERTICAL_I);
		}
		move_up = (ACT_LINE_I < Ui->text_y) ?
		(term_t) ((Ui->text_y - ACT_LINE_I - INDEX) + Ui->lbar_h) : Ui->lbar_h;
	}
	ANSI_CUR_RIGHT(move_right);
	ANSI_CUR_UP(move_up);
}

static void display(Buff_t* Buff)
{
	Ui_t Ui;

	sprintf(Ui.line_num_str, "%u", Buff->lines_i + INDEX);

	Ui.pane_h = TOGGLED_PANE_SZ;
	Ui.lbar_h = (Buff->pane_toggled) ? Ui.pane_h : LBAR_SZ;

	Ui.line_num_len = (uint8_t) (strlen(Ui.line_num_str) + SPACE_SZ);
	Ui.text_x       = (term_t)  (get_term_sz(Buff, 'X') - Ui.line_num_len);
	Ui.text_y       = (term_t)  (get_term_sz(Buff, 'Y') - UBAR_SZ - Ui.lbar_h);

	ANSI_RESET();

	upper_bar(Buff, &Ui);

	render.display_text(Buff, &Ui);
	fill(Buff, &Ui);

	lower_bar(Buff);

	set_cursor_pos(Buff, &Ui);
}

static void print_line_num(idx_t line_i, term_t line_num_len, const bool act_line)
{
	(!act_line) ? ANSI_INVERT() : 0; // Higlight a current line.

	printf("%*d", line_num_len - SPACE_SZ, ++line_i); // Increment the index.

	ANSI_RESET();
	putchar(' '); // Whitespace adding.
}

namespace_window window =
{
	get_term_sz,
	flush,
	upper_bar,
	lower_bar,
	fill,
	display,
	print_line_num,
	set_cursor_pos
};
