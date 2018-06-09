#include "render.h"
#include "ui.c"

// Drawing funcions.
static char text[10][81];

static uint16_t windowSize(char axis) // Check term size and return width or height.
{
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		fputs("Minimal terminal size is 40x20.\n", stderr);
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
	{
		printf("%s", "\033[F\033[K");
	}
}

static void allocateChars(int8_t chars, int8_t lines, char key)
{
	int8_t charPos;
	char* lineBuffer = malloc(chars * lines * sizeof(char) + 1);
	if(lineBuffer == NULL)
	{
		memError();
	}
	if(key != BACKSPACE) // To prevent double 'backspace'.
	{
		text[lines - 1][chars - 1] = key; // TODO: allocates only 79 char.
	}
	text[lines - 1][chars] = '\0';
	/*
	Integration of variables: "chars" and "charPos" is highly required TODO.
	They means the same so there is no need to split.
	*/
	for(charPos = 1; charPos <= chars; charPos++)
	{
		printf("%c", text[lines - 1][charPos - 1]);
	}
	cursor();
	free(lineBuffer);
}

void window(int8_t chars, int8_t lines, char key, char baseFilename[])
{
	uint16_t height;
	allocateChars(chars, lines, key);
	for(height = lines; height < windowSize('y'); height++)
	{
		printf("%c", '\n');
	}
	infoBar(chars, lines, baseFilename);
}

