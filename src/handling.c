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

void set_fname(buff dat, char* passed)
{
	if(passed[strlen(passed) - NULLTERM_SZ] == '/')
	{
		fputs("Cannot open passed directory as the file, exited.\n", stderr);
		exit(1);
	}

	if(passed[0] == '/') // Is absolute path.
	{
		if(strlen(passed) + NULLTERM_SZ > PATH_MAX)
		{
			fputs("Given path is to long, exited.\n", stderr);
			exit(1);
		}
		strcpy(dat.fname, passed); // Malloced so doesn't need 'n' for size.
	}
	else // Relative or basename.
	{
		const bool slash_sz = 1;

		char* abs_path = malloc(PATH_MAX); // Man 3 getcwd.
		ptr_check((getcwd(abs_path, PATH_MAX)),
		"Cannot get your current path. Can be too long, exited.\0");

		if((strlen(abs_path) + strlen(passed) + NULLTERM_SZ) > PATH_MAX)
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

buff_t get_file_sz(FILE* fd)
{
	buff_t pos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	buff_t sz = ftell(fd);
	fseek(fd, pos, SEEK_SET);

	return sz;
}
/*
buff read_file(buff dat)
{
	char chr;

	FILE* textfile = fopen(dat.fname, "r");
	if(textfile)
	{
		dat.txt = malloc(get_file_sz(textfile) + NULLTERM_SZ);
		while((chr = getc(textfile)) != EOF)
		{
			dat.txt[dat.ln] = chr;
			dat.chrs++;
		}
		fclose(textfile);
	}
	else
	{
		dat.txt = malloc(NULLTERM_SZ);
		dat.txt[dat.chrs] = NULLTERM;
	}
	return dat;
}
*//*
void save_file(buff dat)
{
	if(access(dat.fname, F_OK) == -1) // ... there is no file.
	{
		int created = open(dat.fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(created == -1)
		{
			fputs("Failed to create the file.\n", stderr);
			exit(1);
		}		
	}
	FILE* textfile = fopen(dat.fname, "w");
	ptr_check(textfile, "Cannot write to the file, exited.\0");
	fputs(dat.txt, textfile);
	fclose(textfile);
}
*/
buff addition_char(buff dat, char key) // TODO: WORK AND SHORTEN.
{
	dat.chrs++;
	dat.chrs_ln++;

	dat.txt[dat.ln] = realloc(dat.txt[dat.ln], dat.chrs_ln + 1);
	ptr_check(dat.txt[dat.ln],

	"Cannot realloc memory for the new char, exited.\0");
	switch(key) // + 1 means 'the new char'.
	{
		default:
			dat.txt[dat.ln][dat.chrs_ln - 1] = key;
			dat.txt[dat.ln][dat.chrs_ln] = NULLTERM;
		break;

		case LINEFEED:
			dat.txt[dat.ln][dat.chrs_ln - 1] = LINEFEED;
			dat.txt[dat.ln][dat.chrs_ln] = NULLTERM;

			dat.ln++;
			dat.chrs_ln = 0;
			dat.txt = realloc(dat.txt, (dat.ln + 1) * 8);
			dat.txt[dat.ln] = malloc(1);
			dat.txt[dat.ln][0] = NULLTERM;
		break;
	}
	return dat;
}
/*
buff keyboard_shortcut(buff dat, char key)
{
	switch(key)
	{
		case NEGATIVE_CHAR:
		break;

		case CTRL_D:
			save_file(dat);
		break;

		case CTRL_X:
			free(dat.txt);
			free(dat.fname);
			exit(0);
		break;

	}
	return dat;
}

void limits(buff dat)
{
	if(dat.ln >= MAX_LINES)
	{
		fprintf(stderr, "Maximum lns amount: %d, got: %d.\n", MAX_LINES,
		dat.ln);
		exit(1);
	}
	else if(dat.chrs >= MAX_CHARS)
	{
		fprintf(stderr, "Maximum lns amount: %d, got: %d.\n", MAX_CHARS,
		dat.chrs);
		exit(1);
	}
}
*/
buff alloc_text(buff dat, char key)
{
	switch(key)
	{
		case NULLTERM: // Required for rendering.
		break;

		case NEGATIVE_CHAR: // Eg. CTRL+C.
		case CTRL_D:
		case CTRL_X:
//			dat = keyboard_shortcut(dat, key);
		break;

		case BACKSPACE:
			dat.txt[dat.ln] = realloc(dat.txt[dat.ln], dat.chrs_ln + 1);
			ptr_check(dat.txt[dat.ln],
			"Cannot realloc memory for the backspace, exited.\0");

			dat.chrs_ln--;
			dat.chrs--;
			if(dat.chrs < 0)
			{
				dat.chrs = 0;
			}
			if(dat.chrs_ln < 0)
			{
				dat.ln--;
				if(dat.ln < 0)
				{
					dat.ln = 0;
				}
				else if(dat.chrs > 0)
				{
					dat.chrs_ln = strlen(dat.txt[dat.ln] + 1);
				}
			}
			dat.txt[dat.ln][dat.chrs_ln] = NULLTERM;
		break;

		default:
			dat = addition_char(dat, key);
		break;
	}
//	limits(dat);

	return dat;
}

