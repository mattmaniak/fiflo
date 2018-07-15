// Shows how the program single round works.

#include "logic.h"
#include "ui.c"

void setFilename(struct Data buff, char *name)
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	const uint8_t terminator_sz = 1;
	const uint8_t slash_sz = 1;

	if(name[0] == '/')
	{
		strcpy(buff.filename, name);
	}
	else
	{
		uint16_t chr;
		char *path = malloc(PATH_MAX + terminator_sz);

		if(getcwd(path, PATH_MAX + terminator_sz) == NULL)
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
			buff.filename[chr + strlen(path) + slash_sz] = name[chr];
		}
		buff.filename[strlen(path) + strlen(name) + slash_sz] = TERMINATOR;

		free(path);
	}
}

struct Data punchedCard(struct Data buff, term_t limit, bool mode, char key)
{
	static term_t x = 0;
	if(key == LINEFEED)
	{
		x = 0;
	}
	else if(key == BACKSPACE)
	{
		x--;
		if(x == 0)
		{
			x = limit + 1;
		}
	}
	else
	{
		if(x > limit)
		{
			switch(mode)
			{
				case R:
					fprintf(stderr, "%s%i%s%i%s%i%c\n",
					"A single line cannot have more than ", limit,
					" chars, exited.\nLine ", buff.lines, " has got ", x, '.');

					free(buff.filename);
					exit(1);
				break;

				case W:
					buff.text[buff.chars - 1] = LINEFEED;
					buff.lines++;
					x = 0;
				break;
			}
		}
		x++;

		printf("%i\n", x); // DEBUG
	}
	return buff;
}

struct Data readFile(struct Data buff, char *name)
{
	char chr;
	const uint8_t terminator_sz = 1;

	buff.chars = 0;
	buff.lines = 1;

	buff.filename = malloc(PATH_MAX + NAME_MAX + terminator_sz);
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
		punchedCard(buff, 80, R, chr);
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
	if(key == CTRL_D || key == LINEFEED || key == CTRL_X || key >= 32)
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

			case CTRL_D:
				saveFile(buff);
			break;

			case CTRL_X:
				free(buff.filename);
				exit(0);
		}
	}
	return buff;
}

// Drawing funcions.
// Pressed keys to rendered chars in proper order.
void renderText(struct Data buff)
{
	buff_t x;

	if(buff.text[0] == LINEFEED)
	{
		printf("%c", LINEFEED); // Necessary at least for LXTerminal.
	}
	for(x = 0; x < buff.chars; x++) // Chars rendering.
	{
		if(x == buff.chars - 1) // Invert a last char as a cursor.
		{
			printf("%s%c%s", INVERT, buff.text[x], RESET);
		}
		else
		{
			printf("%c", buff.text[x]);
		}
	}
}

// Terminal fill that shows chars and other stupid things.
struct Data window(struct Data buff, char key)
{
	term_t y;

	buff = allocText(buff, key);
	buff = punchedCard(buff, 80, W, key);
	linesLimit(buff.lines);

	// Renderable.
	bar(buff, key);
	renderText(buff);

	for(y = buff.lines + BARS_SZ; y < getSize(Y); y++)
	{
		printf("%c", LINEFEED);
	}
	return buff;
}

