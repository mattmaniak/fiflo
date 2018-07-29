// Shows how the program single round works.

#include "handling.h"

#include "hardware.c"

void pointerCheck(void *ptr, char *errmsg) // Eg. malloc or FILE*.
{
	if(ptr == NULL)
	{
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}
}

void charsLimit(buff_t chars)
{
	if(chars > MAX_CHARS)
	{
		fprintf(stderr, "%s%i%s%i%s",
		"Max. lines amount: ", MAX_CHARS, ", got: ", chars,
		". Stretch your terminal or sth.\n");
		exit(1);
	}
}

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
	static int8_t pos;

	if(key == LINEFEED)
	{
		pos = 1;
	}
	else if(key == BACKSPACE)
	{
		pos--;
		if(pos <= 1)
		{
			pos = limit + newline + 1;
		}
	}
	else
	{
		pos++;
		if(pos > limit + newline)
		{
			switch(mode)
			{
				case READ:
					fprintf(stderr, "%s%i%s%i%s%i%c\n",
					"A single line cannot have more than ", limit,
					" chars, exited.\nLine ", buff.lines, " has got ", pos,
					'.');

					free(buff.filename);
					exit(1);
				break;

				// Auto newline. TODO!!!
				case WRITE:
					buff.text[buff.chars - 1] = LINEFEED;
					buff.lines++;
					pos = 1;
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

	setFilename(buff, name);
	FILE *fd = fopen(buff.filename, "r");

	if(fd != NULL)
	{
		buff_t pos = ftell(fd); // Check size of the file.
		fseek(fd, 0, SEEK_END);
		buff_t fd_sz = ftell(fd);
		fseek(fd, pos, SEEK_SET);

		buff.text = malloc(fd_sz);

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
		buff.text = malloc(1);
		buff.text[0] = TERMINATOR;
	}
	return buff;
}

void saveFile(struct Data buff)
{
	buff_t pos;

	FILE *fd = fopen(buff.filename, "w");
	pointerCheck(fd, "Cannot write to the file, exited.\0");

	for(pos = 0; pos < buff.chars; pos++)
	{
		if(buff.text[pos] != TERMINATOR)
		{
			fprintf(fd, "%c", buff.text[pos]);
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
				buff.text = realloc(buff.text, buff.chars + 1); // TODO: CHEKCING
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
				buff.text = realloc(buff.text, buff.chars + 1); // TODO: CHEKCING
				buff.text[buff.chars] = LINEFEED;
				buff.chars++;
				if(buff.chars > MAX_CHARS)
				{
					buff.chars = MAX_CHARS;
				}
				buff.lines++;
			break;

			case BACKSPACE:
				buff.text = realloc(buff.text, buff.chars + 1); // TODO: CHEKCING
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

