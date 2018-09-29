#include "files.h"

void fnameset(buf* dt, const char* passed)
{
	if(passed[strlen(passed) - NTERM_SZ] == '/')
	{
		fputs("Can't open the directory as a file, exited.\n", stderr);
		freeallexit(dt, 1);
	}
	if(passed[0] == '/') // Is absolute path.
	{
		if(strlen(passed) + NTERM_SZ > PATH_MAX)
		{
			fputs("Given path is too long, exited.\n", stderr);
			freeallexit(dt, 1);
		}
		strcpy(dt->fname, passed); // Malloc'ed so doesn't need 'n' for size.
	}
	else // Relative or basename.
	{
		char* abs_path = malloc(PATH_MAX);
		checkptr(dt, (getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0");

		if((strlen(abs_path) + strlen(passed) + NTERM_SZ) > PATH_MAX)
		{
			fputs("Given filename is too long, exited.\n", stderr);
			freeallexit(dt, 1);
		}
		strcpy(dt->fname, abs_path); // Copy the path.
		dt->fname[strlen(abs_path)] = '/'; // Add the slash between.
		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&dt->fname[strlen(abs_path) + SLASH_SZ + pos], &passed[pos]);
		}
		free(abs_path);
	}
}

buf* readfile(buf* dt)
{
	char chr;
	dt->txtf = fopen(dt->fname, "rt");

	if(dt->txtf)
	{
		while((chr = (char) getc(dt->txtf)) != EOF)
		{
			dt = charadd(dt, chr);
		}
		fclose(dt->txtf);
	}
	return dt;
}

void savefile(buf* dt)
{
	if(access(dt->fname, F_OK) == -1) // There is no file.
	{
		int create = open(dt->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exited.\n", stderr);
			freeallexit(dt, 1);
		}
	}
	dt->txtf = fopen(dt->fname, "wt");
	checkptr(dt, dt->txtf, "write to the file\0");

	for(buf_t ln = 0; ln <= dt->lns; ln++)
	{
		fputs(dt->txt[ln], dt->txtf);
	}
	fclose(dt->txtf);
}

