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

term_t get_term_sz(char axis)
{
	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_X || win.ws_row < MIN_Y)
	{
		fprintf(stderr, "Min. term size is %dx%d, exited.\n", MIN_X, MIN_Y);
		exit(1);
	}
	else if(win.ws_col > MAX_X || win.ws_row > MAX_Y)
	{
		fprintf(stderr, "Max. term size is %dx%d, exited.\n", MAX_X, MAX_Y);
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

void flush_window(buff dat)
{
	if(dat.lns < TXT_AREA)
	{
		MV_CURSOR_DOWN(TXT_AREA - dat.lns);
	}
	printf("%s", CLEAN_WHOLE_LINE);

	for(term_t y = 0; y < get_term_sz('Y'); y++)
	{
		printf("%s%s", LINE_UP, CLEAN_WHOLE_LINE);
	}
	fflush(stdout);
}

void bar(buff dat, char key)
{
	const uint8_t dots_and_space = 6;
	const char* title = " fiflo | filename: \0";
	uint16_t max_len = get_term_sz('X') - strlen(title) - dots_and_space;

	printf("%s%s", COLORS_INVERT, title);

	if(strlen(dat.fname) > max_len)
	{
		printf("%.*s%s", max_len, dat.fname, "[...] ");
	}
	else
	{
		term_t fill = get_term_sz('X') - strlen(title) - strlen(dat.fname);
		printf("%s%*s", dat.fname, fill, " ");
	}
	// Lower part of the bar.
	printf(" lines: %.*d, chars: %.*d, last key: %.*d%*s%s", 11, dat.lns + 1,
	11, dat.chrs, 3, key, get_term_sz('X') - 54, " ", COLORS_RESET);
}

buff_t scroll(buff dat)
{
	const bool current_line = 1;
	buff_t hidden_lines = 0;
	if(dat.lns >= TXT_AREA)
	{
		hidden_lines = dat.lns - TXT_AREA + current_line;
	}
	return hidden_lines;
}

void set_cursor_pos(buff dat)
{
	if(dat.lns < TXT_AREA)
	{
		MV_CURSOR_UP(TXT_AREA - dat.lns - 1);
	}
	else
	{
//		MV_CURSOR_DOWN(1);
	}
	MV_CURSOR_RIGHT((buff_t) strlen(dat.txt[dat.lns]));
	if(dat.txt[dat.lns][0] == NULLTERM)
	{
		MV_CURSOR_LEFT(1);
	}
}

void window(buff dat, char key)
{
	bar(dat, key);

	for(term_t ln = scroll(dat); ln <= dat.lns; ln++)
	{
		fputs(dat.txt[ln], stdout);
	}
	if(dat.chrs == 0 || dat.txt[0][0] == LINEFEED)
	{
		putchar(LINEFEED); // Necessary for tested terminals.
	}
	for(term_t ln = dat.lns + 1; ln < TXT_AREA; ln++)
	{
		putchar(LINEFEED);
	}
	set_cursor_pos(dat);
}

