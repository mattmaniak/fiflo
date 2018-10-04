#include "render.h"

term_t term_sz(meta* dt, char axis)
{
	const uint8_t y_min = BARS_SZ + CURRENT;
	const term_t sz_max = USHRT_MAX;

	struct winsize win;

	// TODO: COMMENT HOW WORKS.
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	// Terminal size check.
	if(win.ws_col < TERM_X_MIN || win.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exit(1).\n", TERM_X_MIN, y_min);
		free_all_exit(dt, 1);
	}
	else if(win.ws_col > sz_max || win.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		free_all_exit(dt, 1);
	}
	switch(axis)
	{
		case 'x':
			return win.ws_col;

		case 'y':
			return win.ws_row;

		default:
			A_RESET();
			fputs("Check term_sz function params, exit(1).\n", stderr);
			exit(1);
	}
	// Required -Wreturn-type.
	return 1;
}

void flush_win(meta* dt)
{
	A_RESTORE_CUR_POS();
	A_CLEAN_LN();

	for(term_t y = 0; y < term_sz(dt, 'y'); y++)
	{
		A_CUR_UP(1);
		A_CLEAN_LN();
	}
	fflush(stdout);
}

void upper_bar(meta* dt) // TODO: SIMPLIFY, BIGGER VALUES FOR STRLEN_BUF_T.
{
	const char* title = "fiflo: \0";
	const char* dots = "...\0";
	const char* indicators = " line length/chars: \0";

	term_t fname_max = term_sz(dt, 'x') - (term_t) (strlen(title) + strlen(dots)
	+ strlen(indicators) + (2 * STRLEN_BUF_T) + SLASH_SZ);

	A_INVERT_COLORS();
	printf("%s", title);
	if(strlen(dt->fname) > fname_max)
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s", fname_max, dt->fname, dots);
	}
	else
	{
		// Whole filename will be displayed.
		printf("%s%*s", dt->fname,
		term_sz(dt, 'x') - (term_t) (strlen(dt->fname) + strlen(title)
		+ strlen(indicators) + SLASH_SZ + (2 * STRLEN_BUF_T)), " ");
	}
	printf("%s%*d/%*d\n", indicators, STRLEN_BUF_T, dt->ln_len,
	STRLEN_BUF_T, dt->chrs);
}

void render_txt(meta* dt)
{
	buf_t scrolled_lns = 0;
	if((dt->lns + INDEX) > TXT_Y)
	{
		// Horizontal scrolling.
		scrolled_lns = dt->lns + INDEX - TXT_Y;
	}
	for(term_t ln = scrolled_lns; ln <= dt->lns; ln++)
	{
		A_INVERT_COLORS();
		printf("%*d", STRLEN_BUF_T, ln + INDEX);
		A_RESET();

		if((term_t) strlen(dt->txt[ln]) < TXT_X)
		{
			// There is small amount of chars. X-scroll isn't required.
			printf("%s", dt->txt[ln]);
		}
		else
		{
			// Horizontal scroll required.
			if((strlen(dt->txt[ln]) - TXT_X) >= dt->cusr_x)
			{
				_Bool mv_right = 0;
				if(dt->txt[ln][strlen(dt->txt[ln]) - NTERM_SZ] == LF)
				{
					mv_right = 1;
				}
				// Text will be scrolled. Not cursor.
				for(buf_t x = (buf_t) (strlen(dt->txt[ln]) - dt->cusr_x - TXT_X
				+ CUR_SZ - mv_right);
				x <= strlen(dt->txt[ln]) - dt->cusr_x - CUR_SZ - mv_right; x++)
				{
					putchar(dt->txt[ln][x]);
				}
				if(mv_right == 1)
				{
					// Text is shifted so LF in dt->txt isn't rendered.
					putchar(LF);
				}
			}
			else
			{
				// Scrolled to start of the line. Now cursor will be scrolled.
				printf("%.*s", TXT_X, dt->txt[ln]);
			}
		}
	}
}

void fill(meta* dt)
{
	if((dt->lns + INDEX) <= TXT_Y)
	{
		// Fill the empty area below the text to position lower bar.
		for(term_t ln = dt->lns; ln < TXT_Y - CURRENT; ln++)
		{
			putchar(LF);
		}
	}
}

void lower_bar(meta* dt)
{
	A_INVERT_COLORS();
	printf("\n%s%*s",
	LBAR_STR, term_sz(dt, 'x') - TERM_X_MIN + AT_LEAST_CHAR, " ");

	A_RESET();
}

void window(meta* dt)
{
	upper_bar(dt);
	render_txt(dt);
	fill(dt);
	lower_bar(dt);
	set_cursor(dt);
}

void set_cursor(meta* dt)
{
	// Cursor is moved by default to the right side by lower bar. Move it back.
	A_CUR_LEFT(term_sz(dt, 'x'));
	// Left bottom corner [0, 0]. Will be used in the flush_win().
	A_SAVE_CUR_POS();

	if(dt->ln_len < TXT_X)
	{
		// No horizontal scrolling.
		A_CUR_RIGHT((term_t) STRLEN_BUF_T + dt->ln_len - dt->cusr_x);
	}
	else if((dt->ln_len - TXT_X) >= dt->cusr_x)
	{
		// Last TXT_X chars are seen. Current line is scrolled, not cursor.
		A_CUR_RIGHT(term_sz(dt, 'x') - CUR_SZ);
	}
	else
	{
		// Text is scrolled horizontally to the start. Cursor can be moved.
		A_CUR_RIGHT(dt->ln_len - dt->cusr_x + STRLEN_BUF_T);	
	}

	// All lines fits in the window.
	int32_t mv_up = TXT_Y - (dt->lns + INDEX);
	// Lines are scrolled.
	if(dt->lns >= TXT_Y)
	{
		mv_up = 0;
	}
	A_CUR_UP(LBAR_SZ + dt->cusr_y + mv_up);
}

