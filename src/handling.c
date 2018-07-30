// Functions connected with the program API. Generally what's unseen.

#include "handling.h"

buff_t get_file_sz(FILE *fd)
{
	buff_t pos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	buff_t sz = ftell(fd);
	fseek(fd, pos, SEEK_SET);

	if(sz > MAX_CHARS)
	{
		fputs("To big file on input, exited.\n", stderr);
		exit(1);
	}
	return sz;
}

char nix_getch(void) // https://stackoverflow.com/questions/12710582/
{
	char key;
	struct termios oldt, newt;

	tcgetattr(STDIN_FILENO, &oldt); // Get terminal attribiutes.
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set terminal attribiutes.

	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Set terminal attribiutes.
	return key;
}

void ptr_check(void *ptr, char *errmsg) // Eg. malloc or FILE*.
{
	if(ptr == NULL)
	{
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}
}

void chars_limit(buff_t chars)
{
	if(chars > MAX_CHARS)
	{
		fprintf(stderr, "%s%i%s%i%s",
		"Max. lines amount: ", MAX_CHARS, ", got: ", chars,
		". Stretch your terminal or sth.\n");
		exit(1);
	}
}

void set_filename(data buff, char *name)
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	const bool terminator_sz = 1;
	const bool slash_sz = 1;

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
data punched_card(data buff, term_t limit, bool mode, char key)
{
	const bool newline = 1;
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

data read_file(data buff, char *name)
{
	char chr;
	const bool terminator_sz = 1;

	buff.chars = 0;
	buff.lines = 1;

	buff.filename = malloc(PATH_MAX + NAME_MAX + terminator_sz);

	set_filename(buff, name);
	FILE *textfile = fopen(buff.filename, "r");

	if(textfile != NULL)
	{
		buff.text = malloc(get_file_sz(textfile));

		while((chr = getc(textfile)) != EOF)
		{
			buff.text[buff.chars] = chr;
			buff.chars++;
			if(chr == LINEFEED)
			{
				buff.lines++;
			}
//			punched_card(buff, MAX_CHARS_PER_LINE, READ, chr);
		}
		fclose(textfile);
	}
	else
	{
		buff.text = malloc(1);
		buff.text[0] = TERMINATOR;
	}
	return buff;
}

void save_file(data buff)
{
	buff_t pos;

	FILE *textfile = fopen(buff.filename, "w");
	ptr_check(textfile, "Cannot write to the file, exited.\0");

	for(pos = 0; pos < buff.chars; pos++)
	{
		if(buff.text[pos] != TERMINATOR)
		{
			fprintf(textfile, "%c", buff.text[pos]);
		}
	}
	fclose(textfile);
}

// Convert pressed key into a char in the string.
data alloc_text(data buff, char key)
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
				buff.text[buff.chars] = BACKSPACE;
				buff.text[buff.chars] = LINEFEED;
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
				save_file(buff);
			break;

			case CTRL_X:
				free(buff.filename);
				free(buff.text);
				exit(0);
		}
		// TODO: UNIFIED TERMINATOR.
	}
	return buff;
}

