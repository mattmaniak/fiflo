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

void set_fname(buff dat, char* passed)
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
		strcpy(dat.fname, passed); // Malloced so doesn't need 'n' for size.
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
		strcpy(dat.fname, abs_path); // Copy the path.
		dat.fname[strlen(abs_path)] = '/'; // Add the slash between.

		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&dat.fname[strlen(abs_path) + slash_sz + pos],
			&passed[pos]);
		}
		free(abs_path);
	}
}

buff read_file(buff dat)
{
	char chr;

	FILE* textfile = fopen(dat.fname, "r");
	if(textfile)
	{
		while((chr = getc(textfile)) != EOF)
		{
			dat = txt_alloc(dat, chr);
		}
		fclose(textfile);
	}
	else
	{
		dat.txt[dat.lns][dat.chrs] = NULLTERM;
	}
	return dat;
}

void save_file(buff dat)
{
	if(access(dat.fname, F_OK) == -1) // ... there is no file.
	{
		int created = open(dat.fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(created == -1)
		{
			fputs("Failed to create the new file, exited.\n", stderr);
			exit(1);
		}		
	}
	FILE* textfile = fopen(dat.fname, "w");
	ptr_check(textfile, "write to the file\0");

	for(buff_t ln = 0; ln <= dat.lns; ln++)
	{
		fputs(dat.txt[ln], textfile);
	}
	fclose(textfile);
}

buff add_char(buff dat, char key)
{
	dat.chrs_ln++;
	dat.txt[dat.lns] = realloc(dat.txt[dat.lns], dat.chrs_ln + 1);
	ptr_check(dat.txt[dat.lns], "realloc memory for the new char\0");
	switch(key)
	{
		default:
			dat.txt[dat.lns][dat.chrs_ln - 1] = key;
		break;

		case LF:
			dat.txt[dat.lns][dat.chrs_ln - 1] = LF;
			dat.txt[dat.lns][dat.chrs_ln] = NULLTERM;
			dat.lns++;

			dat.txt = realloc(dat.txt, (dat.lns + 1) * sizeof(&dat.txt));
			ptr_check(dat.txt, "realloc memory for the new line\0");
			dat.txt[dat.lns] = malloc(sizeof(char)); // The new line.
			ptr_check(dat.txt[dat.lns], "malloc byte in the new line\0");

			dat.chrs_ln = 0;
		break;
	}
	dat.txt[dat.lns][dat.chrs_ln] = NULLTERM;
	dat.chrs++;
	return dat;
}

buff keyboard_shortcut(buff dat, char key)
{
	switch(key)
	{
		case NEG_CHAR:
		break;

		case CTRL_D:
			save_file(dat);
		break;

		case CTRL_X: // Frees everything and exits the program.
			for(buff_t ln = 0; ln <= dat.lns; ln++)
			{
				free(dat.txt[ln]);
			}
			free(dat.txt);
			free(dat.fname);
			exit(0);
		break;
	}
	return dat;
}

void limits(buff dat)
{
	if(dat.lns >= MAX_LNS)
	{
		fprintf(stderr, "Maximum lns amount: %d, got: %d.\n", MAX_LNS, dat.lns);
		exit(1);
	}
	else if(dat.chrs >= MAX_CHRS)
	{
		fprintf(stderr, "Maximum lns amount: %d, got: %d.\n", MAX_CHRS,
		dat.chrs);
		exit(1);
	}
}

buff txt_alloc(buff dat, char key)
{
	if(key != 033) // ...is not ANSI escape code then convert arrows to chars.
	{
		switch(key)
		{
			case NULLTERM: // Only for the initialization.
			break;

			case NEG_CHAR: // Eg. CTRL+C - singal.
			case CTRL_D:
			case CTRL_X:
				dat = keyboard_shortcut(dat, key);
			break;

			case BACKSPACE:
				dat.txt[dat.lns] = realloc(dat.txt[dat.lns], dat.chrs_ln + 1);
				ptr_check(dat.txt[dat.lns], "realloc memory for the backspace\0");

				dat.chrs_ln--;
				if(dat.lns > 0 && dat.chrs_ln < 0)
				{
					free(dat.txt[dat.lns]);
					dat.lns--;
					dat.chrs_ln = strlen(dat.txt[dat.lns]) - 1;
				}
				else if(dat.lns == 0 && dat.chrs_ln < 0)
				{
					dat.chrs_ln = 0;
				}
				dat.txt[dat.lns][dat.chrs_ln] = NULLTERM;

				dat.chrs--;
				if(dat.chrs < 0)
				{
					dat.chrs = 0;
				}
			break;

			default:
				dat = add_char(dat, key);
			break;
		}
	}
	limits(dat);

	return dat;
}

