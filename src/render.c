#include "fiflo.h"
#include "render.h"

term_t termgetsz(char axis, meta* Dat)
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
		freeallexit(1, Dat);
	}
	else if(term.ws_col > sz_max || term.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		freeallexit(1, Dat);
	}
	switch(axis)
	{
		case 'X':
			return term.ws_col;

		case 'Y':
			return term.ws_row;

		default:
			A_RESET();
			fputs("Check \"termgetsz\" function params, exit(1).\n", stderr);
			exit(1);
	}
	// Required -Wreturn-type.
	return 0;
}

void flushwin(meta* Dat)
{
	A_RESTORE_CUR_POS();
	A_CLEAN_LN();

	for(term_t y = 0; y < termgetsz('Y', Dat); y++)
	{
		A_CUR_UP(1);
		A_CLEAN_LN();
	}
	fflush(stdout);
}

void ubar(meta* Dat) // TODO: SIMPLIFY, BIGGER VALUES SUPPORT FOR STRLEN_BUF_T.
{
	const char* title = "fiflo: \0";
	const char* dots = "...\0";
	const char* indicators = " line length/chars: \0";

	term_t fname_max = termgetsz('X', Dat) - (term_t) (strlen(title) + strlen(dots)
	+ strlen(indicators) + (2 * STRLEN_BUF_T) + SLASH_SZ);

	A_INVERT_COLORS();
	printf("%s", title);
	if(strlen(Dat->fname) > fname_max)
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s", fname_max, Dat->fname, dots);
	}
	else // TODO: VARIABLE INSTEAD OF THESE CENTEPIDE.
	{
		// Whole filename will be displayed.
		printf("%s%*s", Dat->fname,
		termgetsz('X', Dat) - (term_t) (strlen(Dat->fname) + strlen(title)
		+ strlen(indicators) + SLASH_SZ + (2 * STRLEN_BUF_T)), " ");
	}
	printf("%s%*d/%*d\n", indicators, STRLEN_BUF_T, Dat->ln_len[Dat->lns],
	STRLEN_BUF_T, Dat->chrs);
}

void xscrolltxt(buf_t ln, meta* Dat)
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

buf_t yscrolltxt(meta* Dat)
{
	buf_t scrolled = 0;
	if((Dat->lns + INDEX) > TXT_Y)
	{
		// Amount of lines to hide in the magic upper area.
		scrolled = Dat->lns + INDEX - TXT_Y;
	}
	return scrolled;
}

void rendertxt(meta* Dat)
{
	// Vertical rendering.
	for(term_t ln = yscrolltxt(Dat); ln <= Dat->lns; ln++)
	{
		A_INVERT_COLORS();
		printf("%*d", STRLEN_BUF_T, ln + INDEX);
		A_RESET();

		// Horizontal rendering.
		if((term_t) strlen(Dat->txt[ln]) < TXT_X)
		{
			// There is small amount of chars. X-scroll isn't required.
			printf("%s", Dat->txt[ln]);
		}
		else
		{
			// Chars won't fits in the horizontal space.
			if((Dat->ln_len[Dat->lns] - TXT_X) >= Dat->cusr_x)
			{
				// Render only right part of the line.
				xscrolltxt(ln, Dat);
			}
			else
			{
				// Render only left part of the line. Cursor can scrolled.
				printf("%.*s", TXT_X, Dat->txt[ln]);
			}
		}
	}
}

void fill(meta* Dat)
{
	if((Dat->lns + INDEX) <= TXT_Y)
	{
		// Fill the empty area below the text to position the lower bar.
		for(term_t ln = Dat->lns; ln < TXT_Y - CURRENT; ln++)
		{
			putchar(LF);
		}
	}
	// Else the bar will by positioned by the text.
}

void lbar(meta* Dat)
{
	A_INVERT_COLORS();

	printf("\n%s%*s",
	LBAR_STR, termgetsz('X', Dat) - TERM_X_MIN + AT_LEAST_CHAR, " ");

	A_RESET();
}

void window(meta* Dat)
{
	ubar(Dat);
	rendertxt(Dat);
	fill(Dat);
	lbar(Dat);
	setcurpos(Dat);
}

void setcurpos(meta* Dat)
{
	// Cursor is moved by default to the right side by lower bar. Move it back.
	A_CUR_LEFT(termgetsz('X', Dat));
	// Left bottom corner [0, 0]. Will be used in the flushwin().
	A_SAVE_CUR_POS();

	if(Dat->ln_len[Dat->lns] < TXT_X)
	{
		// No horizontal scrolling.
		A_CUR_RIGHT((term_t) STRLEN_BUF_T + Dat->ln_len[Dat->lns] - Dat->cusr_x);
	}
	else if((Dat->ln_len[Dat->lns] - TXT_X) >= Dat->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		A_CUR_RIGHT(termgetsz('X', Dat) - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		A_CUR_RIGHT(Dat->ln_len[Dat->lns] - Dat->cusr_x + STRLEN_BUF_T);
	}

	// All lines fits in the window.
	int32_t mv_up = TXT_Y - (Dat->lns + INDEX);

	if(Dat->lns >= TXT_Y)
	{
		// Scrolled so cursor is moved only 1 line above.
		mv_up = 0;
	}
	A_CUR_UP(LBAR_SZ + Dat->cusr_y + mv_up);
}

