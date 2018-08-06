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

void ptr_check(void* ptr, const char* errmsg)
{
	if(!ptr)
	{
		fputs(errmsg, stderr);
		exit(1);
	}
}

void limits(buff data)
{
	if(data.lines >= MAX_LINES)
	{
		fprintf(stderr, "Maximum lines amount: %i, got: %i.\n", MAX_LINES,
		data.lines);
		exit(1);
	}
	else if(data.chars >= MAX_CHARS)
	{
		fprintf(stderr, "Maximum lines amount: %i, got: %i.\n", MAX_CHARS,
		data.chars);
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
		ptr_check(path, "Cannot allocate path in memory, exited.\n\0");

		if(getcwd(path, MAX_PATH + terminator_sz) == NULL)
		{
			fputs("Cannot get your current dir, exited.\n\0", stderr);
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
	ptr_check(data.filename, "Cannot alloc filename in memory, exited.\n\0");

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
	ptr_check(textfile, "Cannot write to the file, exited.\n\0");

	fputs(data.text, textfile);
	fclose(textfile);
}

buff visible_char(buff data, char key)
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
		case TAB: // TODO
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
	ptr_check(data.text, "Cannot realloc memory for the new char, exited.\n\0");
	data.text[data.chars] = TERMINATOR;
	return data;
}

buff keyboard_shortcut(buff data, char key)
{
	switch(key)
	{
		case CTRL_D:
			save_file(data);
		break;

		case CTRL_X:
			free(data.text);
			free(data.filename);
			exit(0);
	}
	return data;
}

buff count_lines(buff data)
{
	data.lines = 1; // Reset.

	for(buff_t pos = 0; pos <= data.chars; pos++)
	{
		if(data.text[pos] == LINEFEED)
		{
			data.lines++;
		}
	}
	return data;
}

buff alloc_text(buff data, char key)
{
	switch(key)
	{
		default:
			data = visible_char(data, key);
		break;

		case CTRL_D:
		case CTRL_X:
			data = keyboard_shortcut(data, key);
		break;
	}
	data = count_lines(data);
	return data;
}

