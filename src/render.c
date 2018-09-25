#include "render.h"

term_t termgetsz(buf* dt, char axis)
{
	const uint8_t y_min = 4;
	const term_t sz_max = USHRT_MAX;

	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < TERM_X_MIN || win.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size: %dx%d, exited.\n", TERM_X_MIN, y_min);
		freeallexit(dt, 1);
	}
	else if(win.ws_col > sz_max || win.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size: %dx%d, exited.\n", sz_max, sz_max);
		freeallexit(dt, 1);
	}
	switch(axis)
	{
		case 'X':
			return win.ws_col;
		case 'Y':
			return win.ws_row;
	}
	return 0; // Required -Wreturn-type.
}

void flushwin(buf* dt)
{
	RESTORECURSPOS();
	printf("%s", CLEANLN);

	for(term_t y = 0; y <= TXT_AREA; y++)
	{
		printf("%s%s", LINE_UP, CLEANLN);
	}
	fflush(stdout);
}

void bar(buf* dt, char key)
{
	const char* title = "fiflo | filename: \0";
	term_t fnmax = termgetsz(dt, 'X') - (term_t) (strlen(title) + strlen(DOTS));

	printf("%s%s", INVERT, title);
	if(strlen(dt->fname) > fnmax)
	{
		printf("%.*s%s\n", fnmax, dt->fname, DOTS);
	}
	else
	{
		printf("%s%*s\n", dt->fname, termgetsz(dt, 'X')
		- (term_t) (strlen(title) + strlen(dt->fname)), " ");
	}
	printf(
	"chars (all, ln, last): %*d, %*d, %*d%*s| CTRL+: D - save, C - exit%s\n",
	STRLENBUFF, dt->chrs, STRLENBUFF, dt->chrs_ln, 3, key,
	termgetsz(dt, 'X') - TERM_X_MIN + 1, " ", RESET);
}


void window(buf* dt, char key)
{
	buf_t hidden_lns = 0;
	bar(dt, key);

	if(dt->lns >= TXT_AREA) // Horizontal scroll.
	{
		hidden_lns = dt->lns - TXT_AREA + CURRENT + 1; // 1 - DOTS in Y.
		printf("%s%s%s\n", INVERT, DOTS, INVERT);
	}
	for(term_t ln = hidden_lns; ln <= dt->lns; ln++)
	{
		if(STRLENBUFF + strlen(dt->txt[ln]) >= termgetsz(dt, 'X'))
		{
			_Bool left;
			if(dt->txt[ln][strlen(dt->txt[ln]) - INDEX] == LF) // Move one right.
			{
				left = 0;
			}
			else
			{
				left = 1;
			}
			printf("%s%s%s%s", INVERT, DOTS, RESET, dt->txt[ln]
			+ strlen(dt->txt[ln]) + strlen(DOTS) - termgetsz(dt, 'X') + left);
		}
		else
		{
			printf("%s%*d%s", INVERT, STRLENBUFF, ln + INDEX, RESET);
			if(dt->txt[ln][0] == TAB)
			{
				printf("%*s", STRLENBUFF, " ");
			}
			fputs(dt->txt[ln], stdout);
		}
	}
	lower_fill(dt);
	setcurspos(dt);
}

void lower_fill(buf* dt)
{
	if(dt->lns < TXT_AREA)
	{
		for(term_t ln = dt->lns; ln < TXT_AREA - CURRENT; ln++)
		{
			putchar(LF);
		}
	}
}

void setcurspos(buf* dt)
{
	SAVECURSPOS();
	if(dt->lns < TXT_AREA - CURRENT)
	{
		if(strlen(CURRLN) < termgetsz(dt, 'X') - strlen(DOTS))
		{
			CURSRIGHT((term_t) strlen(DOTS) + dt->cusr_x);
		}
		else
		{
			CURSRIGHT((term_t) termgetsz(dt, 'X') - CURRENT);
		}
		CURSUP(TXT_AREA - dt->cusr_y - CURRENT);
	}
	// Else by default on the bottom && auto-positioned.
}

