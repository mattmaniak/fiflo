#include "fiflo.h"
#include "window.h"

term_t get_term_sz(f_mtdt* Buff, char axis)
{
	const int8_t  error     = -1;
	const _Bool   line_y_sz = 1;

	// Remember to not override the upper bar width.
	const term_t  x_min  = (term_t) (strlen(LBAR_STR) + SPACE_SZ);
	const uint8_t y_min  = BARS_SZ + line_y_sz;
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
	for(term_t y = 0; y < (get_term_sz(Buff, 'Y') - LBAR_SZ); y++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(f_mtdt* Buff, win_mtdt Ui)
{
	const char* logo_half = "|`` \0"; // Lower and upper parts are the same.
	const char* dots      = "[...]\0";

	buff_t indicator_width =
	(buff_t) (get_term_sz(Buff, 'X') - (2 * SPACE_SZ)
	- (strlen(logo_half) + strlen(Buff->status)));

	term_t fname_max =
	get_term_sz(Buff, 'X') - (term_t) (strlen(logo_half) + strlen(dots));

	ANSI_INVERT();

	/* Sometimes the empty space of width Ui.line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r%s", logo_half);

	if(strlen(Buff->fname) <= fname_max)
	{
		// Whole filename will be displayed.
		printf("%s%*s\n", Buff->fname, get_term_sz(Buff, 'X')
		- (term_t) (strlen(logo_half) + strlen(Buff->fname)), " ");
	}
	else
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s\n", fname_max, Buff->fname, dots);
	}

	// The lower part with the "chars in the current line" indicator.
	printf("%s%*s", logo_half, (buff_t) strlen(Buff->status), Buff->status);

	if((ACT_LN_LEN < Ui.text_x) || ((ACT_LN_LEN - Buff->cusr_x) < Ui.text_x))
	{
		printf("%*d^ \n", indicator_width,
		get_term_sz(Buff, 'X') - Ui.line_num_len - SPACE_SZ);
	}
	else if((ACT_LN_LEN - Buff->cusr_x) >= Ui.text_x)
	{
		printf("%*d^ \n", indicator_width, ACT_LN_LEN - Buff->cusr_x);
	}
	ANSI_RESET();
}

void lower_bar(f_mtdt* Buff)
{
	ANSI_INVERT();

	printf("\n%s%*s",
	LBAR_STR, (int) (get_term_sz(Buff, 'X') - strlen(LBAR_STR)), " ");

	ANSI_RESET();

}

void fill(f_mtdt* Buff, win_mtdt Ui)
{
	if((Buff->lines + INDEX) < Ui.text_y)
	{
		putchar(LF);
		ANSI_INVERT();

		// Fill the empty area below the text to position the lower bar.
		for(buff_t line = Buff->lines + INDEX + 1; line < Ui.text_y; line++)
		{
			// Just empty line num block but without the number.
			printf("%*s\n", Ui.line_num_len - SPACE_SZ, " ");
		}
		printf("%*s", Ui.line_num_len - SPACE_SZ, " ");
		ANSI_RESET();
	}
	// Else the bar will by positioned by the text.
}

void render_window(f_mtdt* Buff)
{
	win_mtdt Ui;

	// Snprinf isn't needed because the format specifier gives a warning.
	sprintf(Ui.line_num_str, "%u", Buff->lines + INDEX);

	Ui.line_num_len = (uint8_t) strlen(Ui.line_num_str) + SPACE_SZ;
	Ui.text_x       = get_term_sz(Buff, 'X') - Ui.line_num_len;
	Ui.text_y       = get_term_sz(Buff, 'Y') - BARS_SZ;

	ANSI_RESET();

	upper_bar(Buff, Ui);

	display_text(Buff, Ui);

	fill(Buff, Ui);

	lower_bar(Buff);
	set_cur_pos(Buff, Ui);
}

void print_line_num(buff_t line, uint8_t line_num_len, const _Bool mode)
{
	// One with the cursor.
	const _Bool actual_line = 1;

	ANSI_INVERT();

	if(mode == actual_line)
	{
		ANSI_UNDERSCORE();
	}
	printf("%*d", line_num_len - SPACE_SZ, line + INDEX);

	ANSI_RESET();
	putchar(' ');
}

void set_cur_pos(f_mtdt* Buff, win_mtdt Ui)
{
	// Case when all lines fits in the window.
	term_t move_up = 0;

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT(get_term_sz(Buff, 'X'));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(ACT_LN_LEN < Ui.text_x)
	{
		// No horizontal scrolling.
		ANSI_CUR_RIGHT(Ui.line_num_len + ACT_LN_LEN - Buff->cusr_x);
	}
	else if((ACT_LN_LEN - Ui.text_x) >= Buff->cusr_x)
	{
		// Last Ui.text_x chars are seen. Current line is scrolled, not cursor.
		ANSI_CUR_RIGHT(get_term_sz(Buff, 'X') - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		ANSI_CUR_RIGHT(Ui.line_num_len + ACT_LN_LEN - Buff->cusr_x);
	}

	if((Buff->lines - Buff->cusr_y) < Ui.text_y)
	{
		// Scrolled so cursor is moved only 1 line above.
		move_up = Ui.text_y - (term_t) (Buff->lines + INDEX - Buff->cusr_y);
	}
	ANSI_CUR_UP(LBAR_SZ + move_up);
}
