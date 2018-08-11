#include "handling.h"

void ptr_check(void* ptr, const char* errmsg)
{
	if(!ptr)
	{
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}
}

char nix_getch(void)
{
	char key;
	struct termios old, new;

	tcgetattr(STDIN_FILENO, &old); // Get terminal attribiutes.
	new = old; // Set new terminal settings same as old.
	new.c_lflag &= ~(ICANON | ECHO); // Disable buffered I/O and echo mode.
	
	tcsetattr(STDIN_FILENO, TCSANOW, &new); // Use new terminal I/O settings.
	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &old); // Restore terminal settings.

	return key;
}

void set_fname(buff data, char* passed)
{
	if(passed[strlen(passed) - TERMINATOR_SZ] == '/')
	{
		fputs("Cannot open passed directory as the file, exited.\n", stderr);
		exit(1);
	}

	if(passed[0] == '/') // Is absolute path.
	{
		if(strlen(passed) + TERMINATOR_SZ > PATH_MAX)
		{
			fputs("Given path is to long, exited.\n", stderr);
			exit(1);
		}
		strcpy(data.fname, passed); // Malloced so doesn't need 'n' for size.
	}
	else // Relative or basename.
	{
		const bool slash_sz = 1;

		char* abs_path = malloc(PATH_MAX); // Man 3 getcwd.
		ptr_check((getcwd(abs_path, PATH_MAX)),
		"Cannot get your current path. Can be too long, exited.\0");

		if((strlen(abs_path) + strlen(passed) + TERMINATOR_SZ) > PATH_MAX)
		{
			fputs("Given filename is too long, exited.\n", stderr);
			exit(1);
		}
		strcpy(data.fname, abs_path); // Copy the path.
		data.fname[strlen(abs_path)] = '/'; // Add the slash between.

		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&data.fname[strlen(abs_path) + slash_sz + pos],
			&passed[pos]);
		}
		free(abs_path);
	}
}

buff_t get_file_sz(FILE* fd)
{
	buff_t pos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	buff_t sz = ftell(fd);
	fseek(fd, pos, SEEK_SET);

	return sz;
}

buff read_file(buff data)
{
	char chr;

	FILE* textfile = fopen(data.fname, "r");
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
	if(access(data.fname, F_OK) == -1) // ... there is no file.
	{
		int created = open(data.fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(created == -1)
		{
			fputs("Failed to create the file.\n", stderr);
			exit(1);
		}		
	}
	FILE* textfile = fopen(data.fname, "w");
	ptr_check(textfile, "Cannot write to the file, exited.\0");
	fputs(data.text, textfile);
	fclose(textfile);
}

buff visible_char(buff data, char key)
{
	switch(key) // + 1 means 'the new char'.
	{
		default:
			data.text = realloc(data.text, data.chars + TERMINATOR_SZ + 1);
			data.text[data.chars] = key;
			data.chars++;
		break;

		case TERMINATOR: // Required for rendering.
		break;

		case TAB:
			for(uint8_t spaces = 0; spaces < 2; spaces++) // Actually converts.
			{
				data.text = realloc(data.text, data.chars + TERMINATOR_SZ + 1);
				data.text[data.chars] = SPACE;
				data.chars++;
			}
		break;

		case LINEFEED:
			data.text = realloc(data.text, data.chars + TERMINATOR_SZ + 1);
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
	ptr_check(data.text, "Cannot realloc memory for the new char, exited.\0");
	data.text[data.chars] = TERMINATOR;

	return data;
}

buff keyboard_shortcut(buff data, char key)
{
	switch(key)
	{
		case NEGATIVE_CHAR:
		break;

		case CTRL_D:
			save_file(data);
		break;

		case CTRL_X:
			free(data.text);
			free(data.fname);
			exit(0);
		break;

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

void limits(buff data)
{
	if(data.lines >= MAX_LINES)
	{
		fprintf(stderr, "Maximum lines amount: %d, got: %d.\n", MAX_LINES,
		data.lines);
		exit(1);
	}
	else if(data.chars >= MAX_CHARS)
	{
		fprintf(stderr, "Maximum lines amount: %d, got: %d.\n", MAX_CHARS,
		data.chars);
		exit(1);
	}
}

buff alloc_text(buff data, char key)
{
	switch(key)
	{
		default:
			data = visible_char(data, key);
		break;

		case NEGATIVE_CHAR: // Eg. CTRL+C.
		case CTRL_D:
		case CTRL_X:
			data = keyboard_shortcut(data, key);
		break;
	}
	data = count_lines(data);
	limits(data);

	return data;
}

