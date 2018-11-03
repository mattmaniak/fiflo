#include "fiflo.h"
#include "render.h"

term_t get_term_sz(char axis, meta* Dt)
{
	const _Bool single_line_y_sz = 1;
	const uint8_t y_min = BARS_SZ + single_line_y_sz;
	const term_t sz_max = USHRT_MAX;

	struct winsize term;

	/* TIOCGWINSZ request to the stdout descriptor. &term is required by that
	specific device (stdout). */
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &term) == -1)
	{
		fputs("Can't get the terminal size, exit(1).\n", stderr);
		free_all_exit(1, Dt);
	}

	// Terminal size check.
	if(term.ws_col < TERM_X_MIN || term.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exit(1).\n", TERM_X_MIN, y_min);
		free_all_exit(1, Dt);
	}
	else if(term.ws_col > sz_max || term.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		free_all_exit(1, Dt);
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

void flush_window(meta* Dt)
{
	// Restore to the left lower corner and clean the lowest line.
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LN();

	// Then from move up and clean the next lines till the window ends.
	for(term_t y = 0; y < get_term_sz('Y', Dt) - LBAR_SZ; y++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(meta* Dt)
{
	const char* title     = "fiflo: \0";
	const char* dots      = "[...]\0";
	const char* separator = "; \0";

	term_t limiters =
	(term_t) (strlen(title) + strlen(Dt->status) + strlen(separator));

	// Maximum length of a filename that can be fully displayed.
	term_t fname_max = get_term_sz('X', Dt) - (term_t) strlen(dots) - limiters;

	ANSI_BOLD();

	/* Sometimes the empty space of width STRLEN_BUF_T will rendered before the
	upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program
	CR is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r%s", title);

	if(strlen(Dt->fname) <= fname_max)
	{
		// Whitespace after a filename.
		term_t whitespace = get_term_sz('X', Dt)
		- (term_t) strlen(Dt->fname) - limiters;

		// Whole filename will be displayed.
		printf("%s%s%s%*s\n",
		Dt->fname, separator, Dt->status, whitespace, " ");
	}
	else
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s%s%s\n",
		fname_max, Dt->fname, dots, separator, Dt->status);
	}
	ANSI_RESET();
}

void scroll_line_x(meta* Dt)
{
	_Bool mv_right = 0;

	if(ACT_LN[ACT_LN_LEN - NUL_SZ] == LF)
	{
		// Shifts the line right because the linefeed is also rendered.
		mv_right = 1;
	}
	buf_t text_offset = ACT_LN_LEN - Dt->cusr_x - mv_right;

	// Text will be scrolled. Not cursor.
	for (buf_t x = text_offset + CUR_SZ - TXT_X; x <= text_offset - CUR_SZ; x++)
	{
		putchar(ACT_LN[x]);
	}
	if(mv_right == 1)
	{
		// Text is shifted so the last printable char (LF) isn't rendered.
		putchar(LF);
	}
}

buf_t scroll_lines(meta* Dt)
{
	buf_t scrolled = 0;

	if(Dt->lines >= TXT_Y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Dt->lines + INDEX - TXT_Y;
	}
	return scrolled;
}

void print_line_num(buf_t line)
{
	_Bool space_sz = 1;

	ANSI_BOLD();
	printf("%*d", STRLEN_BUF_T - space_sz, line + INDEX);

	ANSI_RESET();
	putchar(' ');
}

void display_text(meta* Dt)
{
	// Previous lines. If scrolled. Only beginning is shown.
	for(buf_t line = scroll_lines(Dt); line < Dt->lines; line++)
	{
		print_line_num(line);
		printf("%.*s", TXT_X - CUR_SZ, Dt->text[line]);

		if(Dt->line_len[line] > TXT_X)
		{
			// Just because there is place for the cursor and LF isn't printed.
			puts(" ");
		}

	}
	print_line_num(Dt->lines);

	// Current line. Can be scrolled etc.
	if(ACT_LN_LEN < TXT_X)
	{
		// There is small amount of chars. X-scroll isn't required.
		printf("%s", ACT_LN);
	}
	// Chars won't fits in the horizontal space.
	else if((ACT_LN_LEN - TXT_X) >= Dt->cusr_x)
	{
		// Render only right part of the line.
		scroll_line_x(Dt);
	}
	else
	{
		// Render only left part of the line. Cursor can scrolled.
		printf("%.*s", TXT_X - CUR_SZ, ACT_LN);
	}
}

void fill(meta* Dt)
{
	if(Dt->lines < TXT_Y)
	{
		// Fill the empty area below the text to position the lower bar.
		for(buf_t line = Dt->lines + INDEX; line < TXT_Y; line++)
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

void window(meta* Dt)
{
	upper_bar(Dt);

	display_text(Dt);
	fill(Dt);

	lower_bar();
	set_cur_pos(Dt);
}

void set_cur_pos(meta* Dt)
{
	// Case when all lines fits in the window.
	buf_t mv_up = TXT_Y - (Dt->lines + INDEX);

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT((term_t) strlen(LBAR_STR));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(ACT_LN_LEN < TXT_X)
	{
		// No horizontal scrolling.
		ANSI_CUR_RIGHT(STRLEN_BUF_T + ACT_LN_LEN - Dt->cusr_x);
	}
	else if((ACT_LN_LEN - TXT_X) >= Dt->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		ANSI_CUR_RIGHT(get_term_sz('X', Dt) - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		ANSI_CUR_RIGHT(STRLEN_BUF_T + ACT_LN_LEN - Dt->cusr_x);
	}

	if(Dt->lines >= TXT_Y)
	{
		// Scrolled so cursor is moved only 1 line above.
		mv_up = 0;
	}
	ANSI_CUR_UP(LBAR_SZ + mv_up);
}

