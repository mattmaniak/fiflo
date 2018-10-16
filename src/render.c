#include "fiflo.h"
#include "render.h"

term_t get_term_sz(char axis, meta* Dt)
{
	const uint8_t y_min = BARS_SZ + CURRENT;
	const term_t sz_max = USHRT_MAX;

	struct winsize term;

	// TODO: COMMENT HOW WORKS.
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &term);

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
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LN();

	for(term_t y = 0; y < get_term_sz('Y', Dt); y++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(meta* Dt) // TODO: SIMPLIFY, BIGGER VALUES SUPPORT FOR STRLEN_BUF_T.
{
	const char* title = "fiflo: \0";
	const char* dots = "...\0";
	const char* indicators = " line length/chars: \0";

	// Whitespace betweet a filename and indicators.
	term_t space = get_term_sz('X', Dt) - (term_t) (strlen(Dt->fname)
	+ strlen(title) + strlen(indicators) + SLASH_SZ + (2 * STRLEN_BUF_T));

	// Maximum length of a filename that can be fully displayed.
	term_t fname_max = get_term_sz('X', Dt) - (term_t) (strlen(title)
	+ strlen(dots) + strlen(indicators) + (2 * STRLEN_BUF_T) + SLASH_SZ);

	ANSI_BOLD();

	/* Sometimes the empty space of width STRLEN_BUF_T will rendered before the
	upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. */
	printf("\r%s", title);

	if(strlen(Dt->fname) > fname_max)
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s", fname_max, Dt->fname, dots);
	}
	else
	{
		// Whole filename will be displayed.
		printf("%s%*s", Dt->fname, space, " ");
	}
	printf("%s%*d/%*d\n", indicators, STRLEN_BUF_T, CURR_LN_LEN,
	STRLEN_BUF_T, Dt->chrs);
	ANSI_RESET();
}

void scroll_chrs_in_ln(buf_t ln, meta* Dt)
{
	_Bool mv_right = 0;

	if(Dt->txt[ln][Dt->ln_len[ln] - NTERM_SZ] == LF)
	{
		// Shifts the line right because the linefeed is also rendered.
		mv_right = 1;
	}

	// Text will be scrolled. Not cursor.
	for(buf_t x = (buf_t) (Dt->ln_len[ln] - Dt->cusr_x - TXT_X
	+ CUR_SZ - mv_right);
	x <= strlen(Dt->txt[ln]) - Dt->cusr_x - CUR_SZ - mv_right; x++)
	{
		putchar(Dt->txt[ln][x]);
	}
	if(mv_right == 1)
	{
		// Text is shifted so LF in Dt->txt isn't rendered.
		putchar(LF);
	}
}

buf_t scroll_lns(meta* Dt)
{
	buf_t scrolled = 0;

	if((Dt->lns + INDEX) > TXT_Y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Dt->lns + INDEX - TXT_Y;
	}
	return scrolled;
}

void print_ln_num(buf_t ln)
{
	_Bool space_sz = 1;

	ANSI_BOLD();
	printf("%*d ", STRLEN_BUF_T - space_sz, ln + INDEX);
	ANSI_RESET();
}

void display_txt(meta* Dt)
{
	// Vertical rendering.
	for(term_t ln = scroll_lns(Dt); ln <= Dt->lns; ln++)
	{
		print_ln_num(ln);

		// Horizontal rendering.
		if((term_t) strlen(Dt->txt[ln]) < TXT_X)
		{
			// There is small amount of chars. X-scroll isn't required.
			printf("%s", Dt->txt[ln]);
		}
		else
		{
			// Chars won't fits in the horizontal space.
			if((CURR_LN_LEN - TXT_X) >= Dt->cusr_x)
			{
				// Render only right part of the line.
				scroll_chrs_in_ln(ln, Dt);
			}
			else
			{
				// Render only left part of the line. Cursor can scrolled.
				printf("%.*s", TXT_X, Dt->txt[ln]);
			}
		}
	}
}

void fill(meta* Dt)
{
	if((Dt->lns + INDEX) <= TXT_Y)
	{
		// Fill the empty area below the text to position the lower bar.
		for(term_t ln = Dt->lns; ln < (TXT_Y - CURRENT); ln++)
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
	display_txt(Dt);
	fill(Dt);
	lower_bar();
	set_cur_pos(Dt);
}

void set_cur_pos(meta* Dt)
{
	// Case when all lines fits in the window.
	int32_t mv_up = TXT_Y - (Dt->lns + INDEX);

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT((term_t) strlen(LBAR_STR));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(CURR_LN_LEN < TXT_X)
	{
		// No horizontal scrolling.
		ANSI_CUR_RIGHT((term_t) STRLEN_BUF_T + CURR_LN_LEN - Dt->cusr_x);
	}
	else if((CURR_LN_LEN - TXT_X) >= Dt->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		ANSI_CUR_RIGHT(get_term_sz('X', Dt) - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		ANSI_CUR_RIGHT(CURR_LN_LEN - Dt->cusr_x + STRLEN_BUF_T);
	}

	if(Dt->lns >= TXT_Y)
	{
		// Scrolled so cursor is moved only 1 line above.
		mv_up = 0;
	}
	ANSI_CUR_UP(LBAR_SZ + mv_up);
}

