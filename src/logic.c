// File with the all elements needed to a runtime.

#include "logic.h"
#include "ui.c"

// TODO: EACH LINE SHOULD HAVE OTHER CHARS PARAM INSTEAD OF UNIVERSAL chars.

BUFF_T lines = 1;
BUFF_T chars = 0; // text = lines + chars
BUFF_T cursor_pos = 1; // 0 is non-printed char eg. '\n'.

char text[BUFF_SZ][MAX_WIDTH + 1];
char filename[4353]; // 4096 (path) + 1 (slash) + 255 (basename) + 1 (null).
// TODO

void setFilename(const char *basename) // TODO: SIMPLIFY!
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	uint8_t chr_num;
	char path[4096];

	if(getcwd(path, sizeof(path)) == NULL)
	{
		fputs("Cannot get your current absolute dir.", stderr);
		exit(1);
	}
	if(strlen(path) > 4096 || strlen(basename) > 255)
	{
		fputs("Max. absolute path length is 4096, basename: 255.", stderr);
		exit(1);
	}
	path[strlen(path)] = TERMINATOR;

	for(chr_num = 0; chr_num < strlen(path); chr_num++) // Copy cwd.
	{
		filename[chr_num] = path[chr_num];
	}
	filename[strlen(path)] = '/'; // Add a slash between.

	for(chr_num = 0; chr_num < strlen(basename); chr_num++) // Copy basename.
	{
		filename[chr_num + strlen(path) + 1] = basename[chr_num];
	}
	filename[strlen(path) + strlen(basename) + 1] = TERMINATOR;
}

void readFromFile(void)
{
	char chr;

	FILE *textfile = fopen(filename, "ab+");
	pointerCheck(textfile, "Cannot open the file, exit.\0");

	while((chr = getc(textfile)) != EOF)
	{
		if(chr == LINEFEED)
		{
			lines++;
			text[CURRENT_LINE][chars - 1] = chr;
		}
		else
		{
			chars++;
			text[CURRENT_LINE][chars - 1] = chr;
		}
	}
	fclose(textfile);
}

void saveToFile(void)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	FILE *textfile = fopen(filename, "w");
	pointerCheck(textfile, "Cannot write to the file, exit.\0");

	for(ln_num = 1; ln_num <= lines; ln_num++) // Lines rendering.
	{
		for(chr_num = 0; chr_num <= chars; chr_num++) // Chars in lines.
		{
			if(text[ln_num - 1][chr_num] != TERMINATOR)
			{
				fprintf(textfile, "%c", text[ln_num - 1][chr_num]);
			}
		}
	}
	fclose(textfile);
}

Buffer keyHandling(char key)
{
	Buffer buff = {chars, lines, cursor_pos};
	switch(key)
	{
		default: // Just convert pressed key into a char in the string.
			chars++;
			if(chars > MAX_WIDTH)
			{
				chars = MAX_WIDTH;
			}
			else
			{
				text[CURRENT_LINE][chars - 1] = key;
			}
		break;

		case LINEFEED:
			text[CURRENT_LINE][chars] = LINEFEED;
			lines++;
			if(lines > termSize('Y') - 2)
			{
				lines = termSize('Y') - 2;
				text[CURRENT_LINE][chars] = TERMINATOR; // Prevent double bar.
			}
		break;

		case BACKSPACE:
			chars--;
			text[CURRENT_LINE][chars] = TERMINATOR;

			if(chars < 0)
			{
				chars = 0;
			}
			if(lines > 1 && text[UPPER_LINE][chars] == LINEFEED)
			{
				lines--;
				if(lines < 1)
				{
					lines = 1;
				}
				text[CURRENT_LINE][chars] = TERMINATOR;
				// return chars, lines;
			}
		break;

		// More special.
		case CTRL_X:
			saveToFile();
		break;

		case ARROW_LEFT:
			cursor_pos++;
			if(cursor_pos > chars)
			{
				cursor_pos = chars;
			}
		break;

		case ARROW_RIGHT:
			cursor_pos--;
			if(cursor_pos < 1)
			{
				cursor_pos = 1;
			}
		break;
	}
	return buff;
}

// Drawing funcions.
uint16_t termSize(char axis) // Check terminal size.
{
	struct winsize win;
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
		"Max. term size is ", MAX_WIDTH, 'x', MAX_HEIGHT, ".\n");
		exit(1);
	}

	switch(axis)
	{
		case 'X':
			return win.ws_col;

		case 'Y':
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

// Pressed keys to rendered chars in proper order. TODO: ALL KEYS HANDLING.
void renderText(char key)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	Buffer buff = keyHandling(key);

	for(ln_num = 1; ln_num <= lines; ln_num++) // Lines rendering.
	{
		for(chr_num = 0; chr_num <= chars; chr_num++) // Chars rendering.
		{
			// Invert last char color as a integrated cursor.
			if(ln_num == lines && chr_num == chars - cursor_pos)
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

uint16_t autoFill(uint16_t fill, char key)
{
	if(chars == 0 || text[0][0] == LINEFEED) // No visible char.
	{
		fill = 1;
	}
	else if(chars > 1)
	{
		if(key != BACKSPACE &&  chars % termSize('X') == 1)
		{
			lines++;
		}
		else if(key == BACKSPACE && chars % termSize('X') == 0)
		{
			lines--;
		}
	}
	return fill;
}

void window(char key) // Terminal fill that shows chars and other stupid things.
{
	/* TODO: IMPLEMENTATION, NO GLOBALS
	struct Buffers
	{
		BUFF_T lines;
		BUFF_T chars;
		BUFF_T cursor_pos;
	};
	struct Buffers buff;
	*/

	uint16_t height;
	uint16_t fill = 2; // Two bars.
	fill = autoFill(fill, key);

	upperBar(filename);
	renderText(key);

	for(height = lines; height <= termSize('Y') - fill; height++)
	{
		printf("%c", LINEFEED);
	}
	lowerBar(lines, chars, key); // chars - 1 - last char index.
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	uint16_t lines;
	for(lines = 0; lines < termSize('Y'); lines++)
	{
		printf("%s", "\033[F\033[K");
	}
	fflush(stdout);
}

