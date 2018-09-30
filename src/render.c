#include "render.h"

term_t termgetsz(buf* dt, char axis)
{
	const uint8_t y_min = 4;
	const term_t sz_max = USHRT_MAX;

	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	// Terminal size check.
	if(win.ws_col < TERM_X_MIN || win.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exit(1).\n", TERM_X_MIN, y_min);
		freeallexit(dt, 1);
	}
	else if(win.ws_col > sz_max || win.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exit(1).\n", sz_max, sz_max);
		freeallexit(dt, 1);
	}
	switch(axis)
	{
		case 'x':
			return win.ws_col;

		case 'y':
			return win.ws_row;

		default:
			printf("%s", RESET);
			fputs("Check termgetsz function params, exit(1).\n", stderr);
			exit(1);
	}
	// Required -Wreturn-type.
	return 1;
}

void flushwin(buf* dt)
{
	RESTORECURSPOS();
	printf("%s", CLEANLN);

	for(term_t y = 0; y <= TXT_Y; y++)
	{
		printf("%s%s", LINE_UP, CLEANLN);
	}
	fflush(stdout);
}

void upbar(buf* dt)
{
	const char* title = "fiflo | filename: \0";
	term_t fnmax = termgetsz(dt, 'x') - (term_t) (strlen(title) + strlen(DOTS));

	printf("%s%s", INVERT, title);
	if(strlen(dt->fname) > fnmax)
	{
		// Filename will be visually shrinked and terminated by "[...]".
		printf("%.*s%s\n", fnmax, dt->fname, DOTS);
	}
	else
	{
		// Whole filename will be displayed.
		printf("%s%*s\n", dt->fname, termgetsz(dt, 'x')
		- (term_t) (strlen(title) + strlen(dt->fname)), " ");
	}
}

void lowbar(buf* dt, char key)
{
	printf(
	"%s\nchars [all, ln, last]: %*d, %*d, %*d%*s| CTRL+: D - save, C - exit%s",
	INVERT, STRLEN_BUF_T, dt->chrs, STRLEN_BUF_T, dt->chrs_ln, 3, key,
	termgetsz(dt, 'x') - TERM_X_MIN + 1, " ", RESET);
}

void window(buf* dt, char key) // TODO: SPLIT SCROLLING TO SEPARATE FUNCS.
{
	buf_t hidden_lns = 0;
	upbar(dt);

	// Horizontal scroll set.
	if(dt->lns >= TXT_Y)
	{
		hidden_lns = dt->lns - TXT_Y + CURRENT;
	}
	for(term_t ln = hidden_lns; ln <= dt->lns; ln++)
	{
		if((STRLEN_BUF_T + strlen(dt->txt[ln])) < termgetsz(dt, 'x'))
		{
			// There is small amount of chars. X-scroll isn't required.
			printf("%s%*d%s%s", INVERT, STRLEN_BUF_T, ln + INDEX, RESET,
			dt->txt[ln]);
		}
		else
		{
			// Horizontal scroll required.
			_Bool left;
			if(dt->txt[ln][strlen(dt->txt[ln]) - INDEX] == LF) // Move one right.
			{
				left = 0;
			}
			else
			{
				left = 1;
			}
			// TODO: SCROLLING TO LEFT;
			printf("%s%s%s", INVERT, DOTS, RESET);

			if((dt->chrs_ln - TXT_X) >= dt->cusr_x) // TODO: LEFT.
			{
				// Text will be scrolled. Not cursor.
				for(buf_t x = dt->chrs_ln - TXT_X + left - dt->cusr_x;
				x <= dt->chrs_ln - dt->cusr_x; x++)
				{
					putchar(dt->txt[ln][x]);
				}
			}
			else
			{
				// Scrolled to start of the line. Now cursor will be scrolled.
				printf("%.*s", TXT_X, dt->txt[ln]);
			}
//			printf("%s", dt->txt[ln]
//			+ strlen(dt->txt[ln]) + strlen(DOTS) - termgetsz(dt, 'x') + left);
		}
	}
	fill(dt);
	lowbar(dt, key);
	setcurpos(dt);
}

void fill(buf* dt)
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

void setcurpos(buf* dt)
{
	// Cursor is moved by default to the right side by lower bar. Move it back.
	CURSLEFT(termgetsz(dt, 'x') - CUR_SZ);
	// Will be used in the flushwin().
	SAVECURSPOS();

	// X axis.
	if((term_t) strlen(CURRLN) < TXT_X) // TODO: MAYBE dt->chrs_ln INSTEAD STRLEN.
	{
		// No horizontal scroll.
		CURSRIGHT((term_t) strlen(DOTS) + dt->chrs_ln - dt->cusr_x);
	}
	else
	{
		// Only last TXT_X chars are seen.
		if((dt->chrs_ln - TXT_X) > dt->cusr_x)
		{
			// Fixed position. Current line is scrolled, not cursor.
			CURSRIGHT(termgetsz(dt, 'x') - CUR_SZ);
		}
		else
		{
			// Text is scrolled to the start. Cursor can be moved.
			CURSRIGHT(dt->chrs_ln - dt->cusr_x + STRLEN_BUF_T);
		}
	}

	// Y axis.
	if((dt->lns + INDEX) <= TXT_Y)
	{
		// All lines fits in the vertical text area.
		CURSUP(TXT_Y - dt->lns + dt->cusr_y);
	}
	else
	{
		// Lines are scrolled.
		CURSUP(LBAR_SZ + dt->cusr_y);
	}
}

