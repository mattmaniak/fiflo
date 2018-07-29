#include "render.h"

#include "handling.c"
#include "ui.c"

term_t termSize(bool axis) // Check terminal size.
{
	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		fprintf(stderr, "%s%i%c%i%s",
		"Min. term size is ", MIN_WIDTH, 'x', MIN_HEIGHT, ".\n");
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT)
	{
		fprintf(stderr, "%s%i%c%i%s",
		"Max. term size is ", MAX_WIDTH, 'x', MAX_HEIGHT, ", exited.\n");
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
		printf("%s", "\033[F\033[K");
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
	buff = punchedCard(buff, MAX_CHARS_PER_LINE, WRITE, key);
//	charsLimit(buff.chars);

	bar(buff, key);
	renderText(buff);

	return buff;
}

