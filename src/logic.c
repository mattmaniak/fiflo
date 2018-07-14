// Shows how the program single round works.

#include "logic.h"
#include "ui.c"

void setFilename(struct Data buff, char *name)
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	if(name[0] == '/')
	{
		strcpy(buff.filename, name);
	}
	else
	{
		uint16_t chr;
		char *path = malloc(4097); // TODO: SIZE

		if(getcwd(path, 4097) == NULL)
		{
			fputs("Cannot get your current dir, exited.\n", stderr);
			exit(1);
		}
		path[strlen(path)] = TERMINATOR;

		for(chr = 0; chr < strlen(path); chr++) // Copy cwd.
		{
			buff.filename[chr] = path[chr];
		}
		buff.filename[strlen(path)] = '/'; // Add a slash between.

		for(chr = 0; chr < strlen(name); chr++) // Copy bname.
		{
			buff.filename[chr + strlen(path) + 1] = name[chr];
		}
		buff.filename[strlen(path) + strlen(name) + 1] = TERMINATOR;
	}
}

struct Data readFile(struct Data buff, char *name)
{
	buff.chars = 0;
	buff.lines = 1;
	char chr;

	buff.filename = malloc(4096 + 256);
	setFilename(buff, name);

	FILE *fd = fopen(buff.filename, "a+");
	pointerCheck(fd, "Cannot open the file, exited.\0");

	while((chr = getc(fd)) != EOF)
	{
		buff.text[buff.chars] = chr;
		buff.chars++;
		if(chr == LINEFEED)
		{
			buff.lines++;
		}
	}
	fclose(fd);
	return buff;
}

void saveFile(struct Data buff)
{
	buff_t x;

	FILE *fd = fopen(buff.filename, "w");
	pointerCheck(fd, "Cannot write to the file, exited.\0");

	for(x = 0; x <= buff.chars; x++)
	{
		if(buff.text[x] != TERMINATOR)
		{
			fprintf(fd, "%c", buff.text[x]);
		}
	}
	fclose(fd);
}

struct Data allocText(struct Data buff, char key)
{
	if(key == LINEFEED || key == CTRL_X || key >= 32)
	{
		switch(key)
		{
			default: // Convert pressed key into a char in the string.
				buff.text[buff.chars] = key;
				if(key != TERMINATOR)
				{
					buff.chars++;
				}
				if(buff.chars > MAX_CHARS)
				{
					buff.chars = MAX_CHARS;
				}
			break;

			case LINEFEED:
				buff.text[buff.chars] = LINEFEED;
				buff.chars++;
				if(buff.chars > MAX_CHARS)
				{
					buff.chars = MAX_CHARS;
				}
				buff.lines++;
			break;

			case BACKSPACE:
				buff.text[buff.chars] = TERMINATOR;
				buff.chars--;
				if(buff.chars < 0)
				{
					buff.chars = 0;
				}
				if(buff.text[buff.chars] == LINEFEED)
				{
					buff.lines--;
				}
			break;

			case CTRL_X:
				saveFile(buff);
			break;
		}
	}
	linesLimit(buff.lines);
	return buff;
}

// Drawing funcions.
// Pressed keys to rendered chars in proper order. TODO: ALL KEYS HANDLING.
void renderText(struct Data buff)
{
	buff_t x;

	if(buff.text[0] == LINEFEED)
	{
		printf("%c", LINEFEED); // Necessary at least for LXTerminal.
	}
	for(x = 0; x < buff.chars; x++) // Chars rendering.
	{
		printf("%c", buff.text[x]);
	}
	cursor();
}

// Terminal fill that shows chars and other stupid things.
struct Data window(char key, struct Data buff)
{
	static term_t fill = BARS_SZ;
	term_t y;

	buff = allocText(buff, key);
	fill = autoFill(buff, key, fill);

	bar(buff, key);
	renderText(buff);

	for(y = buff.lines + fill; y < getSize(Y); y++)
	{
		printf("%c", LINEFEED);
	}
	return buff;
}

