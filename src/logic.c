// File with the all elements needed to a runtime.

#include "logic.h"
#include "ui.c"

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
		fputs("Cannot get your current absolute dir, exited.", stderr);
		exit(1);
	}
	if(strlen(path) > 4096 || strlen(basename) > 255)
	{
		fputs("Max. absolute path length is 4096, basename: 255, exited.",
		stderr);
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

struct Params readFromFile(struct Params buff)
{
	char chr;

	FILE *fd = fopen(filename, "ab+");
	pointerCheck(fd, "Cannot open the file, exited.\0");

	while((chr = getc(fd)) != EOF)
	{
		buff.text[buff.chars] = chr;
		buff.chars++;
		printf("%c\n", buff.text[buff.chars]);
	}
	fclose(fd);
	return buff;
}

void saveToFile(struct Params buff)
{
	BUFF_T x;

	FILE *fd = fopen(filename, "w");
	pointerCheck(fd, "Cannot write to the file, exited.\0");

	for(x = 0; x <= buff.chars; x++)
	{
		fprintf(fd, "%c", buff.text[x]);
	}
	fclose(fd);
}

struct Params keyHandling(char key, struct Params buff) // TODO: SHORTEN!
{
	bool empty = 0;
	if(key < 62 || key > 65)
	{
		switch(key)
		{
			default: // Just convert pressed key into a char in the string.
				buff.chars++;
				buff.text[buff.chars - 1] = key;
				buff.text[buff.chars] = TERMINATOR;
				
	/*			if(buff.chars > MAX_CHARS) TODO
				{
					buff.chars = MAX_CHARS;
				}
	*/
			break;

			case TERMINATOR: // Protection from counting '\0'.
				break;

			case LINEFEED:
				buff.text[buff.chars] = LINEFEED;
				buff.lines++;
				if(buff.lines > getSize(Y) - 2)
				{
					buff.lines = getSize(Y) - 2;
					buff.text[buff.chars] = TERMINATOR;
					// Prevent double bar.
				}
			break;

			case BACKSPACE:
				buff.chars--;
				if(buff.chars < 0)
				{
					buff.chars = 0;
				}
				buff.text[buff.chars] = TERMINATOR;
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
	}
	if(key == '\b' && empty == 0) // Set control flag.
	{
		empty = 1;
		buff.chars = 0;
		buff.text[buff.chars] = BACKSPACE;
	}
	return buff;
}

// Drawing funcions.
// Pressed keys to rendered chars in proper order. TODO: ALL KEYS HANDLING.
void renderText(struct Params buff)
{
	BUFF_T x;

	for(x = 0; x <= buff.chars; x++) // Chars rendering.
	{
	// Invert last char color as a integrated cursor.
		if(x == buff.chars - buff.cursor_pos)
		{
			printf("%s%c%s", INVERT, buff.text[x], RESET);
		}
		else
		{
			printf("%c", buff.text[x]);
		}
	}
	if(buff.cursor_pos == 0)
	{
		cursor();
	}

	// TODO: LINES HANDLING.
/*	for(x = 0; x < getSize(X) - buff.chars; x++)
	{
		if(buff.text[x] == LINEFEED)
		{
			printf("%c", 'F');
		}
	}*/
}

// Terminal fill that shows chars and other stupid things.
struct Params window(char key, struct Params buff)
{
	TERM_SIZE y;
	static TERM_SIZE fill = BARS_AMOUNT; // Three bars.

	buff = keyHandling(key, buff);
	fill = autoFill(fill, key, buff);

	upperBar(filename);
	renderText(buff);

	for(y = buff.lines + fill; y <= getSize(Y); y++)
	{
		printf("%c", LINEFEED);
	}
	lowerBar(key, buff);

	return buff;
}

