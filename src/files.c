#include "files.h"

void set_fname(meta* dt, const char* passed)
{
	if(passed[strlen(passed) - NTERM_SZ] == '/')
	{
		// Is folder.
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		free_all_exit(dt, 1);
	}
	if(passed[0] == '/')
	{
		// Is absolute path.
		if(strlen(passed) + NTERM_SZ > PATH_MAX)
		{
			fputs("Given path is too long, exit(1).\n", stderr);
			free_all_exit(dt, 1);
		}
		strcpy(dt->fname, passed); // Malloc'ed so doesn't need 'n' for size.
	}
	else
	{
		// Is relative path or basename.
		char* abs_path = malloc(PATH_MAX);
		check_ptr(dt, (getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0");

		if((strlen(abs_path) + strlen(passed) + NTERM_SZ) > PATH_MAX)
		{
			// Exceeded 4096 chars.
			fputs("Given filename is too long, exit(1).\n", stderr);
			free_all_exit(dt, 1);
		}
		// TODO: COMMENTS && HOW DOES IT WORKS?
		strcpy(dt->fname, abs_path); // Copy the path.
		dt->fname[strlen(abs_path)] = '/'; // Add the slash between.
		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&dt->fname[strlen(abs_path) + SLASH_SZ + pos], &passed[pos]);
		}
		free(abs_path);
	}
}

meta* read_file(meta* dt)
{
	char chr;
	dt->txtf = fopen(dt->fname, "rt");

	if(dt->txtf)
	{
		while((chr = (char) getc(dt->txtf)) != EOF)
		{
			// Read all chars before end of file.
			dt = add_char(dt, chr);
		}
		fclose(dt->txtf);
	}
	return dt;
}

void save_file(meta* dt)
{
	if(access(dt->fname, F_OK) == -1)
	{
		// There is no file so...
		int create = open(dt->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			free_all_exit(dt, 1);
		}
	}
	dt->txtf = fopen(dt->fname, "wt");
	check_ptr(dt, dt->txtf, "write to the file\0");

	for(buf_t ln = 0; ln <= dt->lns; ln++)
	{
		// Write each line to the file. NULL terminator is omited.
		fputs(dt->txt[ln], dt->txtf);
	}
	fclose(dt->txtf);
}

