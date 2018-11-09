#include "fiflo.h"
#include "file.h"

f_mtdt* set_fname(const char* arg, f_mtdt* Buff)
{
	const _Bool slash_sz = 1;

	if(arg[strlen(arg) - NUL_SZ] == '/')
	{
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		free_all_exit(1, Buff);
	}

	// Is a absolute path.
	if(arg[0] == '/')
	{
		if((strlen(arg) + NUL_SZ) > PATH_MAX)
		{
			fputs("The passed filename is too long, exit(1).\n", stderr);
			free_all_exit(1, Buff);
		}
		strncpy(Buff->fname, arg, PATH_MAX);
	}
	// Relative path or a basename.
	else
	{
		char* cw_dir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		chk_ptr(cw_dir, "alloc memory for the current path\0", Buff);

		chk_ptr((getcwd(cw_dir, PATH_MAX - NAME_MAX - slash_sz)),
		"get current path. Too long\0", Buff);

		// Exceeded 4096 chars.
		if((strlen(cw_dir) + strlen(arg)) >= PATH_MAX)
		{
			fputs("Passed filename is too long, exit(1).\n", stderr);
			free_all_exit(1, Buff);
		}
		// Copy a path.
		strncpy(Buff->fname, cw_dir, PATH_MAX - NAME_MAX - slash_sz);

		// Add the slash between.
		Buff->fname[strlen(cw_dir)] = '/';

		// Append a basename.
		for(uint16_t pos = 0; pos < strlen(arg); pos++)
		{
			strcpy(&Buff->fname[strlen(cw_dir) + slash_sz + pos], &arg[pos]);
		}
		free(cw_dir);
		cw_dir = NULL;
	}
	return Buff;
}

f_mtdt* read_file(f_mtdt* Buff)
{
	char ch;
	Buff->textf = fopen(Buff->fname, "rt");

	if(Buff->textf)
	{
		while((ch = (char) getc(Buff->textf)) != EOF)
		{
			// Temponary and ugly tab to two spaces conversion.
			if(ch == '\t')
			{
				ch = ' ';
				Buff = text_char(ch, Buff);
			}

			// Read all chars before end of file.
			Buff = text_char(ch, Buff);
		}
		fclose(Buff->textf);
		SET_STATUS("read the file\0");
	}
	else
	{
		SET_STATUS("the file will be created\0");
	}
	return Buff;
}

void save_file(f_mtdt* Buff)
{
	if(access(Buff->fname, F_OK) == -1)
	{
		// There is no file so create with -rw------- file mode.
		int create = open(Buff->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			free_all_exit(1, Buff);
		}
	}
	Buff->textf = fopen(Buff->fname, "wt");

	if(Buff->textf)
	{
		// Prevents blinking a little.
		window(Buff);

		for(buff_t line = 0; line <= Buff->lines; line++)
		{
			// Write each line to the file. NULL terminator is ignored.
			fputs(Buff->text[line], Buff->textf);
		}
		fclose(Buff->textf);

		SET_STATUS("saved\0");

		flush_window(Buff);
	}
	else
	{
		SET_STATUS("can't write to the file");
	}
}

