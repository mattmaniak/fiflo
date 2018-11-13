#include "fiflo.h"
#include "window.h"

term_t get_term_sz(f_mtdt* Buff, char axis)
{
	const _Bool   line_y_sz = 1;
	const uint8_t y_min     = BARS_SZ + line_y_sz;
	const term_t  sz_max    = USHRT_MAX;

	struct winsize term;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &term) == -1)
	{
		fputs("Can't get the terminal size, exit(1).\n", stderr);
		free_all_exit(Buff, 1);
	}

	// Terminal size check.
	if(term.ws_col < TERM_X_MIN || term.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exit(1).\n", TERM_X_MIN, y_min);
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
			return term.ws_col;

		case 'Y':
			return term.ws_row;
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
	LBAR_STR, get_term_sz(Buff, 'X') - TERM_X_MIN + SPACE_SZ, " ");

	ANSI_RESET();

}

void scroll_line_x(f_mtdt* Buff, win_mtdt Ui)
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
	if(mv_right == 1)
	{
		// Text is shifted so the last printable char (LF) isn't rendered.
		putchar(LF);
	}
}

buff_t set_start_line(f_mtdt* Buff)
{
	buff_t scrolled = 0;

	if((Buff->lines - Buff->cusr_y) >= TXT_Y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Buff->lines + INDEX - TXT_Y - Buff->cusr_y;
	}
	return scrolled;
}

buff_t set_end_line(f_mtdt* Buff) // TODO: WHEN LINE INDEX IS 0.
{
	buff_t scrolled = Buff->lines;

	if((Buff->lines - Buff->cusr_y) >= TXT_Y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Buff->lines - Buff->cusr_y;
	}
	return scrolled;
}

void print_line_num(buff_t line, uint8_t line_num_len)
{
	ANSI_INVERT();
	printf("%*d", line_num_len - SPACE_SZ, line + INDEX);

	ANSI_RESET();
	putchar(' ');
}

void display_text(f_mtdt* Buff, win_mtdt Ui) // TODO: CUSR_Y WHEN SCROLL.
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line = set_start_line(Buff);
	line < Buff->lines - Buff->cusr_y; line++)
	{
		print_line_num(line, Ui.line_num_len);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			puts(" ");
		}
	}
	print_line_num(Buff->lines - Buff->cusr_y, Ui.line_num_len);

	// Current line. Can be scrolled etc.
	if(ACT_LN_LEN < Ui.text_x)
	{
		// There is small amount of chars. X-scroll isn't required.
		printf("%s", ACT_LN);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LN_LEN - Ui.text_x) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_x(Buff, Ui);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", Ui.text_x - CUR_SZ, ACT_LN);
	}
	// Next lines. If scrolled. Only beginning is shown.
	for(buff_t line = Buff->lines - Buff->cusr_y + LF_SZ;
	line <= set_end_line(Buff); line++)
	{
		print_line_num(line, Ui.line_num_len);
		printf("%.*s", Ui.text_x - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > Ui.text_x)
		{
			// Just because there is place for the cursor and LF isn't printed.
			puts(" ");
		}
	}
}

void fill(f_mtdt* Buff)
{
	if(Buff->lines < TXT_Y)
	{
		// Fill the empty area below the text to position the lower bar.
		for(buff_t line = Buff->lines + INDEX; line < TXT_Y; line++)
		{
			putchar(LF);
		}
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

	ANSI_RESET();

	upper_bar(Buff, Ui);

	display_text(Buff, Ui);
	fill(Buff);

	lower_bar(Buff);
	set_cur_pos(Buff, Ui);
}

void set_cur_pos(f_mtdt* Buff, win_mtdt Ui)
{
	// Case when all lines fits in the window.
	term_t mv_up     = 0;
	_Bool  ignore_lf = 0;

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT(get_term_sz(Buff, 'X'));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(ACT_LN_LEN < Ui.text_x)
	{
		// When You scroll verticaly the line can have length of 1 (linefeed).
		if(Buff->cusr_y != 0)
		{
			ignore_lf = 1;
		}
		// No horizontal scrolling.
		ANSI_CUR_RIGHT(Ui.line_num_len + ACT_LN_LEN - Buff->cusr_x - ignore_lf);
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

	if(Buff->lines < TXT_Y)
	{
		// Scrolled so cursor is moved only 1 line above.
		mv_up = TXT_Y - (term_t) (Buff->lines + INDEX - Buff->cusr_y);
	}
	ANSI_CUR_UP(LBAR_SZ + mv_up);
}
