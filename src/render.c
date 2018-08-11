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
	"fiflo v1.1.0 (WIP)",
	"https://gitlab.com/mattmaniak/fiflo",
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

void flush_window(buff_t lines)
{
	MV_CURSOR_DOWN(PLACE_FOR_TEXT - lines);

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

void bar(buff data, char key)
{
	const uint8_t dots_and_space = 4;
	const char* words[5] =
	{
		" fiflo | filename: \0",
		" lines: \0",
		" | chars: \0",
		" | last: \0",
		" | save: CTRL+D | exit: CTRL+X \0"
	};
	char keycode[4];
	char lines[11];
	char chars[11];

	snprintf(keycode, sizeof(keycode), "%d", key);
	snprintf(lines, sizeof(lines), "%d", data.lines);
	snprintf(chars, sizeof(chars), "%d", data.chars);

	// Upper part of the bar.
	printf("%s%s", COLORS_INVERT, words[0]);
	print_fname(words[0], data.fname, get_term_sz('X') - strlen(words[0])
	- dots_and_space);

	// Lower part of the bar.
	term_t whitespace = strlen(words[4]) + strlen(words[1]) + strlen(lines)
	+ strlen(words[2]) + strlen(chars) + strlen(words[3]) + strlen(keycode);

	printf("%s%d%s%d%s%d%s%*s%s", words[1], data.lines, words[2], data.chars,
	words[3], key, words[4], get_term_sz('X') - whitespace, " ", COLORS_RESET);
}

buff_t scroll(buff data)
{
	buff_t lines_to_hide = data.lines - PLACE_FOR_TEXT;
	buff_t offset = 0;

	for(buff_t pos = 0; pos < data.chars; pos++)
	{
		if(data.text[pos] == LINEFEED)
		{
			offset++; // Temponary usage.
		}
		if(offset == lines_to_hide + 1) // How many lines to scroll.
		{
			offset = pos;
			break;
		}
	}
	printf("%s", "...");
	return offset;
}

void set_cursor_pos(buff data)
{
	term_t right = 1;

	if(data.lines < PLACE_FOR_TEXT)
	{
		MV_CURSOR_UP(PLACE_FOR_TEXT - data.lines);
		for(buff_t pos = data.chars; pos > 0; pos--) // Last line is auto pos.
		{
			if(data.text[pos] == LINEFEED)
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

	if(data.text[0] == LINEFEED && data.lines == 2) // Upper algorithm weakness.
	{
		MV_CURSOR_LEFT(1);
	}
}

void print_text(buff data)
{
	if(data.lines <= PLACE_FOR_TEXT) // Text fits in a screen.
	{
		if(data.chars == 0 || data.text[0] == LINEFEED)
		{
			putchar(LINEFEED); // Necessary at least for the LXTerminal.
		}
		fputs(data.text, stdout);
	}
	else
	{
		for(buff_t pos = scroll(data); pos < data.chars; pos++)
		{
			putchar(data.text[pos]);
		}
	}
}

buff auto_newline(buff data) // TODO: CHANGE TO HORIZONTAL SCROLL
{
	term_t offset = 0;
	if(data.lines == 1)
	{
		if(data.chars >= get_term_sz('X'))
		{
			data.text[data.chars - 1] = LINEFEED;
			data.lines++;
		}
	}
	else
	{
		for(buff_t pos = data.chars - 1; pos >= 0; pos--)
		{
			if(data.text[pos] == LINEFEED)
			{
				offset = data.chars - pos;
				break;
			}
		}
		if(offset > get_term_sz('X'))
		{
			data.text[data.chars - 1] = LINEFEED;
			data.lines++;
		}
	}
	return data;
}

void window(buff data, char key)
{
	bar(data, key);
	data = auto_newline(data);
	print_text(data);

	if(data.lines <= PLACE_FOR_TEXT) // Visible bottom fill.
	{
		for(term_t y = data.lines; y < PLACE_FOR_TEXT; y++)
		{
			putchar(LINEFEED);
		}
	}
	set_cursor_pos(data);
}

