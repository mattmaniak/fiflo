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
		fprintf(stderr, "Minimum terminal size is: %dx%d, exited.\n", MIN_X,
		MIN_Y);
		exit(1);
	}
	else if(win.ws_col > MAX_X || win.ws_row > MAX_Y)
	{
		fprintf(stderr, "Maximum terminal size is: %dx%d, exited.\n", MAX_X,
		MAX_Y);
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

void flush_window(buff_t lns)
{
	MV_CURSOR_DOWN(TXT_PLACE - lns);

	printf("%s", CLEAN_WHOLE_LINE);
	for(term_t y = 0; y < get_term_sz('Y'); y++)
	{
		printf("%s%s", LINE_UP, CLEAN_WHOLE_LINE);
	}
	fflush(stdout);
}

void print_fname(const char* prog, char* fname, term_t max_len)
{
	if(strlen(fname) > max_len)
	{
		printf("%.*s%s", max_len, fname, "... "); // Precision of the output.
	}
	else
	{
		term_t whitespace = get_term_sz('X') - strlen(prog) - strlen(fname);
		printf("%s%*s", fname, whitespace, " ");
	}
}

void bar(buff dat, char key)
{
	const uint8_t dots_and_space = 4;
	const char* words[5] =
	{
		" fiflo | filename: \0",
		" lines: \0",
		" | chars: \0",
		" | last: \0",
		" | save: CTRL+D | exit: CTRL+X\0"
	};
	char keycode[4];
	char lns[11];
	char chrs[11];

	snprintf(keycode, sizeof(keycode), "%d", key);
	snprintf(lns, sizeof(lns), "%d", dat.ln);
	snprintf(chrs, sizeof(chrs), "%d", dat.chrs);

	// Upper part of the bar.
	printf("%s%s", COLORS_INVERT, words[0]);
	print_fname(words[0], dat.fname, get_term_sz('X') - strlen(words[0])
	- dots_and_space);

	// Lower part of the bar.
	term_t whitespace = strlen(words[4]) + strlen(words[1]) + strlen(lns)
	+ strlen(words[2]) + strlen(chrs) + strlen(words[3]) + strlen(keycode);

	printf("%s%d%s%d%s%d%s%*s%s", words[1], dat.ln, words[2], dat.chrs,
	words[3], key, words[4], get_term_sz('X') - whitespace, " ", COLORS_RESET);
}
/*
buff_t scroll(buff dat)
{
	buff_t lines_to_hide = dat.ln - TXT_PLACE;
	buff_t offset = 0;

	for(buff_t pos = 0; pos < dat.chrs; pos++)
	{
		if(dat.txt[pos] == LINEFEED)
		{
			offset++; // Temponary usage.
		}
		if(offset == lines_to_hide + 1) // How many lns to scroll.
		{
			offset = pos;
			break;
		}
	}
	printf("%s", "[...]");
	return offset;
}
*//*
void set_cursor_pos(buff dat)
{
	term_t right = 1;

	if(dat.ln < TXT_PLACE)
	{
		MV_CURSOR_UP(TXT_PLACE - dat.ln);
		for(buff_t pos = dat.chrs; pos > 0; pos--) // Last row is auto pos.
		{
			if(dat.txt[pos] == LINEFEED)
			{
				if(right > 0)
				{
					right--;
				}
				break;
			}
			right++;
		}
	}
	MV_CURSOR_RIGHT(right);
	MV_CURSOR_LEFT(1);

	if(dat.txt[0] == LINEFEED && dat.ln == 2) // Upper algorithm weakness.
	{
		MV_CURSOR_LEFT(1);
	}
}
*/
void print_text(buff dat)
{
	if(dat.chrs == 0 || dat.txt[0][0] == LINEFEED)
	{
		putchar(LINEFEED); // Necessary for tested terminals.
	}
	for(term_t ln = 0; ln <= dat.ln; ln++)
	{
		fputs(dat.txt[ln], stdout);
	}
}

void window(buff dat, char key)
{
	bar(dat, key);
	print_text(dat);

	if(dat.ln <= TXT_PLACE) // Visible bottom fill.
	{
		for(term_t ln = dat.ln + 1; ln < TXT_PLACE; ln++)
		{
			putchar(LINEFEED);
		}
	}
//	set_cursor_pos(dat);
}

