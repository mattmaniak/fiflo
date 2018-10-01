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
	CLEANLN();

	for(term_t y = 0; y < term_sz(dt, 'y'); y++)
	{
		LINE_UP();
		CLEANLN();
	}
	fflush(stdout);
}

void upper_bar(meta* dt)
{
	const char* title = "fiflo | filename: \0";
	term_t fnmax = term_sz(dt, 'x') - (term_t) (strlen(title) + 3);

	ANSI_INVERT();
	printf("%s", title);
	if(strlen(dt->fname) > fnmax)
	{
		// Filename will be visually shrinked and terminated by dots.
		printf("%.*s%s\n", fnmax, dt->fname, "...");
	}
	else
	{
		// Whole filename will be displayed.
		printf("%s%*s\n", dt->fname,
		term_sz(dt, 'x') - (term_t) (strlen(title) + strlen(dt->fname)), " ");
	}
}

void lower_bar(meta* dt)
{
	ANSI_INVERT();
	printf(
	"\nchars [all, ln]: %*d, %*d%*s| CTRL+: D - save, C - exit",
	STRLEN_BUF_T, dt->chrs, STRLEN_BUF_T, dt->chrs_ln,
	term_sz(dt, 'x') - TERM_X_MIN + 1, " ");

	printf("\n%*s", term_sz(dt, 'x'), " ");

	ANSI_RESET();
}

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
		if((STRLEN_BUF_T + strlen(dt->txt[ln])) < term_sz(dt, 'x'))
		{
			// There is small amount of chars. X-scroll isn't required.
			printf("%s", dt->txt[ln]);
		}
		else
		{
			// Horizontal scroll required.
			if((dt->chrs_ln - TXT_X) >= dt->cusr_x)
			{
				// Text will be scrolled. Not cursor.
				for(buf_t x = dt->chrs_ln - dt->cusr_x - TXT_X + CUR_SZ;
				x <= dt->chrs_ln - dt->cusr_x - CUR_SZ; x++)
				{
					putchar(dt->txt[ln][x]);
				}
			}
			else
			{
				// Scrolled to start of the line. Now cursor will be scrolled.
				printf("%.*s", TXT_X, dt->txt[ln]);
			}
		}
	}
	fill(dt);
	lower_bar(dt);
	set_cursor_pos(dt);
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

void set_cursor_pos(meta* dt) // TODO
{
	// Cursor is moved by default to the right side by lower bar. Move it back.
	MV_CUR_LEFT(term_sz(dt, 'x') - CUR_SZ);
	// Left bottom corner. Will be used in the flush_win().
	SAVE_CUR_POS();

	// X axis.
	if((term_t) strlen(CURR_LN) < TXT_X) // TODO: MAYBE dt->chrs_ln INSTEAD STRLEN.
	{
		// No horizontal scroll.
		MV_CUR_RIGHT((term_t) STRLEN_BUF_T + dt->chrs_ln - dt->cusr_x);
	}
	else
	{
		// Only last TXT_X chars are seen.
		if((dt->chrs_ln - TXT_X) > dt->cusr_x)
		{
			// Fixed position. Current line is scrolled, not cursor.
			MV_CUR_RIGHT(term_sz(dt, 'x') - CUR_SZ);
		}
		else
		{
			// Text is scrolled to the start. Cursor can be moved.
			MV_CUR_RIGHT(dt->chrs_ln - dt->cusr_x + STRLEN_BUF_T);
		}
	}

	// Y axis.
	if((dt->lns + INDEX) <= TXT_Y)
	{
		// All lines fits in the vertical text area.
		MV_CUR_UP(TXT_Y - (dt->lns + INDEX) + LBAR_SZ + dt->cusr_y);
	}
	else
	{
		// Lines are scrolled.
		MV_CUR_UP(LBAR_SZ + dt->cusr_y);
	}
}

