#include "fiflo.h"
#include "file.h"

f_mtdt* set_fname(f_mtdt* Buff, const char* passed)
{
	const _Bool slash_sz = 1;

	if(passed[strlen(passed) - NUL_SZ] == '/')
	{
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		free_all_exit(Buff, 1);
	}

	// Is the absolute path.
	if(passed[0] == '/')
	{
		if((strlen(passed) + NUL_SZ) > PATH_MAX)
		{
			fputs("The passed filename is too long, exit(1).\n", stderr);
			free_all_exit(Buff, 1);
		}
		strncpy(Buff->fname, passed, PATH_MAX);
	}
	// Relative path or the basename.
	else
	{
		char* cw_dir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		chk_ptr(Buff, "malloc for the current path\0", cw_dir);

		chk_ptr(Buff, "get current path. Too long\0",
		getcwd(cw_dir, PATH_MAX - NAME_MAX - slash_sz));

		// Exceeded 4096 chars.
		if((strlen(cw_dir) + strlen(passed)) >= PATH_MAX)
		{
			fputs("Passed filename is too long, exit(1).\n", stderr);
			free_all_exit(Buff, 1);
		}
		// Copy the path.
		strncpy(Buff->fname, cw_dir, PATH_MAX - NAME_MAX - slash_sz);

		// Add the slash between.
		Buff->fname[strlen(cw_dir)] = '/';

		// Append the basename.
		for(uint16_t pos = 0; pos < strlen(passed); pos++)
		{
			strcpy(&Buff->fname[strlen(cw_dir) + slash_sz + pos], &passed[pos]);
		}
		free(cw_dir);
		cw_dir = NULL;
	}
	return Buff;
}

f_mtdt* read_file(f_mtdt* Buff)
{
	char ch;
	FILE* textfile = fopen(Buff->fname, "r");

	if(textfile)
	{
		while((ch = (char) getc(textfile)) != EOF)
		{
			// Temponary and ugly tab to two spaces conversion.
			if(ch == '\t')
			{
				ch = ' ';
				Buff = text_char(Buff, ch);
			}

			// Read all chars before end of file.
			Buff = text_char(Buff, ch);
		}
		fclose(textfile);
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
	const int8_t not_created = -1;
	int          file_status = access(Buff->fname, F_OK);

	if(file_status == not_created)
	{
		// There is no file so create with -rw------- file mode.
		int create = open(Buff->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == not_created)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			free_all_exit(Buff, 1);
		}
	}
	FILE* textfile = fopen(Buff->fname, "w");

	if(textfile)
	{
		// Prevents blinking a little.
		window(Buff);

		// Write each line to the file. NULL terminator is ignored.
		for(buff_t line = 0; line <= Buff->lines; line++)
		{
			/* Using fputs or fprintf causes "use-of-uninitialized-value" using
			MSan because of there is more memory allocated than is needed. */
			for(buff_t ch = 0; ch < Buff->line_len[line]; ch++)
			{
				fputc(Buff->text[line][ch], textfile);
			}
		}
		fclose(textfile);

		SET_STATUS("saved\0");

		flush_window(Buff);
	}
	else
	{
		SET_STATUS("can't write to the file");
	}
}

