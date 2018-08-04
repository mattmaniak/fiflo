// Functions connected with the program API. Generally what's unseen.

#include "handling.h"

buff_t get_file_sz(FILE* fd)
{
	buff_t pos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	buff_t sz = ftell(fd);
	fseek(fd, pos, SEEK_SET);

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

void ptr_check(void* ptr, const char* errmsg) // Eg. malloc or FILE*.
{
	if(ptr == NULL)
	{
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}
}

void chars_limit(buff_t chars)
{
	if(chars > MAX_CHARS - 1) // -1 to handle max_lines.
	{
		fprintf(stderr, "%s%i%s%i\n", "Max. lines amount: ", MAX_CHARS,
		", got: ", chars);
		exit(1);
	}
}

void set_filename(buff data, char* name) // TODO: FOLDER PREVENTION
{
	// Filename = absolute path + basename eg. "/home/user/my_file".
	// Basename (base filename) eg. "my_file".
	const bool terminator_sz = 1;
	const bool slash_sz = 1;

	if(name[0] == '/')
	{
		strcpy(data.filename, name); // PROBABLY TODO
	}
	else
	{
		// https://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
		uint16_t pos;
		char* path = malloc(MAX_PATH + terminator_sz);
		ptr_check(path, "Cannot allocate path in memory, exited.\0");

		if(getcwd(path, MAX_PATH + terminator_sz) == NULL)
		{
			fputs("Cannot get your current dir, exited.\0", stderr);
			exit(1);
		}
		path[strlen(path)] = TERMINATOR;

		for(pos = 0; pos < strlen(path); pos++) // Copy cwd.
		{
			data.filename[pos] = path[pos];
		}
		data.filename[strlen(path)] = '/'; // Add the slash between.

		for(pos = 0; pos < strlen(name); pos++) // Copy bname.
		{
			data.filename[pos + strlen(path) + slash_sz] = name[pos];
		}
		data.filename[strlen(path) + strlen(name) + slash_sz] = TERMINATOR;

		free(path);
	}
}

buff read_file(buff data, char* name)
{
	char chr;
	const bool terminator_sz = 1;

	data.filename = malloc(MAX_PATH + MAX_NAME + terminator_sz); // >~ 4 KiB.
	ptr_check(data.filename, "Cannot alloc filename in memory, exited.\0");

	set_filename(data, name);
	FILE* textfile = fopen(data.filename, "r");

	if(textfile != NULL)
	{
		data.text = malloc(get_file_sz(textfile) + terminator_sz);
		while((chr = getc(textfile)) != EOF)
		{
			data.text[data.chars] = chr;
			data.chars++;
		}
		data.text[data.chars] = TERMINATOR;
		fclose(textfile);
	}
	else
	{
		data.text = malloc(terminator_sz);
		data.text[data.chars] = TERMINATOR;
	}
	return data;
}

void save_file(buff data)
{
	FILE* textfile = fopen(data.filename, "w");
	ptr_check(textfile, "Cannot write to the file, exited.\0");

	fputs(data.text, textfile);
	fclose(textfile);
}

buff count_lines(buff data)
{
	buff_t pos;
	data.lines = 1; // Reset.

	for(pos = 0; pos <= data.chars; pos++)
	{
		if(data.text[pos] == LINEFEED)
		{
			data.lines++;
		}
	}
	return data;
}


buff visible_chars(buff data, char key)
{
	bool terminator_sz = 1;
	bool new_sz = 1;

	switch(key)
	{
		default:
			data.text = realloc(data.text, data.chars + terminator_sz + new_sz);
			data.text[data.chars] = key;
			data.chars++;
		break;

		case TERMINATOR: // Required for rendering.
		case TAB:
		break;

		case LINEFEED:
			data.text = realloc(data.text, data.chars + terminator_sz + new_sz);
			data.text[data.chars] = LINEFEED;
			data.chars++;
		break;

		case BACKSPACE:
			data.text = realloc(data.text, data.chars);
			data.chars--;
			if(data.chars < 0)
			{
				data.chars = 0;
				data.text = realloc(data.text, data.chars + terminator_sz);
			}
		break;
	}
	ptr_check(data.text, "Cannot realloc memory for a new char, exited.\0");
	data.text[data.chars] = TERMINATOR;
	return data;
}

buff shortcut(buff data, char key)
{
	switch(key)
	{
		case CTRL_D:
			save_file(data);
		break;

		case CTRL_X:
			free(data.filename);
			free(data.text);
			exit(0);
	}
	return data;
}

// Convert pressed key into a char in the string.
buff alloc_text(buff data, char key)
{
	switch(key)
	{
		default:
			data = visible_chars(data, key);
		break;

		case CTRL_D:
		case CTRL_X:
			data = shortcut(data, key);
		break;
	}
	return data;
}

