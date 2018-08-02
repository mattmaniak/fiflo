// TODO: DESCRIPTION!

#include "render.h"

void help(void)
{
	printf("%s\n\n%s\n%s\n%s\n%s\n%s\n%s\n",
	"Usage: fiflo [option].",

	"Options:        Description:",
	"<NULL>          Open and set the basename to 'noname.asdf'.",
	"basename        Open the textfile named 'basename' using current path.",
	"/path/basename  Open the textfile 'basename' located in the '/path'.",
	"-h, --help      Show program help.",
	"-v, --version   Display words about the current version.");
}

void version(void)
{
	printf("%s\n%s\n%s\n",
	"fiflo v1.1.0 (WIP)",
	"(c) 2018 mattmaniak",
	"https://gitlab.com/mattmaniak/fiflo");
}

term_t get_term_sz(char axis) // Check terminal size.
{
	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_X || win.ws_row < MIN_Y)
	{
		fprintf(stderr, "%s%i%c%i%s\n",
		"Min. term size is ", MIN_X, 'x', MIN_Y, ", exited.");
		exit(1);
	}
	else if(win.ws_col > MAX_X || win.ws_row > MAX_Y)
	{
		fprintf(stderr, "%s%i%c%i%s\n",
		"Max. term size is ", MAX_X, 'x', MAX_Y, ", exited.");
		exit(1);
	}

	switch(axis)
	{
		case 'X':
			return win.ws_col;
		case 'Y':
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

void flush_window(buff_t lines) // To provide rendering in a one frame.
{
	term_t y;
	for(y = 0; y < get_term_sz('Y') - BAR_SZ - lines; y++)
	{
		printf("%s", CURSOR_DOWN);
	}
	printf("%s", CLEAN_LINE);
	for(y = 0; y < get_term_sz('Y'); y++)
	{
		printf("%s%s", GO_UPPER_LINE, CLEAN_LINE);
	}
	fflush(stdout);
}

// Cuts a string when is too long. TODO: NAMES!
void print_fname(const char* string, const char* prog, term_t max_len)
{
	term_t pos;
	term_t whitespace = get_term_sz('X') - strlen(string) - strlen(prog);

	if(strlen(string) > max_len)
	{
		for(pos = 0; pos < max_len; pos++)
		{
			putchar(string[pos]);
		}
		printf("%s", "... ");
	}
	else
	{
		printf("%s", string);
		for(pos = 0; pos < whitespace; pos++)
		{
			putchar(' ');
		}
	}
}

void bar(buff data, char key)
{
	term_t x;

	const char* words[5] = {" fiflo | file: \0",
	" lines: \0",
	" | chars: \0",
	" | last: \0",
	" | save: CTRL+D | exit: CTRL+'X' \0"};

	const uint8_t dots_and_space = 4;
	char keycode[4];
	char lines[11];
	char chars[11];

	sprintf(keycode, "%i", key);
	sprintf(lines, "%i", data.lines);
	sprintf(chars, "%i", data.chars);

	printf("%s%s", INVERT, words[0]);
	print_fname(data.filename, words[0], get_term_sz('X') - strlen(words[0])
	- dots_and_space);

	// Lower part of the bar.
	term_t whitespace = strlen(words[4]) + strlen(words[1]) + strlen(lines)
	+ strlen(words[2]) + strlen(chars) + strlen(words[3]) + strlen(keycode);

	printf("%s%i%s%i%s%i%s", words[1], data.lines, words[2],
	data.chars, words[3], key, words[4]);

	for(x = 0; x < get_term_sz('X') - whitespace; x++)
	{
		putchar(' ');
	}
	printf("%s", RESET);
}

void scroll(buff data) // TODO: DELETING LAST CHAR AFTER SCROLL.
{
	static term_t chars_offset;
	static term_t renderable_lines = 0;
	buff_t pos;

	for(pos = 0; pos < data.chars; pos++)
	{
		if(data.text[pos] == LINEFEED)
		{
			renderable_lines++;
			if(renderable_lines == get_term_sz('Y') - BAR_SZ)
			{
				renderable_lines = get_term_sz('Y') - BAR_SZ;
				chars_offset = data.chars;
			}
		}
	}
	for(pos = chars_offset; pos < data.chars; pos++) // Chars rendering.
	{
		putchar(data.text[pos]);
	}
}

// Pressed keys to rendered chars in proper order.
void print_text(buff data)
{
	if(data.lines < get_term_sz('Y') - BAR_SZ)
	{
		if(data.chars == 0 || data.text[0] == LINEFEED)
		{
			putchar(LINEFEED); // Necessary at least for the LXTerminal.
		}
		printf("%s", data.text);
	}
	else
	{
		scroll(data);
	}
}

// Shows chars and other stupid things.
buff window(buff data, char key)
{
	term_t y;

	data = alloc_text(data, key);
	data = count_lines(data);
	chars_limit(data.chars);

	bar(data, key);
	print_text(data);

	if(data.lines <= get_term_sz('Y') + BAR_SZ)
	{
		for(y = data.lines + BAR_SZ; y < get_term_sz('Y'); y++)
		{
			putchar(LINEFEED);
		}
	}
	return data;
}

