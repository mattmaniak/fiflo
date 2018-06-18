#include "logic.h"

#include "keys.c"
#include "ui.c"

int8_t lines_amount = 1, chars_amount = 1; // text = lines + chars

void saveToFile(LINES_CHARS_KEY_FILENAME)
{
	int8_t line_pos;
	int8_t char_pos;

	FILE* file = fopen(filename, "w");
	pointerCheck(file);

	for(line_pos = 1; line_pos <= lines; line_pos++) // Y rendering.
	{
		for(char_pos = 1; char_pos <= chars; char_pos++) // X rendering.
		{
			fprintf(file, "%c", text[line_pos - 1][char_pos - 1]);
		}
	}
	fclose(file);
}

void keyCheck(LINES_CHARS_KEY_FILENAME)
{
	if(key == CTRL_X) // Check if exit key is pressed.
	{
		saveToFile(lines, chars, key, filename);
		exit(1);
	}
	else if(key == ENTER) // Change to ENTER will render old strings.
	{
		lines_amount++;
		if(lines_amount >= 19) // 19 only for testing.
		{
			lines_amount = 19; // Will be BUFFER_SIZE.
		}
	}
	else if(key == BACKSPACE && text[lines_amount - 1][chars_amount - 1] == '\n')
	{
		lines_amount--;
		if(lines_amount <= 1)
		{
			lines_amount = 1;
		}
	}
	else if(key == BACKSPACE) // Check if user want to remove a last char.
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
uint16_t windowSize(char axis) // Check terminal size.
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

// Pressed keys to rendered chars in proper order. TODO: all keys handling.
void allocChars(LINES_CHARS_KEY_FILENAME)
{
	int8_t line_pos;
	int8_t char_pos;

	char* text_buffer = malloc(chars * lines * sizeof(char) + 1);
	pointerCheck(text_buffer);

	keyHandling(lines, chars, key);

	for(line_pos = 1; line_pos <= lines; line_pos++) // Y rendering.
	{
		for(char_pos = 1; char_pos <= chars; char_pos++) // X rendering.
		{
			printf("%c", text[line_pos - 1][char_pos - 1]);
		}
	}
	cursor();
	free(text_buffer);
}

void initWindow(int8_t lines, int8_t chars, char filename[])
{
	uint16_t height;

	upperBar();
	cursor();

	for(height = lines; height <= windowSize('y') - 2; height++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines, chars, '\0', filename);
}

// Terminal filler that shows chars and another stupid things.
void window(LINES_CHARS_KEY_FILENAME)
{
	uint16_t height;
	uint16_t vertical_filler = 2;

	upperBar();
	allocChars(lines, chars, key, filename);

	if(chars % windowSize('x') >= 0)
	{
		vertical_filler++;
	}
	else if(chars % windowSize('x') <= 0)
	{
		vertical_filler--;
	}

	for(height = lines; height <= windowSize('y') - vertical_filler; height++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines, chars, key, filename);
}

