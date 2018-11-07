#include "fiflo.h"
#include "logic.h"

meta* set_fname(const char* arg, meta* Dt)
{
	const _Bool slash_sz = 1;

	if(arg[strlen(arg) - NTERM_SZ] == '/')
	{
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		free_all_exit(1, Dt);
	}

	// Is a absolute path.
	if(arg[0] == '/')
	{
		if((strlen(arg) + NTERM_SZ) > PATH_MAX)
		{
			fputs("The passed filename is too long, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
		strncpy(Dt->fname, arg, PATH_MAX);
	}
	// Relative path or a basename.
	else
	{
		char* cw_dir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		chk_ptr(cw_dir, "alloc memory for the current path\0", Dt);

		chk_ptr((getcwd(cw_dir, PATH_MAX - NAME_MAX - slash_sz)),
		"get current path. Too long\0", Dt);

		// Exceeded 4096 chars.
		if((strlen(cw_dir) + strlen(arg)) >= PATH_MAX)
		{
			fputs("Passed filename is too long, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
		// Copy a path.
		strncpy(Dt->fname, cw_dir, PATH_MAX - NAME_MAX - slash_sz);

		// Add the slash between.
		Dt->fname[strlen(cw_dir)] = '/';

		// Append a basename.
		for(uint16_t pos = 0; pos < strlen(arg); pos++)
		{
			strcpy(&Dt->fname[strlen(cw_dir) + slash_sz + pos], &arg[pos]);
		}
		free(cw_dir);
		cw_dir = NULL;
	}
	return Dt;
}

meta* read_file(meta* Dt)
{
	char chr;
	Dt->textf = fopen(Dt->fname, "rt");

	if(Dt->textf)
	{
		while((chr = (char) getc(Dt->textf)) != EOF)
		{
			// Temponary and ugly tab to two spaces conversion.
			if(chr == '\t')
			{
				chr = ' ';
				Dt = text_char(chr, Dt);
			}

			// Read all chars before end of file.
			Dt = text_char(chr, Dt);
		}
		fclose(Dt->textf);
		SET_STATUS("read the file\0");
	}
	else
	{
		SET_STATUS("the file will be created\0");
	}

	return Dt;
}

void save_file(meta* Dt)
{
	if(access(Dt->fname, F_OK) == -1)
	{
		// There is no file so create with -rw------- file mode.
		int create = open(Dt->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
	}
	Dt->textf = fopen(Dt->fname, "wt");

	if(Dt->textf)
	{
		// Prevents blinking a little.
		window(Dt);

		for(buf_t line = 0; line <= Dt->lines; line++)
		{
			// Write each line to the file. NULL terminator is ignored.
			fputs(Dt->text[line], Dt->textf);
		}
		fclose(Dt->textf);

		SET_STATUS("saved\0");

		flush_window(Dt);
	}
	else
	{
		SET_STATUS("can't write to the file");
	}
}

meta* shift_text_horizonally(char direction, meta* Dt)
{
	switch(direction)
	{
		case 'l':
			if(Dt->cusr_x > ACT_LN_LEN - NTERM_SZ && ACT_LN_LEN > 0)
			{
				Dt->cusr_x = ACT_LN_LEN - NTERM_SZ;
			}
			if(ACT_LN_LEN > 0)
			{
				for(buf_t x = ACT_LN_LEN - Dt->cusr_x; x <= ACT_LN_LEN; x++)
				{
					ACT_LN[x - INDEX] = ACT_LN[x];
				}
			}
			break;

		case 'r':
			for(buf_t x = ACT_LN_LEN; x >= ACT_LN_LEN - Dt->cusr_x; x--)
			{
				ACT_LN[x] = ACT_LN[x - INDEX];
			}
	}
	return Dt;
}

