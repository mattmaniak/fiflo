#include "file.h"

void set_fname(meta* Dat, const char* passed)
{
	if(passed[strlen(passed) - NTERM_SZ] == '/')
	{
		// Is folder.
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		free_all_exit(Dat, 1);
	}
	if(passed[0] == '/')
	{
		// Is absolute path.
		if(strlen(passed) + NTERM_SZ > PATH_MAX)
		{
			fputs("Given path is too long, exit(1).\n", stderr);
			free_all_exit(Dat, 1);
		}
		// Malloc'ed so doesn't need 'n' for size.
		strcpy(Dat->fname, passed);
	}
	else
	{
		// Is relative path or basename.
		char* abs_path = malloc(PATH_MAX);

		check_ptr(Dat, (getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0");

		if((strlen(abs_path) + strlen(passed) + NTERM_SZ) > PATH_MAX)
		{
			// Exceeded 4096 chars.
			fputs("Given filename is too long, exit(1).\n", stderr);
			free_all_exit(Dat, 1);
		}

		// Copy the path.
		strcpy(Dat->fname, abs_path);
		// Add the slash between.
		Dat->fname[strlen(abs_path)] = '/';
		// Append a basename.
		for(uint16_t pos = 0; pos < strlen(passed); pos++)
		{
			strcpy(&Dat->fname[strlen(abs_path) + SLASH_SZ + pos], &passed[pos]);
		}
		free(abs_path);
	}
}

meta* read_file(meta* Dat)
{
	char chr;
	Dat->txtf = fopen(Dat->fname, "rt");

	if(Dat->txtf)
	{
		while((chr = (char) getc(Dat->txtf)) != EOF)
		{
			// Read all chars before end of file.
			Dat = add_char(Dat, chr);
		}
		fclose(Dat->txtf);
	}
	return Dat;
}

void save_file(meta* Dat)
{
	if(access(Dat->fname, F_OK) == -1)
	{
		// There is no file so create with -rw-rw-r-- file mode.
		int create = open(Dat->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			free_all_exit(Dat, 1);
		}
	}
	Dat->txtf = fopen(Dat->fname, "wt");
	check_ptr(Dat, Dat->txtf, "write to the file\0");

	for(buf_t ln = 0; ln <= Dat->lns; ln++)
	{
		// Write each line to the file. NULL terminator is omited.
		fputs(Dat->txt[ln], Dat->txtf);
	}
	fclose(Dat->txtf);
}

