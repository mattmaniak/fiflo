#include "buffer.h"
#include "ui.h"
#include "window.h"

term_t get_term_sz(f_mtdt* Buff, char axis)
{
	const int8_t error = -1;
	const bool line_height = 1;
	const term_t w_min = STATUS_MAX + 10; // TODO
	const uint8_t h_min = UBAR_SZ + line_height + TOGGLED_PANE_SZ;
	const term_t sz_max = USHRT_MAX;

	struct winsize term;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &term) == error)
	{
		fputs("Can't get the terminal size.\n", stderr);
		free_buff_exit(Buff, 1);
	}

	// Terminal size check.
	if((term.ws_col < w_min) || (term.ws_row < h_min))
	{
		fprintf(stderr, "Min. terminal size: %dx%d.\n", w_min, h_min);
		free_buff_exit(Buff, 1);
	}
	else if((term.ws_col > sz_max) || (term.ws_row > sz_max))
	{
		fprintf(stderr, "Max. terminal size: %dx%d.\n", sz_max, sz_max);
		free_buff_exit(Buff, 1);
	}

	switch(axis)
	{
		case 'X': return term.ws_col;
		case 'Y': return term.ws_row;
	}
	return 1;
}

void flush_window(f_mtdt* Buff)
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

void upper_bar(f_mtdt* Buff, win_mtdt Ui)
{
	term_t fname_max = (term_t) (get_term_sz(Buff, 'X') - HALF_ICON_LEN);

	buff_t indicator_width = (buff_t) (get_term_sz(Buff, 'X') - (2 * SPACE_SZ)
	- HALF_ICON_LEN - STATUS_MAX);

	ANSI_INVERT();

	/* Sometimes the empty space of width Ui.line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r%s", HALF_ICON);

	if(Buff->fname_len_i < fname_max)
	{
		// Whole filename will be displayed.
		printf("%s%*s", Buff->fname, get_term_sz(Buff, 'X')
		- HALF_ICON_LEN - Buff->fname_len_i, " ");

		WRAP_LINE();
	}
	else
	{
		for(term_t char_i = (term_t) Buff->fname_len_i - get_term_sz(Buff, 'X') + CUR_SZ
		+ HALF_ICON_LEN; char_i < Buff->fname_len_i; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		putchar(' ');
		WRAP_LINE();
	}

	// The lower part with the "chars in the current line" indicator.
	printf("%s%.*s%*s", HALF_ICON, STATUS_MAX, Buff->status,
	(int) (STATUS_MAX - strlen(Buff->status)), " ");

	if((ACT_LINE_LEN_I < Ui.text_x) || (CURSOR_VERTICAL_I < Ui.text_x))
	{
		printf("%*d^ ", indicator_width,
		get_term_sz(Buff, 'X') - Ui.line_num_len - CUR_SZ);
	}
	else
	{
		printf("%*d^ ", indicator_width, CURSOR_VERTICAL_I);
	}
	WRAP_LINE();
	ANSI_RESET();
}

void lower_bar(f_mtdt* Buff)
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

void fill(f_mtdt* Buff, win_mtdt Ui)
{
	if(Buff->lines_i < (buff_t) (Ui.text_y - LBAR_SZ))
	{
		WRAP_LINE();
		ANSI_INVERT();

		// Fill the empty area below the text to position the lower bar.
		for(buff_t line = Buff->lines_i + INDEX;
		line < (buff_t) (Ui.text_y - LBAR_SZ); line++)
		{
			// Just empty line num block but without the number.
			printf("%*s", Ui.line_num_len - SPACE_SZ, " ");
			WRAP_LINE();
		}
		printf("%*s", Ui.line_num_len - SPACE_SZ, " ");
		ANSI_RESET();
	}
	// Else the lower bar will by positioned by the text.
}

void render_window(f_mtdt* Buff) // TODO: INITIALIZER
{
	win_mtdt Ui;

	// Snprinf isn't needed because the format specifier gives a warning.
	sprintf(Ui.line_num_str, "%u", Buff->lines_i + INDEX);

	Ui.lbar_h = (Buff->pane_toggled) ? TOGGLED_PANE_SZ : LBAR_SZ;

	Ui.line_num_len = (uint8_t) (strlen(Ui.line_num_str) + SPACE_SZ);
	Ui.text_x = (term_t) (get_term_sz(Buff, 'X') - Ui.line_num_len);
	Ui.text_y = (term_t) (get_term_sz(Buff, 'Y') - UBAR_SZ - Ui.lbar_h);

	ANSI_RESET();

	upper_bar(Buff, Ui);

	display_text(Buff, Ui);
	fill(Buff, Ui);

	lower_bar(Buff);

	set_cursor_pos(Buff, Ui);
}

void print_line_num(buff_t line_i, uint8_t line_num_len, const bool act_line)
{
	if(!act_line)
	{
		// Higlight a current line.
		ANSI_INVERT();
	}
	printf("%*d", line_num_len - SPACE_SZ, line_i + INDEX);

	ANSI_RESET();

	// Padding.
	putchar(' ');
}

void set_cursor_pos(f_mtdt* Buff, win_mtdt Ui)
{
	term_t move_up = 0;
	term_t move_right = 0;

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT(get_term_sz(Buff, 'X'));
	ANSI_SAVE_CUR_POS();

	if(Buff->live_fname_edit)
	{
		move_up = (term_t) (get_term_sz(Buff, 'Y') - LBAR_SZ);
		move_right = (term_t) (Buff->fname_len_i + HALF_ICON_LEN);
	}
	else
	{
		if(ACT_LINE_LEN_I < Ui.text_x)
		{
			// No horizontal scrolling.
			move_right = (term_t) (Ui.line_num_len + CURSOR_VERTICAL_I);
		}
		else if((ACT_LINE_LEN_I - Ui.text_x) >= Buff->cusr_x)
		{
			// Last Ui.text_x chars are seen. Current line is scrolled, not cursor.
			move_right = (term_t) (get_term_sz(Buff, 'X') - CUR_SZ);
		}
		else
		{
			// Text is scrolled horizontally to the start. Cursor can be moved.
			move_right = (term_t) (Ui.line_num_len + CURSOR_VERTICAL_I);
		}
		move_up = (ACT_LINE_I < Ui.text_y) ?
		(term_t) (Ui.text_y - ACT_LINE_I) : LBAR_SZ;

		(Buff->pane_toggled) ? move_up += (TOGGLED_PANE_SZ - LBAR_SZ) : 0;
	}
	ANSI_CUR_RIGHT(move_right);
	ANSI_CUR_UP(move_up);
}
