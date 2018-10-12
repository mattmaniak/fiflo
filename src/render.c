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
			ANSI_RESET();
			fputs("Check \"termgetsz\" function params, exit(1).\n", stderr);
			exit(1);
	}
}

void flushwin(meta* Dat)
{
	ANSI_RESTORE_CUR_POS();
	ANSI_CLEAN_LN();

	for(term_t y = 0; y < termgetsz('Y', Dat); y++)
	{
		ANSI_CUR_UP(1);
		ANSI_CLEAN_LN();
	}
	fflush(stdout);
}

void ubar(meta* Dat) // TODO: SIMPLIFY, BIGGER VALUES SUPPORT FOR STRLEN_BUF_T.
{
	const char* title = "fiflo: \0";
	const char* dots = "...\0";
	const char* indicators = " line length/chars: \0";

	// Whitespace betweet a filename and indicators.
	term_t space = termgetsz('X', Dat) - (term_t) (strlen(Dat->fname)
	+ strlen(title) + strlen(indicators) + SLASH_SZ + (2 * STRLEN_BUF_T));

	// Maximum length of a filename that can be fully displayed.
	term_t fname_max = termgetsz('X', Dat) - (term_t) (strlen(title)
	+ strlen(dots) + strlen(indicators) + (2 * STRLEN_BUF_T) + SLASH_SZ);

	ANSI_INVERT();

	/* Sometimes the empty space of width STRLEN_BUF_T will rendered before the
	upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. */
	printf("\r%s", title);

	if(strlen(Dat->fname) > fname_max)
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s", fname_max, Dat->fname, dots);
	}
	else
	{
		// Whole filename will be displayed.
		printf("%s%*s", Dat->fname, space, " ");
	}
	printf("%s%*d/%*d\n", indicators, STRLEN_BUF_T, Dat->ln_len[Dat->lns],
	STRLEN_BUF_T, Dat->chrs);
	ANSI_RESET();
}

void xscrolltxt(buf_t ln, meta* Dat)
{
	_Bool mv_right = 0;

	if(Dat->txt[ln][Dat->ln_len[ln] - NTERM_SZ] == LF)
	{
		// Shifts the line right because the linefeed is also rendered.
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

void numln(buf_t ln)
{
	ANSI_INVERT();
	printf("%*d", STRLEN_BUF_T, ln + INDEX);
	ANSI_RESET();	
}

void rendertxt(meta* Dat)
{
	// Vertical rendering.
	for(term_t ln = yscrolltxt(Dat); ln <= Dat->lns; ln++)
	{
		numln(ln);

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
		for(term_t ln = Dat->lns; ln < (TXT_Y - CURRENT); ln++)
		{
			putchar(LF);
		}
	}
	// Else the bar will by positioned by the text.
}

void lbar(void)
{
	ANSI_INVERT();

	printf("\n%s", LBAR_STR);

	ANSI_RESET();
}

void window(meta* Dat)
{
	ubar(Dat);
	rendertxt(Dat);
	fill(Dat);
	lbar();
	setcurpos(Dat);
}

void setcurpos(meta* Dat)
{
	// Case when all lines fits in the window.
	int32_t mv_up = TXT_Y - (Dat->lns + INDEX);

	// Cursor is pushed right by the lower bar. Move it back.
	ANSI_CUR_LEFT((term_t) strlen(LBAR_STR));

	// Lower left side. Will be used to position the cursor and flush each line.
	ANSI_SAVE_CUR_POS();

	if(Dat->ln_len[Dat->lns] < TXT_X)
	{
		// No horizontal scrolling.
		ANSI_CUR_RIGHT((term_t) STRLEN_BUF_T + Dat->ln_len[Dat->lns] - Dat->cusr_x);
	}
	else if((Dat->ln_len[Dat->lns] - TXT_X) >= Dat->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		ANSI_CUR_RIGHT(termgetsz('X', Dat) - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		ANSI_CUR_RIGHT(Dat->ln_len[Dat->lns] - Dat->cusr_x + STRLEN_BUF_T);
	}

	if(Dat->lns >= TXT_Y)
	{
		// Scrolled so cursor is moved only 1 line above.
		mv_up = 0;
	}
	ANSI_CUR_UP(LBAR_SZ + Dat->cusr_y + mv_up);
}

