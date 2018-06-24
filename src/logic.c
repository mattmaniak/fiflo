#include "logic.h"

#include "ui.c"

BUFF_T lines_c = 1;
BUFF_T chars_c = 0; // text = lines + chars

char text[BUFF_SZ][BUFF_SZ];

void saveToFile(BUFF_T lines, BUFF_T chars, char filename[])
{
	BUFF_T line_pos;
	BUFF_T char_pos;

	FILE* file = fopen(filename, "w");
	pointerCheck(file);

	for(line_pos = 1; line_pos <= lines; line_pos++) // Y rendering.
	{
		for(char_pos = 0; char_pos < chars; char_pos++) // X rendering.
		{
			fprintf(file, "%c", text[line_pos - 1][char_pos]);
		}
	}
	fclose(file);
}

void keyHandling(char key, char filename[])
{
	if(key == CTRL_X)
	{
		saveToFile(lines_c, chars_c, filename);
	}
	else if(key == ENTER)
	{
		text[lines_c - 1][chars_c] = '\n';
		lines_c++;
		// TODO: SCREEN LIMIT OR SCROLLING.
	}
	else if(key == BACKSPACE)
	{
		text[lines_c - 1][chars_c] = '\0';
		chars_c--;
		
		if(chars_c <= 0)
		{
			chars_c = 0;
		}
		if(lines_c > 1 && text[lines_c - 2][chars_c] == '\n') // Delete line.
		{
			text[lines_c - 1][chars_c] = '\0';
			lines_c--;
			if(lines_c <= 1)
			{
				lines_c = 1;
			}
		}
	}
	else if(key != ENTER)
	{
		text[lines_c - 1][chars_c] = key; // Allocation.
		text[lines_c - 1][chars_c + 1] = '\0';
		chars_c++;
		if(chars_c >= BUFF_SZ)
		{
			chars_c = BUFF_SZ; // TODO: THE LAST CHAR IS OVERWRITTEN.
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
void renderText(BUFF_T lines, BUFF_T chars)
{
	BUFF_T line_pos;
	BUFF_T char_pos;

	char* text_buff = malloc(chars * lines * (sizeof(char) + 1));
	pointerCheck(text_buff);

	for(line_pos = 1; line_pos <= lines; line_pos++) // Y rendering.
	{
		for(char_pos = 0; char_pos < chars; char_pos++) // X rendering.
		{
			// Invert last char color as a integrated cursor.
			if(line_pos == lines && char_pos == chars - 1)
			{
				printf("%s%c%s", INVERT, text[line_pos - 1][char_pos], RESET);
			}
			else
			{
				printf("%c", text[line_pos - 1][char_pos]);
			}
		}
	}
	free(text_buff);
}

void initWindow(BUFF_T lines, BUFF_T chars, char filename[])
{
	uint16_t current;

	upperBar();	
	printf("%c", '\n');

	for(current = lines; current <= windowSize('y') - 2; current++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines, chars, '\0', filename);
}

// Terminal fill that shows chars and other stupid things.
void window(BUFF_T lines, BUFF_T chars, char key, char filename[])
{
	uint16_t height;
	uint16_t vert_fill = 2; // Two bars.

	upperBar();
	renderText(lines, chars);

/*	if(key != BACKSPACE && chars % windowSize('x') == 0)
	{
		text[lines - 1][windowSize('x')] = '\n';
		lines_c++;
		vert_fill++;
	}
	else if(key == BACKSPACE && chars % windowSize('x') == 0)
	{
		text[lines - 1][windowSize('x')] = '\0';
		lines_c--;
		vert_fill--;
	}
*/

	if(chars_c == 0)
	{
		vert_fill--;
	}
	for(height = lines; height <= windowSize('y') - vert_fill; height++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines_c, chars_c, key, filename); // chars - 1 - current index.
}

