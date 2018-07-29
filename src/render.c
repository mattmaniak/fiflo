// TODO: DESCRIPTION!

#include "render.h"

void showHelp(void)
{
	printf("%s\n\n%s\n%s\n%s\n%s\n%s\n%s\n",
	"Usage: fiflo [option].",
	"Options:        Description:",
	"<NULL>          Open and set the basename to 'noname.asdf'.",
	"basename        Open the textfile named 'basename' using current path.",
	"/path/basename  Open the textfile 'basename' located in the '/path'.",
	"-h, --help      Show program help.",
	"-v, --version   Display info about the current version.");
}

void showVersion(void)
{
	printf("%s\n%s\n%s\n",
	"fiflo v1.1.0 (WIP)",
	"(c) 2018 mattmaniak",
	"https://gitlab.com/mattmaniak/fiflo");
}

// Cuts a string when is too long. TODO
void printDynamicFilename(const char *string, const char *prog, term_t max_len)
{
	term_t pos;
	term_t whitespace = termSize(X) - strlen(string) - strlen(prog);

	if(strlen(string) > max_len)
	{
		for(pos = 0; pos < max_len; pos++)
		{
			printf("%c", string[pos]);
		}
		printf("%s", "... ");
	}
	else
	{
		printf("%s", string);
		for(pos = 0; pos < whitespace; pos++)
		{
			printf("%c", ' ');
		}
	}
}

void bar(struct Data buff, char key) // TODO: SIMPLIFY!
{
	const char *info[5] = {" fiflo | file: \0",
	" lines: \0", " | chars: \0", " | last: \0",
	" | save: CTRL+D | exit: CTRL+X \0"};

	const uint8_t dots_and_space = 4;
	term_t x;

	printf("%s%s", INVERT, info[0]);
	printDynamicFilename(buff.filename, info[0], termSize(X) - strlen(info[0])
	- dots_and_space);

	char keycode[4];
	sprintf(keycode, "%i", key);

	char lines[11];
	sprintf(lines, "%i", buff.lines);

	char chars[11];
	sprintf(chars, "%i", buff.chars);

	// Lower part of the bar.
	term_t whitespace = strlen(info[4]) + strlen(info[1]) + strlen(lines)
	+ strlen(info[2]) + strlen(chars) + strlen(info[3]) + strlen(keycode);

	printf("%s%i%s%i%s%i%s", info[1], buff.lines, info[3],
	buff.chars, info[3], key, info[4]);

	for(x = 0; x <= termSize(X) - whitespace; x++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);
}

term_t termSize(bool axis) // Check terminal size.
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
		case X:
			return win.ws_col;
		case Y:
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	term_t y;
	for(y = 0; y < termSize(Y); y++)
	{
		printf("%s", "\033[F\033[K"); // Go to the upper line and clean it.
	}
	fflush(stdout);
}

// Pressed keys to rendered chars in proper order.
void renderText(struct Data buff)
{
	buff_t pos;

	if(buff.text[0] == TERMINATOR || LINEFEED)
	{
		printf("%c", LINEFEED); // Necessary at least for the LXTerminal.
	}

	if(buff.lines <= termSize(Y) - BAR_SZ)
	{
		for(pos = 0; pos < buff.chars; pos++) // Chars rendering.
		{
			printf("%c", buff.text[pos]);
		}
	}
	else // More lines than the terminal can render - scrolling. TODO: MULTIPLE
	{
		static term_t renderable_lines = 0;
		static term_t chars_offset;

		for(pos = 0; pos < buff.chars; pos++)
		{
			if(buff.text[pos] == LINEFEED)
			{
				renderable_lines++;
				if(renderable_lines == termSize(Y) - BAR_SZ)
				{
					renderable_lines = termSize(Y) - BAR_SZ;
					chars_offset = buff.chars;
				}
			}
		}
		for(pos = chars_offset; pos < buff.chars; pos++) // Chars rendering.
		{
			printf("%c", buff.text[pos]);
		}
//		printf("%i", chars_offset);
	}
}

void windowFill(buff_t lines)
{
	term_t y;
	if(lines <= termSize(Y) + BAR_SZ)
	{
		for(y = lines + BAR_SZ; y < termSize(Y); y++)
		{
			printf("%c", LINEFEED);
		}
	}
}

// Terminal fill that shows chars and other stupid things.
struct Data window(struct Data buff, char key)
{
	buff = allocText(buff, key);
//	buff = punchedCard(buff, MAX_CHARS_PER_LINE, WRITE, key);
//	charsLimit(buff.chars);

	bar(buff, key);
	renderText(buff);

	return buff;
}

