// File with the all elements needed to a runtime.

#include "logic.h"
#include "ui.c"

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

/*
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
*/

void saveToFile(struct Params buff)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	FILE *textfile = fopen(filename, "w");
	pointerCheck(textfile, "Cannot write to the file, exit.\0");

	for(ln_num = 1; ln_num <= buff.lines; ln_num++) // Lines rendering.
	{
		for(chr_num = 0; chr_num <= buff.chars; chr_num++) // Chars in lines.
		{
			if(text[ln_num - 1][chr_num] != TERMINATOR)
			{
				fprintf(textfile, "%c", text[ln_num - 1][chr_num]);
			}
		}
	}
	fclose(textfile);
}

struct Params keyHandling(char key, struct Params buff)
{
	switch(key)
	{
		default: // Just convert pressed key into a char in the string.
			buff.chars++;
			if(buff.chars > MAX_WIDTH)
			{
				buff.chars = MAX_WIDTH;
			}
			else
			{
				text[CURRENT_LINE][buff.chars - 1] = key;
			}
		break;

		case LINEFEED:
			text[CURRENT_LINE][buff.chars] = LINEFEED;
			buff.lines++;
			if(buff.lines > termSize(Y) - 2)
			{
				buff.lines = termSize(Y) - 2;
				text[CURRENT_LINE][buff.chars] = TERMINATOR;
				// Prevent double bar.
			}
		break;

		case BACKSPACE:
			buff.chars--;
			text[CURRENT_LINE][buff.chars] = TERMINATOR;

			if(buff.chars < 0)
			{
				buff.chars = 0;
			}
			if(buff.lines > 1 && text[UPPER_LINE][buff.chars] == LINEFEED)
			{
				buff.lines--;
				if(buff.lines < 1)
				{
					buff.lines = 1;
				}
				text[CURRENT_LINE][buff.chars] = TERMINATOR;
				// return chars, lines;
			}
		break;

		// More special.
		case CTRL_X:
			saveToFile(buff);
		break;

		case ARROW_LEFT:
			buff.cursor_pos++;
			if(buff.cursor_pos > buff.chars)
			{
				buff.cursor_pos = buff.chars;
			}
		break;

		case ARROW_RIGHT:
			buff.cursor_pos--;
			if(buff.cursor_pos < 1)
			{
				buff.cursor_pos = 1;
			}
		break;
	}
	return buff;
}

// Drawing funcions.

// Pressed keys to rendered chars in proper order. TODO: ALL KEYS HANDLING.
void renderText(struct Params buff)
{
	BUFF_T ln_num;
	BUFF_T chr_num;

	for(ln_num = 1; ln_num <= buff.lines; ln_num++) // Lines rendering.
	{
		for(chr_num = 0; chr_num <= buff.chars; chr_num++) // Chars rendering.
		{
			// Invert last char color as a integrated cursor.
			if(ln_num == buff.lines && chr_num == buff.chars - buff.cursor_pos)
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

void window(char key) // Terminal fill that shows chars and other stupid things.
{
	static struct Params buff = {0, 1, 1}; // Value initializer.
	buff = keyHandling(key, buff);

	WIN_DIMENSION height;
	WIN_DIMENSION fill = 2; // Two bars.
	fill = autoFill(fill, key, buff);

	upperBar(filename);
	renderText(buff);

	for(height = buff.lines; height <= termSize(Y) - fill; height++)
	{
		printf("%c", LINEFEED);
	}
	lowerBar(key, buff);
}

