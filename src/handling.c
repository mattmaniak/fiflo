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

void set_filename(buff data, char* passed)
{
	const bool slash_sz = 1;

	if(passed[strlen(passed) - 1] == '/')
	{
		fputs("Cannot open the folder as a file, exited.\n", stderr);
		exit(1);
	}
	if(passed[0] == '/') // Is absolute path.
	{
		strcpy(data.filename, passed); // Malloced so doesn't need 'n' for size.
	}
	else
	{
		// https://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
		char* abs_path = malloc(PATH_MAX + TERMINATOR_SZ); // See man 3 getcwd.

		// TODO: MAYBE STRLEN
		ptr_check(getcwd(abs_path, PATH_MAX + TERMINATOR_SZ),
		"Cannot get your current dir.\n\0");

		strcpy(data.filename, abs_path); // Copy the path.
		data.filename[strlen(abs_path)] = '/'; // Add the slash between.

		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&data.filename[strlen(abs_path) + slash_sz + pos], 
			&passed[pos]);
		}
		free(abs_path);
	}
}

buff read_file(buff data)
{
	char chr;

	FILE* textfile = fopen(data.filename, "r");
	if(textfile)
	{
		data.text = malloc(get_file_sz(textfile) + TERMINATOR_SZ);
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
		data.text = malloc(TERMINATOR_SZ);
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
	const bool new_sz = 1;

	switch(key)
	{
		default:
			data.text = realloc(data.text, data.chars + TERMINATOR_SZ + new_sz);
			data.text[data.chars] = key;
			data.chars++;
		break;

		case TERMINATOR: // Required for rendering.
		case TAB:
		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_RIGHT:
		case ARROW_LEFT:
		break;

		case LINEFEED:
			data.text = realloc(data.text, data.chars + TERMINATOR_SZ + new_sz);
			data.text[data.chars] = LINEFEED;
			data.chars++;
		break;

		case BACKSPACE:
			data.text = realloc(data.text, data.chars);
			data.chars--;
			if(data.chars < 0)
			{
				data.chars = 0;
				data.text = realloc(data.text, data.chars + TERMINATOR_SZ);
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

