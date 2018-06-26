#include "logic.h"

#include "ui.c"

BUFF_T lines_c = 1;
BUFF_T chars_c = 0; // text = lines + chars

char text[BUFF_SZ][BUFF_SZ];
char base_filename[510];

void setBaseFilename(char *filename)
{
	uint8_t chr_num;
	char cwd[255];

	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		fputs("Cannot get your current absolute dir.\0", stderr);
		exit(1);
	}
	if(strlen(cwd) > 255)
	{
		fputs("Max. absolute path length: 255.\0", stderr);
		exit(1);
	}

	for(chr_num = 0; chr_num < strlen(cwd); chr_num++) // Copy absolute path.
	{
		base_filename[chr_num] = cwd[chr_num];
	}
	base_filename[strlen(cwd)] = '/'; // Add a slash between.

	for(chr_num = 0; chr_num < strlen(filename); chr_num++) // Copy filename.
	{
		base_filename[chr_num + strlen(cwd) + 1] = filename[chr_num];
	}
}

void saveToFile(BUFF_T lines, BUFF_T chars, char *filename)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	setBaseFilename(filename);

	FILE *file = fopen(base_filename, "w");
	pointerCheck(file);

	for(ln_num = 1; ln_num <= lines; ln_num++) // Y rendering.
	{
		for(chr_num = 0; chr_num < chars; chr_num++) // X rendering.
		{
			fprintf(file, "%c", text[ln_num - 1][chr_num]);
		}
	}
	fclose(file);
}

void keyHandling(char key, char *filename)
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

// Pressed keys to rendered chars in proper order. TODO: all keys handling.
void renderText(BUFF_T lines, BUFF_T chars)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	for(ln_num = 1; ln_num <= lines; ln_num++) // Y rendering.
	{
		for(chr_num = 0; chr_num < chars; chr_num++) // X rendering.
		{
			// Invert last char color as a integrated cursor.
			if(ln_num == lines && chr_num == chars - 1)
			{
				printf("%s%c%s", INVERT, text[ln_num - 1][chr_num], RESET);
			}
			else
			{
				printf("%c", text[ln_num - 1][chr_num]);
			}
		}
	}
}

// Terminal fill that shows chars and other stupid things.
void window(BUFF_T lines, BUFF_T chars, char key)
{
	uint16_t height;
	uint16_t vert_fill = 2; // Two bars.

	upperBar(base_filename);
	renderText(lines, chars);

	if(chars_c == 0)
	{
		vert_fill--;
	}
	for(height = lines; height <= windowSize('y') - vert_fill; height++)
	{
		printf("%c", '\n');
	}
	lowerBar(lines_c, chars_c, key); // chars - 1 - last char index.
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	uint16_t lines;
	for(lines = 0; lines < windowSize('y'); lines++)
	{
		printf("%s", "\033[F\033[K\0");
	}
	fflush(stdout);
}

