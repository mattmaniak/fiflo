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

void set_filename(buff data, char *name)
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	const bool terminator_sz = 1;
	const bool slash_sz = 1;

	if(name[0] == '/')
	{
		strcpy(data.filename, name);
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
			data.filename[chr] = path[chr];
		}
		data.filename[strlen(path)] = '/'; // Add the slash between.

		for(chr = 0; chr < strlen(name); chr++) // Copy bname.
		{
			data.filename[chr + strlen(path) + slash_sz] = name[chr];
		}
		data.filename[strlen(path) + strlen(name) + slash_sz] = TERMINATOR;

		free(path);
	}
}

// Limit chars per line to 80.
buff punched_card(buff data, term_t limit, bool mode, char key)
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
					" chars, exited.\nLine ", data.lines, " has got ", pos,
					'.');

					free(data.filename);
					exit(1);
				break;

				// Auto newline. TODO!!!
				case WRITE:
					data.text[data.chars - 1] = LINEFEED;
					data.lines++;
					pos = 1;
				break;
			}
		}
	}
	return data;
}

buff read_file(buff data, char *name)
{
	char chr;
	const bool terminator_sz = 1;

	data.chars = 0;
	data.lines = 1;

	data.filename = malloc(PATH_MAX + NAME_MAX + terminator_sz);

	set_filename(data, name);
	FILE *textfile = fopen(data.filename, "r");

	if(textfile != NULL)
	{
		data.text = malloc(get_file_sz(textfile));
		while((chr = getc(textfile)) != EOF)
		{
			data.text[data.chars] = chr;
			data.chars++;
			if(chr == LINEFEED)
			{
				data.lines++;
			}
//			punched_card(data, MAX_CHARS_PER_LINE, READ, chr);
		}
		fclose(textfile);
	}
	else
	{
		data.text = malloc(1);
		data.text[0] = TERMINATOR;
	}
	return data;
}

void save_file(buff data)
{
	buff_t pos;

	FILE *textfile = fopen(data.filename, "w");
	ptr_check(textfile, "Cannot write to the file, exited.\0");

	for(pos = 0; pos < data.chars; pos++)
	{
		if(data.text[pos] != TERMINATOR)
		{
			fprintf(textfile, "%c", data.text[pos]);
		}
	}
	fclose(textfile);
}

// Convert pressed key into a char in the string.
buff alloc_text(buff data, char key)
{
	if(key == CTRL_D || key == LINEFEED || key == CTRL_X || key >= 32)
	{
		switch(key)
		{
			default:
				data.text = realloc(data.text, data.chars + 1); // TODO: CHEKCING
				data.text[data.chars] = key;
				if(key != TERMINATOR)
				{
					data.chars++;
				}
			break;

			case LINEFEED:
				data.text = realloc(data.text, data.chars + 1); // TODO: CHEKCING
				data.text[data.chars] = LINEFEED;
				data.chars++;
				data.lines++;
			break;

			case BACKSPACE:
				data.text = realloc(data.text, data.chars + 1); // TODO: CHEKCING
				data.text[data.chars] = BACKSPACE;
				data.text[data.chars] = LINEFEED;
				data.chars--;
				if(data.chars < 0)
				{
					data.chars = 0;
				}
				if(data.text[data.chars] == LINEFEED)
				{
					data.lines--;
				}
				if(data.lines < 1)
				{
					data.lines = 1;
				}
			break;

			case CTRL_D:
				save_file(data);
			break;

			case CTRL_X:
				free(data.filename);
				free(data.text);
				exit(0);
		}
		// TODO: UNIFIED TERMINATOR.
	}
	return data;
}

