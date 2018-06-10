#include "render.h"
#include "ui.c"

// Drawing funcions.
static char text[10][81];

static uint16_t windowSize(char axis) // Check term size.
{
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		fputs("Minimal terminal size is 80x20.\n", stderr);
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT)
	{
		fputs("Maximum terminal size is 500x300.\n", stderr);
		exit(1);
	}

	switch(axis)
	{
		case 'x':
			return win.ws_col;
		case 'y':
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	uint16_t lines;
	for(lines = 0; lines < windowSize('y'); lines++)
		printf("%s", "\033[F\033[K");
}

static void allocateChars(int8_t chars, int8_t lines, char key)
{
	int8_t char_pos;
	char* line_buffer = malloc(chars * lines * sizeof(char) + 1);
	memCheck(line_buffer);

	if(key != BACKSPACE) // To prevent double 'backspace'.
		text[lines - 1][chars - 1] = key; // Allocation.

	text[lines - 1][chars] = '\0';

	for(char_pos = 1; char_pos <= chars; char_pos++) // String rendering.
		printf("%c", text[lines - 1][char_pos - 1]);

	cursor();
	free(line_buffer);
}

void window(int8_t chars, int8_t lines, char key, char base_filename[])
{
	uint16_t height;
	allocateChars(chars, lines, key);

	for(height = lines; height < windowSize('y'); height++)
		printf("%c", '\n');

	infoBar(chars, lines, base_filename);
}

