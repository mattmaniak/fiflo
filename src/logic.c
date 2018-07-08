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

struct Params readFile(struct Params buff)
{
	buff.chars = 0;
	buff.lines = 1;
	char chr;

	FILE *fd = fopen(filename, "a+");
	pointerCheck(fd, "Cannot open the file, exited.\0");

	while((chr = getc(fd)) != EOF)
	{
		buff.text[buff.chars] = chr;
		buff.chars++;
	}
	fclose(fd);
	return buff;
}

void saveFile(struct Params buff)
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
	/*			if(buff.chars > MAX_CHARS) TODO
				{
					buff.chars = MAX_CHARS;
				}
	*/
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
			break;

			// More special.
			case CTRL_X:
				saveFile(buff);
			break;
		}
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

