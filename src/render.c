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
	"(c) 2018 mattmaniak under the MIT License",
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

// TODO: WITH SCROLL
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
		printf("%s%s", LINE_UP, CLEAN_LINE);
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
	" | save: CTRL+D | exit: CTRL+X \0"};

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

void set_cursor_pos(buff data) // TODO: SCROLLING
{
	term_t y;
	buff_t pos;
	term_t right = 0;

	for(y = 0; y < get_term_sz('Y') - BAR_SZ - data.lines; y++)
	{
		printf("%s", CURSOR_UP);
	}
	for(pos = data.chars; pos > 0; pos--)
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
	for(y = 0; y < right; y++)
	{
		printf("%s", CURSOR_RIGHT);
	}
	if(data.lines == 2)
	{
		printf("%s", CURSOR_LEFT);
	}
}

buff_t scroll(buff data)
{
	buff_t hidden_lines = data.lines % (get_term_sz('Y') - BAR_SZ - 1);
	buff_t offset = 0;

	for(buff_t pos = 0; pos < data.chars; pos++)
	{
		if(data.text[pos] == LINEFEED)
		{
			offset++;
		}
		if(offset == hidden_lines)
		{
			offset = pos;
			break;
		}
	}
	return offset;
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

// Shows chars and other stupid things.
void window(buff data, char key)
{
	term_t y;

	bar(data, key);
	print_text(data);

	if(data.lines <= get_term_sz('Y') + BAR_SZ)
	{
		for(y = data.lines + BAR_SZ; y < get_term_sz('Y'); y++)
		{
			putchar(LINEFEED);
		}
	}
	set_cursor_pos(data);
}

