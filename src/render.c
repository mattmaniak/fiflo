#include "fiflo.h"
#include "render.h"

term_t get_term_sz(char axis, f_mtdt* Buff)
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
		free_all_exit(1, Buff);
	}

	// Terminal size check.
	if(term.ws_col < TERM_X_MIN || term.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exit(1).\n", TERM_X_MIN, y_min);
		free_all_exit(1, Buff);
	}
	else if(term.ws_col > sz_max || term.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		free_all_exit(1, Buff);
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
	for(term_t y = 0; y < (get_term_sz('Y', Buff) - LBAR_SZ); y++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(f_mtdt* Buff)
{
	const char* title    = "fiflo: \0";
	const char* dots     = "[...]\0";
	const _Bool space_sz = 1;

	buff_t indicator_width =
	(buff_t) (get_term_sz('X', Buff) - (2 * space_sz) - strlen(Buff->status));

	term_t fname_max =
	get_term_sz('X', Buff) - (term_t) (strlen(title) + strlen(dots));

	ANSI_BOLD();

	/* Sometimes the empty space of width STRLEN_BUF_T will rendered before the
	upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r%s", title);

	if(strlen(Buff->fname) <= fname_max)
	{
		// Whole filename will be displayed.
		printf("%s\n", Buff->fname);
	}
	else
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s\n", fname_max, Buff->fname, dots);
	}
	printf("%*s", (buff_t) strlen(Buff->status), Buff->status);

	// The lower part with the "chars in the current line" indicator.
	if((ACT_LN_LEN < TXT_X) || ((ACT_LN_LEN - Buff->cusr_x) < TXT_X))
	{
		printf("%*d^ \n", indicator_width,
		get_term_sz('X', Buff) - STRLEN_BUF_T - space_sz);
	}
	else if((ACT_LN_LEN - Buff->cusr_x) >= TXT_X)
	{
		printf("%*d^ \n", indicator_width, ACT_LN_LEN - Buff->cusr_x);
	}
	ANSI_RESET();
}

void scroll_line_x(f_mtdt* Buff)
{
	_Bool mv_right = 0;

	if(ACT_LN[ACT_LN_LEN - NUL_SZ] == LF)
	{
		// Shifts the line right because the linefeed is also rendered.
		mv_right = 1;
	}
	buff_t text_offset = ACT_LN_LEN - Buff->cusr_x - mv_right;

	// Text will be scrolled. Not cursor.
	for (buff_t x = text_offset + CUR_SZ - TXT_X; x < text_offset; x++)
	{
		putchar(ACT_LN[x]);
	}
	if(mv_right == 1)
	{
		// Text is shifted so the last printable char (LF) isn't rendered.
		putchar(LF);
	}
}

buff_t scroll_lines(f_mtdt* Buff)
{
	buff_t scrolled = 0;

	if(Buff->lines >= TXT_Y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Buff->lines + INDEX - TXT_Y;
	}
	return scrolled;
}

void print_line_num(buff_t line)
{
	const _Bool space_sz = 1;

	ANSI_BOLD();
	printf("%*d", STRLEN_BUF_T - space_sz, line + INDEX);

	ANSI_RESET();
	putchar(' ');
}

void display_text(f_mtdt* Buff)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buff_t line = scroll_lines(Buff); line < Buff->lines; line++)
	{
		print_line_num(line);
		printf("%.*s", TXT_X - CUR_SZ, Buff->text[line]);

		if(Buff->line_len[line] > TXT_X)
		{
			// Just because there is place for the cursor and LF isn't printed.
			puts(" ");
		}

	}
	print_line_num(Buff->lines);

	// Current line. Can be scrolled etc.
	if(ACT_LN_LEN < TXT_X)
	{
		// There is small amount of chars. X-scroll isn't required.
		printf("%s", ACT_LN);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LN_LEN - TXT_X) >= Buff->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_x(Buff);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", TXT_X - CUR_SZ, ACT_LN);
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

void lower_bar(void)
{
	ANSI_BOLD();
	printf("\n%s", LBAR_STR);
	ANSI_RESET();
}

void window(f_mtdt* Buff)
{
	ANSI_RESET();

	upper_bar(Buff);

	display_text(Buff);
	fill(Buff);

	lower_bar();
	set_cur_pos(Buff);
}

void set_cur_pos(f_mtdt* Buff)
{
	// Case when all lines fits in the window.
	term_t mv_up = 0;

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT((term_t) strlen(LBAR_STR));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(ACT_LN_LEN < TXT_X)
	{
		// No horizontal scrolling.
		ANSI_CUR_RIGHT(STRLEN_BUF_T + ACT_LN_LEN - Buff->cusr_x);
	}
	else if((ACT_LN_LEN - TXT_X) >= Buff->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		ANSI_CUR_RIGHT(get_term_sz('X', Buff) - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		ANSI_CUR_RIGHT(STRLEN_BUF_T + ACT_LN_LEN - Buff->cusr_x);
	}

	if(Buff->lines < TXT_Y)
	{
		// Scrolled so cursor is moved only 1 line above.
		mv_up = TXT_Y - (Buff->lines + INDEX);
	}
	ANSI_CUR_UP(LBAR_SZ + mv_up);
}

