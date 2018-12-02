#include "fiflo.h"
#include "window.h"

term_t get_term_sz(f_mtdt* Buff, char axis)
{
	const int8_t error       = -1;
	const bool   line_height = 1;

	// Remember to not override the upper bar width.
	const term_t  x_min  = (term_t) (strlen(LBAR_STR) + SPACE_SZ);
	const uint8_t y_min  = BARS_SZ + line_height;
	const term_t  sz_max = USHRT_MAX;

	struct winsize term;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &term) == error)
	{
		fputs("Can't get the terminal size, exit(1).\n", stderr);
		free_all_exit(Buff, 1);
	}

	// Terminal size check.
	if(term.ws_col < x_min || term.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exit(1).\n", x_min, y_min);
		free_all_exit(Buff, 1);
	}
	else if(term.ws_col > sz_max || term.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		free_all_exit(Buff, 1);
	}

	switch(axis)
	{
		case 'X':
		{
			return term.ws_col;
		}
		case 'Y':
		{
			return term.ws_row;
		}
	}
	return 1;
}

void flush_window(f_mtdt* Buff)
{
	// Restore to the left lower corner and clean the lowest line.
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LN();

	// Then from move up and clean the next lines till the window ends.
	for(term_t line = 0; line < (get_term_sz(Buff, 'Y') - LBAR_SZ); line++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(f_mtdt* Buff, win_mtdt Ui)
{
	const char* half_logo = "|`` \0"; // Lower and upper parts are the same.

	term_t fname_max = get_term_sz(Buff, 'X') - (term_t) strlen(half_logo);

	buff_t indicator_width =
	(buff_t) (get_term_sz(Buff, 'X') - (2 * SPACE_SZ)
	- (strlen(half_logo) + strlen(Buff->status)));

	ANSI_INVERT();

	/* Sometimes the empty space of width Ui.line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r%s", half_logo);

	if(Buff->fname_len < fname_max)
	{
		// Whole filename will be displayed.
		printf("%s%*s\n", Buff->fname, get_term_sz(Buff, 'X')
		- (term_t) (strlen(half_logo) + strlen(Buff->fname)), " ");
	}
	else
	{
		for(term_t char_i = Buff->fname_len - get_term_sz(Buff, 'X') + CUR_SZ
		+ (term_t) (strlen(half_logo)); char_i < Buff->fname_len; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		puts(" ");
	}

	// The lower part with the "chars in the current line" indicator.
	printf("%s%*s", half_logo, (buff_t) strlen(Buff->status), Buff->status);

	if((ACT_LINE_LEN_I < Ui.text_x) || (CURSOR_VERTICAL_I < Ui.text_x))
	{
		printf("%*d' \n", indicator_width,
		get_term_sz(Buff, 'X') - Ui.line_num_len - SPACE_SZ);
	}
	else
	{
		printf("%*d' \n", indicator_width, CURSOR_VERTICAL_I);
	}
	ANSI_RESET();
}

void lower_bar(f_mtdt* Buff)
{
	term_t horizontal_fill = (get_term_sz(Buff, 'X') - strlen(LBAR_STR));

	ANSI_INVERT();
	printf("\n%s%*s", LBAR_STR, horizontal_fill, " ");
	ANSI_RESET();
}

void fill(f_mtdt* Buff, win_mtdt Ui)
{
	if((Buff->lines_i + INDEX) < Ui.text_y)
	{
		putchar(LF);
		ANSI_INVERT();

		// Fill the empty area below the text to position the lower bar.
		for(buff_t line = Buff->lines_i + INDEX + 1; line < Ui.text_y; line++)
		{
			// Just empty line num block but without the number.
			printf("%*s\n", Ui.line_num_len - SPACE_SZ, " ");
		}
		printf("%*s", Ui.line_num_len - SPACE_SZ, " ");
		ANSI_RESET();
	}
	// Else the lower bar will by positioned by the text.
}

void render_window(f_mtdt* Buff)
{
	win_mtdt Ui;

	// Snprinf isn't needed because the format specifier gives a warning.
	sprintf(Ui.line_num_str, "%u", Buff->lines_i + INDEX);

	Ui.line_num_len = (uint8_t) strlen(Ui.line_num_str) + SPACE_SZ;
	Ui.text_x       = get_term_sz(Buff, 'X') - Ui.line_num_len;
	Ui.text_y       = get_term_sz(Buff, 'Y') - BARS_SZ;

	ANSI_RESET();
	upper_bar(Buff, Ui);

	display_text(Buff, Ui);
	fill(Buff, Ui);

	lower_bar(Buff);
	set_cursor_pos(Buff, Ui);
}

void print_line_num(buff_t line_i, uint8_t line_num_len, const bool mode)
{
	// One with the cursor.
	const bool actual_line = true;

	ANSI_INVERT();

	if(mode == actual_line)
	{
		// Higlight the current line.
		ANSI_UNDERSCORE();
	}
	printf("%*d", line_num_len - SPACE_SZ, line_i + INDEX);

	ANSI_RESET();
	putchar(' ');
}

void set_cursor_pos(f_mtdt* Buff, win_mtdt Ui)
{
	term_t move_up = 0;

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT(get_term_sz(Buff, 'X'));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(Buff->live_fname_edit)
	{
		move_up = get_term_sz(Buff, 'Y') - LBAR_SZ;

		ANSI_CUR_RIGHT(Buff->fname_len + SPACE_SZ + 3);
	}
	else
	{
		if(ACT_LINE_LEN_I < Ui.text_x)
		{
			// No horizontal scrolling.
			ANSI_CUR_RIGHT(Ui.line_num_len + CURSOR_VERTICAL_I);
		}
		else if((ACT_LINE_LEN_I - Ui.text_x) >= Buff->cusr_x)
		{
			// Last Ui.text_x chars are seen. Current line is scrolled, not cursor.
			ANSI_CUR_RIGHT(get_term_sz(Buff, 'X') - CUR_SZ);
		}
		else
		{
			// Text is scrolled horizontally to the start. Cursor can be moved.
			ANSI_CUR_RIGHT(Ui.line_num_len + CURSOR_VERTICAL_I);
		}

		if(ACT_LINE_I < Ui.text_y)
		{
			// Scrolled so cursor is moved only 1 line above.
			move_up = Ui.text_y - (term_t) (Buff->lines_i - Buff->cusr_y);
		}
	}
	ANSI_CUR_UP(move_up);
}
