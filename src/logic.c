#include "logic.h"
#include "ui.c"

//char filename[4353]; // 4096 (path) + 1 (slash) + 255 (bname) + 1 (null).
// TODO

void setFilename(struct Params buff, char *name) // TODO: SIMPLIFY!
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
		char *path = malloc(4097);
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

struct Params readFile(struct Params buff, char *name)
{
	char chr;
	buff.chars = 0;
	buff.lines = 1;

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

void saveFile(struct Params buff)
{
	BUFF_T x;

	FILE *fd = fopen(buff.filename, "w");
	pointerCheck(fd, "Cannot write to the file, exited.\0");

	puts(buff.filename);

	for(x = 0; x <= buff.chars; x++)
	{
		if(buff.text[x] != TERMINATOR)
		{
			fprintf(fd, "%c", buff.text[x]);
		}
	}
	fclose(fd);
}

struct Params allocText(char key, struct Params buff) // TODO: SHORTEN!
{
	if(KEYMAP)
	{
		switch(key)
		{
			default: // Just convert pressed key into a char in the string.
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
				buff.lines++;
				if(buff.lines > getSize(Y) - 2)
				{
					buff.lines = getSize(Y) - 2;
					buff.text[buff.chars] = TERMINATOR;
					// Prevent double bar.
				}
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

			// More special.
			case CTRL_X:
				saveFile(buff);
			break;
		}
	}
	if(buff.lines > getSize(Y) - BARS_SZ)
	{
		fprintf(stderr, "%s%i%s%i%s",
		"Max. lines amount: ", getSize(Y - BARS_SZ), ", got: ", buff.lines,
		". Stretch your terminal or sth.\n");
		exit(1);
	}
	return buff;
}

// Drawing funcions.
// Pressed keys to rendered chars in proper order. TODO: ALL KEYS HANDLING.
void renderText(struct Params buff)
{
	BUFF_T x;

	for(x = 0; x < buff.chars; x++) // Chars rendering.
	{
		printf("%c", buff.text[x]);
	}
	// TODO: LINES HANDLING.
	cursor();
}

// Terminal fill that shows chars and other stupid things.
struct Params window(char key, struct Params buff)
{
	TERM_SIZE y;
	static TERM_SIZE fill = BARS_SZ; // Three bars.

	buff = allocText(key, buff);
	fill = autoFill(fill, key, buff);

	upperBar(buff.filename);
	renderText(buff);

	for(y = buff.lines + fill; y < getSize(Y); y++)
	{
		printf("%c", LINEFEED);
	}
	lowerBar(key, buff);

	return buff;
}

