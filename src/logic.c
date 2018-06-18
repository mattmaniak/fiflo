#include "logic.h"

#include "ui.c"

BUFF_T lines_c = 1;
BUFF_T chars_c = 1; // text = lines + chars

char text[19][BUFF_SZ];

void keyHandling(BUFF_T lines, BUFF_T chars, char key)
{
	if(key == BACKSPACE) // To prevent double 'backspace'.
	{
		text[lines - 1][chars] = '\0';
	}
	else
	{
		text[lines - 1][chars - 1] = key; // Allocation.
		text[lines - 1][chars] = '\0';
	}
}

void saveToFile(BUFF_T lines, BUFF_T chars, char filename[])
{
	BUFF_T line_pos;
	BUFF_T char_pos;

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

void keyCheck(BUFF_T lines, BUFF_T chars, char key, char filename[])
{
	if(key == CTRL_X) // Check if exit key is pressed.
	{
		saveToFile(lines, chars, filename);
		exit(1);
	}
	else if(key == ENTER) // Change to ENTER will render old strings.
	{
		lines_c++;
		if(lines_c >= 19) // 19 only for testing.
		{
			lines_c = 19; // Will be BUFF_SZ.
		}
	}
	else if(key == BACKSPACE && text[lines_c - 1][chars_c - 1] == '\n')
	{
		lines_c--;
		if(lines_c <= 1)
		{
			lines_c = 1;
		}
	}
	else if(key == BACKSPACE) // Check if user want to remove a last char.
	{
		chars_c--;
		if(chars_c <= 0)
		{
			chars_c = 0;
		}
	}
	else if(key != CTRL_N)
	{
		chars_c++;
		if(chars_c >= BUFF_SZ)
		{
			chars_c = BUFF_SZ; // TODO: the last char is overwritten.
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
void allocChars(BUFF_T lines, BUFF_T chars, char key)
{
	BUFF_T line_pos;
	BUFF_T char_pos;

	char* text_BUFF = malloc(chars * lines * sizeof(char) + 1);
	pointerCheck(text_BUFF);

	keyHandling(lines, chars, key);

	for(line_pos = 1; line_pos <= lines; line_pos++) // Y rendering.
	{
		if(line_pos == 1)
		{
			printf("%i", line_pos);
		}
		// TODO: lines numeration.

		for(char_pos = 1; char_pos <= chars; char_pos++) // X rendering.
		{
			printf("%c", text[line_pos - 1][char_pos - 1]);
		}
	}
	cursor();
	free(text_BUFF);
}

void initWindow(BUFF_T lines, BUFF_T chars, char filename[])
{
	uint16_t current;
	int8_t longestLineNum = decimalIntLen(lines);
	int8_t lineNumLen;
	int8_t i;

	upperBar();	

	printf("%c%i%c", ' ', 1, ' ');
	cursor();
	printf("%c", '\n');

	for(current = lines; current <= windowSize('y') - 3; current++)
	{
		lineNumLen = decimalIntLen(current);

		for(i = 0; i <= longestLineNum - lineNumLen; i++)
		{
			printf("%c", ' ');
		}
		printf("%i%s", current + 1, " \n");
	}
	lowerBar(lines, chars, '\0', filename);
}

// Terminal filler that shows chars and another stupid things.
void window(BUFF_T lines, BUFF_T chars, char key, char filename[])
{
	uint16_t height;
	uint16_t vertical_filler = 2;

	upperBar();
	allocChars(lines, chars, key);

	if(key != BACKSPACE && chars % windowSize('x') == 0)
	{
		vertical_filler++;
	}

	for(height = lines; height <= windowSize('y') - vertical_filler; height++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines, chars, key, filename);
}

