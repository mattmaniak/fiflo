#include "fiflo.h"
#include "render.h"

term_t term_sz(meta* Dat, char axis)
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
		free_all_exit(Dat, 1);
	}
	else if(term.ws_col > sz_max || term.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		free_all_exit(Dat, 1);
	}
	switch(axis)
	{
		case 'X':
			return term.ws_col;

		case 'Y':
			return term.ws_row;

		default:
			A_RESET();
			fputs("Check term_sz function params, exit(1).\n", stderr);
			exit(1);
	}
	// Required -Wreturn-type.
	return 1;
}

void flush_win(meta* Dat)
{
	A_RESTORE_CUR_POS();
	A_CLEAN_LN();

	for(term_t y = 0; y < term_sz(Dat, 'Y'); y++)
	{
		A_CUR_UP(1);
		A_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(meta* Dat) // TODO: SIMPLIFY, BIGGER VALUES FOR STRLEN_BUF_T.
{
	const char* title = "fiflo: \0";
	const char* dots = "...\0";
	const char* indicators = " line length/chars: \0";

	term_t fname_max = term_sz(Dat, 'X') - (term_t) (strlen(title) + strlen(dots)
	+ strlen(indicators) + (2 * STRLEN_BUF_T) + SLASH_SZ);

	A_INVERT_COLORS();
	printf("%s", title);
	if(strlen(Dat->fname) > fname_max)
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s", fname_max, Dat->fname, dots);
	}
	else
	{
		// Whole filename will be displayed.
		printf("%s%*s", Dat->fname,
		term_sz(Dat, 'X') - (term_t) (strlen(Dat->fname) + strlen(title)
		+ strlen(indicators) + SLASH_SZ + (2 * STRLEN_BUF_T)), " ");
	}
	printf("%s%*d/%*d\n", indicators, STRLEN_BUF_T, Dat->ln_len[Dat->lns],
	STRLEN_BUF_T, Dat->chrs);
}

void scroll_txt(meta* Dat, buf_t ln)
{
	_Bool mv_right = 0;
	if(Dat->txt[ln][Dat->ln_len[ln] - NTERM_SZ] == LF)
	{
		mv_right = 1;
	}
	// Text will be scrolled. Not cursor.
	for(buf_t x = (buf_t) (Dat->ln_len[ln] - Dat->cusr_x - TXT_X
	+ CUR_SZ - mv_right);
	x <= strlen(Dat->txt[ln]) - Dat->cusr_x - CUR_SZ - mv_right; x++)
	{
		putchar(Dat->txt[ln][x]);
	}
	if(mv_right == 1)
	{
		// Text is shifted so LF in Dat->txt isn't rendered.
		putchar(LF);
	}
}

void render_txt(meta* Dat)
{
	buf_t scrolled_lns = 0;
	if((Dat->lns + INDEX) > TXT_Y)
	{
		// Horizontal scrolling.
		scrolled_lns = Dat->lns + INDEX - TXT_Y;
	}
	for(term_t ln = scrolled_lns; ln <= Dat->lns; ln++)
	{
		A_INVERT_COLORS();
		printf("%*d", STRLEN_BUF_T, ln + INDEX);
		A_RESET();

		if((term_t) strlen(Dat->txt[ln]) < TXT_X)
		{
			// There is small amount of chars. X-scroll isn't required.
			printf("%s", Dat->txt[ln]);
		}
		else
		{
			// Horizontal scroll required.
			if((Dat->ln_len[Dat->lns] - TXT_X) >= Dat->cusr_x)
			{
				scroll_txt(Dat, ln);
			}
			else
			{
				// Scrolled to start of the line. Now cursor will be scrolled.
				printf("%.*s", TXT_X, Dat->txt[ln]);
			}
		}
	}
}

void fill_l_bar(meta* Dat)
{
	if((Dat->lns + INDEX) <= TXT_Y)
	{
		// Fill the empty area below the text to position lower bar.
		for(term_t ln = Dat->lns; ln < TXT_Y - CURRENT; ln++)
		{
			putchar(LF);
		}
	}
	A_INVERT_COLORS();

	printf("\n%s%*s",
	LBAR_STR, term_sz(Dat, 'X') - TERM_X_MIN + AT_LEAST_CHAR, " ");

	A_RESET();
}

void window(meta* Dat)
{
	upper_bar(Dat);
	render_txt(Dat);
	fill_l_bar(Dat);
	set_cur_pos(Dat);
}

void set_cur_pos(meta* Dat)
{
	// Cursor is moved by default to the right side by lower bar. Move it back.
	A_CUR_LEFT(term_sz(Dat, 'X'));
	// Left bottom corner [0, 0]. Will be used in the flush_win().
	A_SAVE_CUR_POS();

	if(Dat->ln_len[Dat->lns] < TXT_X)
	{
		// No horizontal scrolling.
		A_CUR_RIGHT((term_t) STRLEN_BUF_T + Dat->ln_len[Dat->lns] - Dat->cusr_x);
	}
	else if((Dat->ln_len[Dat->lns] - TXT_X) >= Dat->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		A_CUR_RIGHT(term_sz(Dat, 'X') - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		A_CUR_RIGHT(Dat->ln_len[Dat->lns] - Dat->cusr_x + STRLEN_BUF_T);
	}

	// All lines fits in the window.
	int32_t mv_up = TXT_Y - (Dat->lns + INDEX);
	// Lines are scrolled.
	if(Dat->lns >= TXT_Y)
	{
		mv_up = 0;
	}
	A_CUR_UP(LBAR_SZ + Dat->cusr_y + mv_up);
}

