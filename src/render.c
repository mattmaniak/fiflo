#include "render.h"

void help(void)
{
	printf("%s\n\n%s\n%s\n%s\n%s\n%s\n%s\n",
	"Usage: fiflo [option].",

	"Options:      Description:",
	"<NULL>        Set the filename to '/<current_path>/noname.asdf'",
	"basename      Open the textfile 'basename' using your current path.",
	"/dir/bname    Open the textfile 'bname' located in the '/dir' folder.",
	"-h, --help    Show program help.",
	"-v, --version Display some info about the current version.");
}

void version(void)
{
	printf("%s\n%s\n%s\n",
	"fiflo v2.0.0 (WIP)",
	"https://gitlab.com/mattmaniak/fiflo.git",
	"(C) 2018 mattmaniak under the MIT License.");
}

term_t get_term_sz(char axis) // TODO: RENDERING WITH SMALLER STTY SIZE.
{
	const uint8_t x_min = 60;
	const uint8_t y_min = 6;
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

void flush_window(buff dt)
{	if(dt.lns < TXT_AREA)
	{
		A_CURSOR_DOWN(TXT_AREA - dt.lns);
	}
	else
	{
		buff_t hidden_lns = dt.lns - TXT_AREA + CURRENT;
		A_CURSOR_DOWN(dt.lns - hidden_lns);
	}
	printf("%s", A_CLEAN_LINE);

	for(term_t y = 0; y < get_term_sz('Y'); y++)
	{
		printf("%s%s", A_LINE_UP, A_CLEAN_LINE);
	}
	fflush(stdout);
}

void draw_bar(buff dt, char key) // TODO: SIMPLIFY
{
	const char* title = " fiflo | filename: \0";
	uint16_t fname_max = get_term_sz('X') - strlen(title) - strlen(DOTS);

	printf("%s%s", A_INVERT, title);

	if(strlen(dt.fname) > fname_max)
	{
		printf("%.*s%s\n", fname_max, dt.fname, DOTS);
	}
	else
	{
		term_t fill = get_term_sz('X') - strlen(title) - strlen(dt.fname);
		printf("%s%*s\n", dt.fname, fill, " ");
	}
	// Lower part of the draw_bar.
	printf(" chars in the line: %*d, chars: %*d, last key: %*d%*s%s\n", 5,
	dt.chrs_ln, 5, dt.chrs, 3, key, get_term_sz('X') - 54, " ", A_RESET);
}

void lower_fill(buff_t lns)
{
	if(lns < TXT_AREA)
	{
		for(term_t ln = lns + CURRENT; ln < TXT_AREA; ln++)
		{
			putchar(LF);
		}//putchar('s');
	}
}

void set_cursor_pos(buff dt)
{
	if(dt.lns < TXT_AREA - CURRENT)
	{
		A_CURSOR_UP(TXT_AREA - dt.lns - CURRENT);
		if(strlen(dt.txt[dt.lns]) < get_term_sz('X') - strlen(DOTS))
		{
			A_CURSOR_RIGHT((term_t) strlen(dt.txt[dt.lns])
			+ (term_t) strlen(DOTS));
		}
		else
		{
			A_CURSOR_RIGHT((term_t) get_term_sz('X') - CURRENT);
		}
	}
}

void window(buff dt, char key) // TODO: SIMPLIFY
{
	const char* too_many = A_INVERT DOTS A_RESET"\0";
	buff_t hidden_lns = 0;
	draw_bar(dt, key);

	if(dt.lns >= TXT_AREA) // Horizontal scroll.
	{
		hidden_lns = dt.lns - TXT_AREA + CURRENT + 1; // 1 - DOTS in Y.
		puts(too_many);
	}
	for(term_t ln = hidden_lns; ln <= dt.lns; ln++)
	{
		printf("%s%*d%s", A_INVERT, (uint8_t) strlen(DOTS), ln, A_RESET);
		if(strlen(dt.txt[ln]) + (uint8_t) strlen(DOTS) >= get_term_sz('X'))
		{
			if(dt.txt[ln][strlen(dt.txt[ln]) - 1] == LF) // Move one right.
			{
				printf("%s%s", too_many, dt.txt[ln] + strlen(dt.txt[ln])
				+ strlen(too_many) - get_term_sz('X') - 3);
			}
			else
			{
				printf("%s%s", too_many, dt.txt[ln] + strlen(dt.txt[ln])
				+ strlen(too_many) - get_term_sz('X') - 2);
			}
		}
		else
		{
			fputs(dt.txt[ln], stdout);
		}
	}
	lower_fill(dt.lns);
	set_cursor_pos(dt);
}

