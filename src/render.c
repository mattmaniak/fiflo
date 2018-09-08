#include "render.h"

term_t termgetsz(char axis)
{
	const uint8_t x_min = 45;
	const uint8_t y_min = 4;
	const uint16_t sz_max = USHRT_MAX;

	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < x_min || win.ws_row < y_min)
	{
		fprintf(stderr, "Min. term size is %dx%d, exited.\n", x_min, y_min);
		exit(1);
	}
	else if(win.ws_col > sz_max || win.ws_row > sz_max)
	{
		fprintf(stderr, "Max. term size is %dx%d, exited.\n", sz_max, sz_max);
		exit(1);
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

void flushwin(void)
{
	RESTORE_CURSOR_POS();
	printf("%s", CLEAN_LN);

	for(term_t y = 0; y <= TXT_AREA; y++)
	{
		printf("%s%s", LINE_UP, CLEAN_LN);
	}
	fflush(stdout);
}

void bar(buff dt, char key)
{
	const char* title = "fiflo | filename:\0";
	term_t fname_max = termgetsz('X') - (term_t )(strlen(title) - strlen(DOTS));

	printf("%s%s", INVERT, title);

	if(strlen(dt.fname) > fname_max)
	{
		printf(" %.*s%s\n", fname_max, dt.fname, DOTS);
	}
	else
	{
		printf("%s%*s\n", dt.fname, (term_t) (termgetsz('X') - strlen(title)
		- strlen(dt.fname)), " ");
	}
	// Lower part of the bar.
	printf("chars (all | line | last):%*s%*d | %*d | %*d%s\n",
	termgetsz('X') - 45, " ", 5, dt.chrs, 5, dt.chrs_ln, 3, key, RESET);
}

void lower_fill(buff_t lns)
{
	if(lns < TXT_AREA)
	{
		for(term_t ln = lns; ln < TXT_AREA - CURRENT; ln++)
		{
			putchar(LF);
		}
	}
}

void set_cursor_pos(buff dt)
{
	SAVE_CURSOR_POS();
	if(dt.lns < TXT_AREA - CURRENT)
	{
		if(strlen(dt.txt[dt.lns]) < termgetsz('X') - strlen(DOTS))
		{
			CURSOR_RIGHT((term_t) (strlen(dt.txt[dt.lns]) + strlen(DOTS)));
		}
		else
		{
			CURSOR_RIGHT((term_t) termgetsz('X') - CURRENT);
		}
		CURSOR_UP(TXT_AREA - dt.lns - CURRENT);
	}
	// Else by default on the bottom.
}

void window(buff dt, char key) // TODO: SIMPLIFY
{
	buff_t hidden_lns = 0;
	bar(dt, key);

	if(dt.lns >= TXT_AREA) // Horizontal scroll.
	{
		hidden_lns = dt.lns - TXT_AREA + CURRENT + 1; // 1 - DOTS in Y.
		printf("%s%s%s\n", INVERT, DOTS, INVERT);
	}
	for(term_t ln = hidden_lns; ln <= dt.lns; ln++)
	{
		if(strlen(dt.txt[ln]) + STRLENBUFF >= termgetsz('X'))
		{
			_Bool fill;
			if(dt.txt[ln][strlen(dt.txt[ln]) - INDEX] == LF) // Move one right.
			{
				fill = 0;
			}
			else
			{
				fill = 1;
			}
			printf("%s%s%s%s", INVERT, DOTS, RESET, dt.txt[ln] + strlen(dt.txt[ln])
			+ strlen(DOTS) - termgetsz('X') + fill);
		}
		else
		{
			printf("%s%*d%s", INVERT, STRLENBUFF, ln + INDEX, RESET);
			fputs(dt.txt[ln], stdout);
		}
	}
	lower_fill(dt.lns);
	set_cursor_pos(dt);
}

