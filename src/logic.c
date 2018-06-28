// File with the all elements needed to a runtime.

#include "logic.h"
#include "ui.c"

BUFF_T lines_c = 1;
BUFF_T chars_c = 0; // text = lines + chars

char text[BUFF_SZ][MAX_WIDTH + 1];
char base_filename[512]; // 255 (cwd) + 1 (slash) + 255 (filename) + 1 (null).

void setBaseFilename(char *filename) // TODO: SIMPLIFY NAMING.
{
	uint8_t chr_num;
	char cwd[256];

	cwd[strlen(cwd)] = TERMINATOR;

	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		fputs("Cannot get your current absolute dir.", stderr);
		exit(1);
	}
	if(strlen(cwd) > 255 || strlen(filename) > 255)
	{
		fputs("Max. absolute path or filename length is 255.", stderr);
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
	base_filename[strlen(cwd) + strlen(filename) + 1] = TERMINATOR;
}

void readFromFile(void) // TODO
{
	FILE *textfile = fopen(base_filename, "ab+");
	pointerCheck(textfile, "Cannot open the file, exit.\0");

	while(getc(textfile) != EOF)
	{
		printf("%c", getc(textfile));
	}
	fclose(textfile);
}

void saveToFile(void)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	FILE *textfile = fopen(base_filename, "w");
	pointerCheck(textfile, "Cannot write to the file, exit.\0");

	for(ln_num = 1; ln_num <= lines_c; ln_num++) // Lines rendering.
	{
		for(chr_num = 0; chr_num <= chars_c; chr_num++) // Chars in lines.
		{
			if(text[ln_num - 1][chr_num] != TERMINATOR)
			{
				fprintf(textfile, "%c", text[ln_num - 1][chr_num]);
			}
		}
	}
	fclose(textfile);
}

void keyHandling(char key)
{
	switch(key)
	{
		default:  // Just convert pressed key into a char in the string.
			chars_c++;
			if(chars_c > MAX_WIDTH)
			{
				chars_c = MAX_WIDTH;
			}
			else
			{
				text[CURRENT_LINE][chars_c - 1] = key;
			}
		break;

		case NEWLINE:
			text[CURRENT_LINE][chars_c] = NEWLINE;
			lines_c++;
			if(lines_c > windowSize('y') - 1)
			{
				lines_c = windowSize('y') - 1;
			}
			// TODO: SCREEN LIMIT OR SCROLLING.
		break;

		case BACKSPACE:
			chars_c--;
			text[CURRENT_LINE][chars_c] = TERMINATOR;

			if(chars_c < 0)
			{
				chars_c = 0;
			}
			if(lines_c > 1 && text[UPPER_LINE][chars_c] == NEWLINE)
			{
				lines_c--;
				if(lines_c < 1)
				{
					lines_c = 1;
				}
				text[CURRENT_LINE][chars_c] = TERMINATOR;
			}
		break;

		case CTRL_X:
			saveToFile();
		break;
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
void renderText(void) // TODO: BACKSPACE LINE_C-- FIX.
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	for(ln_num = 1; ln_num <= lines_c; ln_num++) // Y rendering.
	{
		for(chr_num = 0; chr_num <= chars_c; chr_num++) // X rendering.
		{
			// Invert last char color as a integrated cursor.
			if(ln_num == lines_c && chr_num == chars_c - 1)
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
void window(char key)
{
	uint16_t height;
	uint16_t fill = 2; // Two bars.

	upperBar(base_filename);
	renderText();

	if(chars_c == 0 || text[0][0] == NEWLINE) // No visible char.
	{
		fill = 1;
	}

	for(height = lines_c; height <= windowSize('y') - fill; height++)
	{
		printf("%c", NEWLINE);
	}
	lowerBar(lines_c, chars_c, key); // chars - 1 - last char index.
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	uint16_t lines;
	for(lines = 0; lines < windowSize('y'); lines++)
	{
		printf("%s", "\033[F\033[K");
	}
	fflush(stdout);
}

