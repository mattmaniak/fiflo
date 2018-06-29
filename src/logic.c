// File with the all elements needed to a runtime.

#include "logic.h"
#include "ui.c"

// TODO: EACH LINE SHOULD HAVE OTHER CHARS PARAM INSTEAD OF UNIVERSAL CHARS_C.

BUFF_T lines_c = 1;
BUFF_T chars_c = 0; // text = lines + chars
BUFF_T cursor_pos = 1;

char text[BUFF_SZ][MAX_WIDTH + 1];
char filename[512]; // 255 (cwd) + 1 (slash) + 255 (basename) + 1 (null).

void setFilename(const char *basename)
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Base filename (basename) eg. "my_file".
	uint8_t chr_num;
	char path[256];

	if(getcwd(path, sizeof(path)) == NULL)
	{
		fputs("Cannot get your current absolute dir.", stderr);
		exit(1);
	}
	if(strlen(path) > 255 || strlen(basename) > 255)
	{
		fputs("Max. absolute path or basename length is 255.", stderr);
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
		if(chr == NEWLINE)
		{
			lines_c++;
			text[CURRENT_LINE][chars_c - 1] = chr;
		}
		else
		{
			chars_c++;
			text[CURRENT_LINE][chars_c - 1] = chr;
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
		default: // Just convert pressed key into a char in the string.
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
			if(lines_c > termSize('Y') - 2)
			{
				lines_c = termSize('Y') - 2;
				text[CURRENT_LINE][chars_c] = TERMINATOR; // Prevent double bar.
			}
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

		// More special.
		case CTRL_X:
			saveToFile();
		break;

		case ARROW_LEFT:
			cursor_pos++;
		break;

		case ARROW_RIGHT:
			cursor_pos--;
			if(cursor_pos < 1)
			{
				cursor_pos = 1;
			}
		break;
	}
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
void renderText(void)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	for(ln_num = 1; ln_num <= lines_c; ln_num++) // Lines rendering.
	{
		for(chr_num = 0; chr_num <= chars_c; chr_num++) // Chars rendering.
		{
			// Invert last char color as a integrated cursor.
			if(ln_num == lines_c && chr_num == chars_c - cursor_pos)
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

void autoNewline(char key)
{
	if(chars_c > 1)
	{
		if(key != BACKSPACE &&  chars_c % termSize('X') == 1)
		{
			lines_c++;
		}
		else if(key == BACKSPACE && chars_c % termSize('X') == 0)
		{
			lines_c--;
		}
	}
}

void window(char key) // Terminal fill that shows chars and other stupid things.
{
	uint16_t height;
	uint16_t fill = 2; // Two bars.

	if(chars_c == 0 || text[0][0] == NEWLINE) // No visible char.
	{
		fill = 1;
	}
	autoNewline(key);

	upperBar(filename);
	renderText();

	for(height = lines_c; height <= termSize('Y') - fill; height++)
	{
		printf("%c", NEWLINE);
	}
	lowerBar(lines_c, chars_c, key); // chars - 1 - last char index.
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

