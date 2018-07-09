#include "logic.h"
#include "ui.c"

//char filename[4353]; // 4096 (path) + 1 (slash) + 255 (bname) + 1 (null).
// TODO

void setFilename(struct Params buff, const char *bname) // TODO: SIMPLIFY!
{
	// Filename = absolute path + bname eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	uint16_t chr;
	char path[4096];

	if(getcwd(path, sizeof(path)) == NULL)
	{
		fputs("Cannot get your current absolute dir, exited.\n", stderr);
		exit(1);
	}
	if(strlen(path) > 4096 || strlen(bname) > 255)
	{
		fputs("Max. absolute path length is 4096, bname: 255, exited.\n",
		stderr);
		exit(1);
	}
	path[strlen(path)] = TERMINATOR;

	for(chr = 0; chr < strlen(path); chr++) // Copy cwd.
	{
		buff.filename[chr] = path[chr];
	}
	buff.filename[strlen(path)] = '/'; // Add a slash between.

	for(chr = 0; chr < strlen(bname); chr++) // Copy bname.
	{
		buff.filename[chr + strlen(path) + 1] = bname[chr];
	}
	buff.filename[strlen(path) + strlen(bname) + 1] = TERMINATOR;
}

struct Params readFile(struct Params buff, const char *bname)
{
	char chr;
	buff.chars = 0;
	buff.lines = 1;

	buff.filename = malloc(4096 + 255 + 1);
	setFilename(buff, bname);

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
	free(buff.filename);

	return buff;
}
/*
void saveFile(struct Params buff)
{
	BUFF_T x;

	FILE *fd = fopen(filename, "w");
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
*/
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
				//saveFile(buff);
			break;
		}
	}
	if(buff.lines > getSize(Y) - 3)
	{
		fprintf(stderr, "%s%i%s%i%s", "Max. lines amount: ", getSize(Y) - 3,
		", got: ", buff.lines, ". Stretch your terminal or sth.\n");
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
	static TERM_SIZE fill = BARS_AMOUNT; // Three bars.

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

