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
	struct termios old, new;
	char key;

	tcgetattr(STDIN_FILENO, &old); // Get terminal attribiutes.
	new = old;
	new.c_lflag &= ~(ICANON | ECHO); // Disable buffered I/O and echo mode.
	
	tcsetattr(STDIN_FILENO, TCSANOW, &new); // Use new terminal I/O settings.
	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &old); // Restore terminal settings.

	return key;
}

void set_fname(buff dt, const char* passed)
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
		strcpy(dt.fname, passed); // Malloced so doesn't need 'n' for size.
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
		strcpy(dt.fname, abs_path); // Copy the path.
		dt.fname[strlen(abs_path)] = '/'; // Add the slash between.

		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&dt.fname[strlen(abs_path) + slash_sz + pos],
			&passed[pos]);
		}
		free(abs_path);
	}
}

buff read_file(buff dt)
{
	char chr;

	FILE* textfile = fopen(dt.fname, "r");
	if(textfile)
	{
		while((chr = getc(textfile)) != EOF)
		{
			dt = handle_key(dt, chr);
		}
		fclose(textfile);
	}
	else
	{
		dt.txt[dt.lns][dt.chrs] = NTERM;
	}
	return dt;
}

void save_file(buff dt)
{
	if(access(dt.fname, F_OK) == -1) // ... there is no file.
	{
		int created = open(dt.fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(created == -1)
		{
			fputs("Failed to create the new file, exited.\n", stderr);
			exit(1);
		}		
	}
	FILE* textfile = fopen(dt.fname, "w");
	ptr_check(textfile, "write to the file\0");

	for(buff_t ln = 0; ln <= dt.lns; ln++)
	{
		fputs(dt.txt[ln], textfile);
	}
	fclose(textfile);
}

void alloc_block(buff dt)
{
	if(dt.chrs_ln % MEMBLOCK == MEMBLOCK - NTERM_SZ)
	{
		dt.txt[dt.lns] = realloc(dt.txt[dt.lns], dt.chrs_ln + MEMBLOCK);
		ptr_check(dt.txt[dt.lns], "realloc the new memory block for chars\0");
	}
	else if(dt.lns % MEMBLOCK == MEMBLOCK)
	{
		dt.txt = realloc(dt.txt, dt.lns + MEMBLOCK);
		ptr_check(dt.txt[dt.lns], "realloc the new memory block for lines\0");
	}
}

void free_block(buff dt)
{
	if(dt.chrs_ln % MEMBLOCK == MEMBLOCK - NTERM_SZ)
	{
		dt.txt[dt.lns] = realloc(dt.txt[dt.lns], dt.chrs_ln - MEMBLOCK);
		ptr_check(dt.txt[dt.lns], "realloc the new memory block for chars\0");
	}
	else if(dt.lns % MEMBLOCK == MEMBLOCK)
	{
		dt.txt = realloc(dt.txt, dt.lns - MEMBLOCK);
		ptr_check(dt.txt[dt.lns], "realloc the new memory block for lines\0");
	}
}

void free_all(buff dt)
{
	free(dt.fname);
	for(buff_t ln = 0; ln <= dt.lns; ln++)
	{
		free(dt.txt[ln]);
	}
	free(dt.txt);
}

buff add_char(buff dt, char key)
{
	dt.chrs_ln++;
	alloc_block(dt);

	switch(key)
	{
		default:
			dt.txt[dt.lns][dt.chrs_ln - NTERM_SZ] = key;
		break;

		case TAB: // Actuall converts TAB to two spaces.
			dt.txt[dt.lns][dt.chrs_ln - NTERM_SZ] = SPACE;
			dt.chrs_ln++;

			alloc_block(dt);

			dt.txt[dt.lns][dt.chrs_ln - NTERM_SZ] = SPACE;
			dt.chrs++;
		break;			

		case LF:
			dt.txt[dt.lns][dt.chrs_ln - NTERM_SZ] = LF;
			dt.txt[dt.lns][dt.chrs_ln] = NTERM;
			dt.lns++;

			alloc_block(dt);

			dt.chrs_ln = 0;
			dt.txt[dt.lns] = malloc(MEMBLOCK); // The new line.
			ptr_check(dt.txt[dt.lns], "malloc byte in the new line\0");
		break;
	}
	dt.txt[dt.lns][dt.chrs_ln] = NTERM;
	dt.chrs++;
	return dt;
}

buff keyboard_shortcut(buff dt, char key)
{
	switch(key)
	{
		case NEG_CHAR:
		break;

		case CTRL_D:
			save_file(dt);
		break;

		case CTRL_X: // Frees everything and exits the program.
			free_all(dt);
			exit(0);
		break;
	}
	return dt;
}

buff alloc_chr(buff dt, char key)
{
	switch(key)
	{
		case NTERM: // Only for the initialization.
		break;

		case NEG_CHAR: // Eg. CTRL+C - singal.
		case CTRL_D:
		case CTRL_X:
			dt = keyboard_shortcut(dt, key);
		break;

		case BACKSPACE:
			free_block(dt);

			dt.chrs_ln--;
			if(dt.lns > 0 && dt.chrs_ln < 0)
			{
				free(dt.txt[dt.lns]);
				dt.lns--;
				dt.chrs_ln = strlen(dt.txt[dt.lns]) - NTERM_SZ;
			}
			else if(dt.lns == 0 && dt.chrs_ln < 0)
			{
				dt.chrs_ln = 0;
			}
			dt.txt[dt.lns][dt.chrs_ln] = NTERM;

			dt.chrs--;
			if(dt.chrs < 0)
			{
				dt.chrs = 0;
			}
		break;

		default:
			dt = add_char(dt, key);
		break;
	}
	return dt;
}

void limits(buff dt)
{
	if(dt.lns > MAX_LNS)
	{
		fprintf(stderr, "Max. lines amount: %d, got more.\n", MAX_LNS);
		free_all(dt);
		exit(1);
	}
	else if(dt.chrs > MAX_CHRS)
	{
		fprintf(stderr, "Max. chars amount: %d, got more.\n", MAX_CHRS);
		free_all(dt);
		exit(1);
	}
}

buff handle_key(buff dt, char key)
{
	if(key != 033)
	{
		dt = alloc_chr(dt, key);
		limits(dt);
	}
	return dt;
}

