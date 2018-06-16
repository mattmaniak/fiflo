#include "render.h"
#include "keys.c"
#include "ui.c"

int8_t lines_amount = 1, chars_amount = 1; // text = lines + chars

void keyCheck(char key) // TODO: simplify these ifs! Move it to the keys.c!
{
	if(key == CTRL_X) // Check if exit key is pressed.
	{
		cleanFrame();
		exit(0);
	}
	if(key == ENTER) // Change to ENTER will render old strings.
	{
		lines_amount++;
		if(lines_amount >= 19) // 19 only for testing.
		{
			lines_amount = 19; // Will be BUFFER_SIZE.
		}
	}
	if(key == BACKSPACE) // Check if user want to remove a last char.
	{
		chars_amount--;
		if(chars_amount <= 0)
		{
			chars_amount = 0;
		}
	}
	else if(key != CTRL_N)
	{
		chars_amount++;
		if(chars_amount >= BUFFER_SIZE)
		{
			chars_amount = BUFFER_SIZE; // TODO: the last char is overwritten.
		}
	}
}

// Drawing funcions.
static uint16_t windowSize(char axis) // Check terminal size.
{
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		fputs("Min. term size is 80x20.\n", stderr);
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT)
	{
		fputs("Max. term size is 500x300.\n", stderr);
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

// Pressed keys to rendered chars in proper order. TODO: key handling.
static void allocateChars(int8_t lines, int8_t chars, char key)
{
	int8_t line_pos, char_pos; // Iterators.
	char* text_buffer = malloc(chars * lines * sizeof(char) + 1);

	pointerCheck(text_buffer);
	keyHandling(lines, chars, key);

	for(line_pos = 1; line_pos <= lines; line_pos++) // TODO: sth wrong.
	{
		for(char_pos = 1; char_pos <= chars; char_pos++) // String rendering.
		{
			printf("%c", text[line_pos - 1][char_pos - 1]);
		}
	}
	cursor();
	free(text_buffer);
}

// Terminal filler that shows chars and another stupid things.
void window(int8_t lines, int8_t chars, char key, char base_filename[])
{
	upperBar();
	uint16_t height;
	allocateChars(lines, chars, key);

	for(height = lines; height <= windowSize('y') - 2; height++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines, chars, key, base_filename);
}

