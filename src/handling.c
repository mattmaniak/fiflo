#include "handling.h"

void ptr_check(void* ptr, const char* errmsg)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exited.\n", errmsg);
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
	if(passed[strlen(passed) - NTERM_SZ] == '/')
	{
		fputs("Can't open the directory as the file, exited.\n", stderr);
		exit(1);
	}

	if(passed[0] == '/') // Is absolute path.
	{
		if(strlen(passed) + NTERM_SZ > PATH_MAX)
		{
			fputs("Given path is too long, exited.\n", stderr);
			exit(1);
		}
		strcpy(data.fname, passed); // Malloced so doesn't need 'n' for size.
	}
	else // Relative or basename.
	{
		const bool slash_sz = 1;

		char* abs_path = malloc(PATH_MAX); // Man 3 getcwd.
		ptr_check((getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0");

		if((strlen(abs_path) + strlen(passed) + NTERM_SZ) > PATH_MAX)
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

buff read_file(buff data)
{
	char chr;

	FILE* textfile = fopen(data.fname, "r");
	if(textfile)
	{
		while((chr = getc(textfile)) != EOF)
		{
			data = txt_alloc(data, chr);
		}
		fclose(textfile);
	}
	else
	{
		data.txt[data.lns][data.chrs] = NULLTERM;
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
			fputs("Failed to create the new file, exited.\n", stderr);
			exit(1);
		}		
	}
	FILE* textfile = fopen(data.fname, "w");
	ptr_check(textfile, "write to the file\0");

	for(buff_t ln = 0; ln <= data.lns; ln++)
	{
		fputs(data.txt[ln], textfile);
	}
	fclose(textfile);
}

buff add_char(buff data, char key)
{
	data.chrs_ln++;
	data.txt[data.lns] = realloc(data.txt[data.lns], data.chrs_ln + 1);
	ptr_check(data.txt[data.lns], "realloc memory for the new char\0");
	switch(key)
	{
		default:
			data.txt[data.lns][data.chrs_ln - 1] = key;
		break;

		case LINEFEED:
			data.txt[data.lns][data.chrs_ln - 1] = LINEFEED;
			data.txt[data.lns][data.chrs_ln] = NULLTERM;
			data.lns++;

			data.txt = realloc(data.txt, (data.lns + 1) * sizeof(&data.txt));
			ptr_check(data.txt, "realloc memory for the new line\0");
			data.txt[data.lns] = malloc(sizeof(char)); // The new line.
			ptr_check(data.txt[data.lns], "malloc byte in the new line\0");

			data.chrs_ln = 0;
		break;
	}
	data.txt[data.lns][data.chrs_ln] = NULLTERM;
	data.chrs++;
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

		case CTRL_X: // Frees everything and exits the program.
			for(buff_t ln = 0; ln <= data.lns; ln++)
			{
				free(data.txt[ln]);
			}
			free(data.txt);
			free(data.fname);
			exit(0);
		break;
	}
	return data;
}
/*
void limits(buff data)
{
	if(data.lns >= MAX_LN)
	{
		fprintf(stderr, "Maximum lns amount: %d, got: %d.\n", MAX_LN,
		data.lns);
		exit(1);
	}
	else if(data.chrs >= MAX_CHR)
	{
		fprintf(stderr, "Maximum lns amount: %d, got: %d.\n", MAX_CHR,
		data.chrs);
		exit(1);
	}
}
*/
buff txt_alloc(buff data, char key)
{
	switch(key)
	{
		case NULLTERM: // Only for the initialization.
		break;

		case NEGATIVE_CHAR: // Eg. CTRL+C - singal.
		case CTRL_D:
		case CTRL_X:
			data = keyboard_shortcut(data, key);
		break;

		case BACKSPACE:
			data.txt[data.lns] = realloc(data.txt[data.lns], data.chrs_ln + 1);
			ptr_check(data.txt[data.lns], "realloc memory for the backspace\0");

			data.chrs_ln--;
			if(data.lns > 0 && data.chrs_ln < 0)
			{
				free(data.txt[data.lns]);
				data.lns--;
				data.chrs_ln = strlen(data.txt[data.lns]) - 1;
			}
			else if(data.lns == 0 && data.chrs_ln < 0)
			{
				data.chrs_ln = 0;
			}
			data.txt[data.lns][data.chrs_ln] = NULLTERM;

			data.chrs--;
			if(data.chrs < 0)
			{
				data.chrs = 0;
			}
		break;

		default:
			data = add_char(data, key);
		break;
	}
//	limits(data);

	return data;
}

