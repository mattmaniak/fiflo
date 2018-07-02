// File with the all elements needed to a runtime.

#include "logic.h"
#include "ui.c"

char text[BUFF_SZ];
char filename[4353]; // 4096 (path) + 1 (slash) + 255 (basename) + 1 (null).
// TODO

void setFilename(const char *basename) // TODO: SIMPLIFY!
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	uint16_t chr;
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

	for(chr = 0; chr < strlen(path); chr++) // Copy cwd.
	{
		filename[chr] = path[chr];
	}
	filename[strlen(path)] = '/'; // Add a slash between.

	for(chr = 0; chr < strlen(basename); chr++) // Copy basename.
	{
		filename[chr + strlen(path) + 1] = basename[chr];
	}
	filename[strlen(path) + strlen(basename) + 1] = TERMINATOR;
}

void readFromFile(struct Params buff)
{
	char chr;

	FILE *textfile = fopen(filename, "ab+");
	pointerCheck(textfile, "Cannot open the file, exit.\0");

	while((chr = getc(textfile)) != EOF)
	{
//		if(chr == LINEFEED)
//		{
			buff.chars++;
			text[buff.chars] = chr;
/*		}
		else
		{
			buff.chars++;
			text[buff.chars] = chr;
		}
*/	}
	fclose(textfile);
}

void saveToFile(struct Params buff)
{
	BUFF_T x;
	BUFF_T y;

	FILE *textfile = fopen(filename, "w");
	pointerCheck(textfile, "Cannot write to the file, exit.\0");

	for(y = 1; y <= buff.lines; y++) // Lines rendering.
	{
		for(x = 0; x <= buff.chars; x++) // Chars in lines.
		{
//			if(text[x] != TERMINATOR)
//			{
				fprintf(textfile, "%c", text[x]);
//			}
		}
	}
	fclose(textfile);
}

struct Params keyHandling(char key, struct Params buff)
{
	switch(key)
	{
		default: // Just convert pressed key into a char in the string.
			text[buff.chars] = key;

			buff.chars++;
			if(buff.chars > MAX_WIDTH)
			{
				buff.chars = MAX_WIDTH;
			}

		break;

		case LINEFEED:
			text[buff.chars] = LINEFEED;
			buff.lines++;
			if(buff.lines > getSize(Y) - 2)
			{
				buff.lines = getSize(Y) - 2;
				text[buff.chars] = TERMINATOR;
				// Prevent double bar.
			}
		break;

		case BACKSPACE:
			buff.chars--;

			if(buff.chars < 0)
			{
				buff.chars = 0;
			}
			text[buff.chars] = TERMINATOR;
/*			if(buff.lines > 1 && text[buff.chars] == LINEFEED)
			{
				buff.lines--;
				if(buff.lines < 1)
				{
					buff.lines = 1;
				}
				text[buff.chars] = TERMINATOR;
			}
*/		break;

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
	BUFF_T x;
//	BUFF_T y;

//	for(y = 1; y <= buff.lines; y++) // Lines rendering.
//	{
		for(x = 0; x <= buff.chars; x++) // Chars rendering.
		{
			// Invert last char color as a integrated cursor.
			if(/*y == buff.lines && */x == buff.chars - buff.cursor_pos)
			{
				printf("%s%c%s", INVERT, text[x], RESET);
			}
			else
			{
				printf("%c", text[x]);
			}
		}
//	}
}

void window(char key) // Terminal fill that shows chars and other stupid things.
{
	TERM_SIZE y;
	TERM_SIZE fill = 2; // Two bars.

	static struct Params buff = {0, 1, 1}; // Value initializer.
//	readFromFile(buff); // TODO: IS IN A LOOP?

	buff = keyHandling(key, buff);
	fill = autoFill(fill, key, buff);

	upperBar(filename);
	renderText(buff);

	for(y = buff.lines + fill; y <= getSize(Y); y++)
	{
		printf("%c", LINEFEED);
	}
	lowerBar(key, buff);
}

