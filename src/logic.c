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
		buff.filename[strlen(path)] = '/'; // Add the slash between.

		for(chr = 0; chr < strlen(name); chr++) // Copy bname.
		{
			buff.filename[chr + strlen(path) + slash_sz] = name[chr];
		}
		buff.filename[strlen(path) + strlen(name) + slash_sz] = TERMINATOR;

		free(path);
	}
}

// Limit chars per line to 80.
struct Data punchedCard(struct Data buff, term_t limit, bool mode, char key)
{
	const uint8_t newline = 1;
	static int8_t x;

	if(key == LINEFEED)
	{
		x = 1;
	}
	else if(key == BACKSPACE)
	{
		x--;
		if(x <= 1)
		{
			x = limit + newline + 1;
		}
	}
	else
	{
		x++;
		if(x > limit + newline)
		{
			switch(mode)
			{
				case READ:
					fprintf(stderr, "%s%i%s%i%s%i%c\n",
					"A single line cannot have more than ", limit,
					" chars, exited.\nLine ", buff.lines, " has got ", x, '.');

					free(buff.filename);
					exit(1);
				break;

				// Auto newline. TODO!!!
				case WRITE:
					buff.text[buff.chars - 1] = LINEFEED;
					buff.lines++;
					x = 1;
				break;
			}
		}
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
	buff.text = malloc(100); // TODO: dynamic allocation.

	setFilename(buff, name);
	FILE *fd = fopen(buff.filename, "r");

	if(fd != NULL)
	{
		while((chr = getc(fd)) != EOF)
		{
			buff.text[buff.chars] = chr;
			buff.chars++;
			if(chr == LINEFEED)
			{
				buff.lines++;
			}
			punchedCard(buff, MAX_CHARS_PER_LINE, READ, chr);
		}
		fclose(fd);
	}
	else
	{
		buff.text[0] = TERMINATOR;
	}
	return buff;
}

void saveFile(struct Data buff)
{
	buff_t x;

	FILE *fd = fopen(buff.filename, "w");
	pointerCheck(fd, "Cannot write to the file, exited.\0");

	for(x = 0; x < buff.chars; x++)
	{
		if(buff.text[x] != TERMINATOR)
		{
			fprintf(fd, "%c", buff.text[x]);
		}
	}
	fclose(fd);
}

// Convert pressed key into a char in the string.
struct Data allocText(struct Data buff, char key)
{
	if(key == CTRL_D || key == LINEFEED || key == CTRL_X || key >= 32)
	{
		switch(key)
		{
			default:
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
				free(buff.text);
				exit(0);
		}
	}
	return buff;
}

// Pressed keys to rendered chars in proper order.
void renderText(struct Data buff)
{
	buff_t x;

	if(buff.text[0] == TERMINATOR || LINEFEED)
	{
		printf("%c", LINEFEED); // Necessary at least for the LXTerminal.
	}

	if(buff.lines <= getSize(Y) - BAR_SZ)
	{
		for(x = 0; x < buff.chars; x++) // Chars rendering.
		{
			printf("%c", buff.text[x]);
		}
	}
	else // More lines than the terminal can render - scrolling.
	{
		term_t first_line;
		term_t y = 0;

		for(x = 0; x < buff.chars; x++)
		{
			if(buff.text[x] == LINEFEED && y < first_line)
			{
				first_line = x;
				y++;
			}
		}
		for(x = first_line; x < buff.chars; x++) // Chars rendering.
		{
			printf("%c", buff.text[x]);
		}
	}
}

void windowFill(buff_t lines)
{
	term_t y;
	if(lines <= getSize(Y) + BAR_SZ)
	{
		for(y = lines + BAR_SZ; y < getSize(Y); y++)
		{
			printf("%c", LINEFEED);
		}
	}
//	printf("%s", "ssssssssss");
}

// Terminal fill that shows chars and other stupid things.
struct Data window(struct Data buff, char key)
{
	buff = allocText(buff, key);
	buff = punchedCard(buff, MAX_CHARS_PER_LINE, WRITE, key);
//	linesLimit(buff.lines);

	bar(buff, key);
	renderText(buff);

	return buff;
}

