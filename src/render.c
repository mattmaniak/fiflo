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
			ANSI_RESET();
			fputs("Check term_sz function params, exit(1).\n", stderr);
			exit(1);
	}
	// Required -Wreturn-type.
	return 1;
}

void flush_win(meta* dt)
{
	RESTORE_CUR_POS();
	ANSI_CLEANLN();

	for(term_t y = 0; y < term_sz(dt, 'y'); y++)
	{
		ANSI_PTR_UP();
		ANSI_CLEANLN();
	}
	fflush(stdout);
}

void upper_bar(meta* dt) // TODO: SIMPLIFY.
{
	const char* title = "fiflo: \0";
	const char* dots = "...\0";
	const char* indicators = " linelen/chars: \0";
	term_t fname_max = term_sz(dt, 'x')
	- (term_t) (strlen(title) + strlen(dots)
	+ strlen(indicators) + (2 * STRLEN_BUF_T) + SLASH_SZ);

	ANSI_INVERT();
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
		term_sz(dt, 'x') - (term_t) (strlen(dt->fname) + 34), " ");
	}
	printf("%s%*d/%*d\n", indicators, STRLEN_BUF_T, dt->chrs_ln,
	STRLEN_BUF_T, dt->chrs);
}

void fill(meta* dt)
{
	if(dt->lns < TXT_Y)
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
	ANSI_INVERT();

	printf("\n%s%*s", LBAR_STR,
	term_sz(dt, 'x') - TERM_X_MIN + AT_LEAST_1_CHAR, " ");

	ANSI_RESET();
}

// TODO: CURSOR AND RENDERING ON SMALLER THAN TERMINAL STTY SIZE.
void window(meta* dt) // TODO: SPLIT SCROLLING TO SEPARATE FUNCS.
{
	buf_t hidden_lns = 0;
	upper_bar(dt);

	// Horizontal scroll set.
	if(dt->lns >= TXT_Y)
	{
		hidden_lns = dt->lns - TXT_Y + CURRENT;
	}
	for(term_t ln = hidden_lns; ln <= dt->lns; ln++) // TODO: LINEX > 1.
	{
		ANSI_INVERT();
		printf("%*d", STRLEN_BUF_T, ln + INDEX);
		ANSI_RESET();

		// There is small amount of chars. X-scroll isn't required.
		if((STRLEN_BUF_T + strlen(dt->txt[ln])) < term_sz(dt, 'x'))
		{
			printf("%s", dt->txt[ln]);
		}
		// Horizontal scroll required.
		else
		{
			// Text will be scrolled. Not cursor.
			if((dt->chrs_ln - TXT_X) >= dt->cusr_x)
			{
				for(buf_t x = dt->chrs_ln - dt->cusr_x - TXT_X + CUR_SZ;
				x <= dt->chrs_ln - dt->cusr_x - CUR_SZ; x++)
				{
					putchar(dt->txt[ln][x]);
				}
/*				if(dt->txt[ln][dt->chrs_ln - dt->cusr_x - CUR_SZ] != LF
				&& dt->cusr_x != 0)
				{
					putchar(LF); // TODO: WHEN LF ISN'T RENDERED - PLACEHOLDER.
				}
*/			}
			// Scrolled to start of the line. Now cursor will be scrolled.
			else
			{
				printf("%.*s", TXT_X, dt->txt[ln]);
			}
		}
	}
	fill(dt);
	lower_bar(dt);
	set_cursor_pos(dt);
}

void set_cursor_pos(meta* dt) // TODO WHEN STTY SIZE IS SMALLER THAN A TERM.
{
	// Cursor is moved by default to the right side by lower bar. Move it back.
	ANSI_CUR_LEFT(term_sz(dt, 'x') - CUR_SZ);
	// Left bottom corner. Will be used in the flush_win().
	SAVE_CUR_POS();

	// X axis.
	if((term_t) strlen(CURR_LN) < TXT_X) // TODO: MAYBE dt->chrs_ln INSTEAD STRLEN.
	{
		// No horizontal scroll.
		ANSI_CUR_RIGHT((term_t) STRLEN_BUF_T + dt->chrs_ln - dt->cusr_x);
	}
	else
	{
		// Only last TXT_X chars are seen.
		if((dt->chrs_ln - TXT_X) >= dt->cusr_x)
		{
			// Fixed position. Current line is scrolled, not cursor.
			ANSI_CUR_RIGHT(term_sz(dt, 'x') - CUR_SZ);
		}
		else
		{
			// Text is scrolled to the start. Cursor can be moved.
			ANSI_CUR_RIGHT(dt->chrs_ln - dt->cusr_x + STRLEN_BUF_T);
		}
	}

	// Y axis.
	if((dt->lns + INDEX) <= TXT_Y)
	{
		// All lines fits in the vertical text area.
		ANSI_CUR_UP(TXT_Y - (dt->lns + INDEX) + LBAR_SZ + dt->cusr_y);
	}
	else
	{
		// Lines are scrolled.
		ANSI_CUR_UP(LBAR_SZ + dt->cusr_y);
	}
}

