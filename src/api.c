#include "fiflo.h"
#include "api.h"

meta* set_fname(const char* arg, meta* Dt)
{
	Dt->fname = malloc(PATH_MAX);
	chk_ptr(Dt->fname, "malloc for the filename\0", Dt);

	if(arg[strlen(arg) - NTERM_SZ] == '/')
	{
		fputs("Can't open directory as file, exit(1).\n", stderr);
		free_all_exit(1, Dt);
	}
	// Is absolute path.
	if(arg[0] == '/')
	{
		if(strlen(arg) + NTERM_SZ > PATH_MAX)
		{
			fputs("Passed path is too long, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
		strcpy(Dt->fname, arg);
	}
	// Is relative path or basename.
	else
	{
		char* cw_dir = malloc(PATH_MAX);
		chk_ptr(cw_dir, "alloc memory for the current path\0", Dt);

		chk_ptr((getcwd(cw_dir, PATH_MAX)), "get current path. Too long\0", Dt);

		// Exceeded 4096 chars.
		if((strlen(cw_dir) + strlen(arg)) >= PATH_MAX)
		{
			fputs("Passed filename is too long, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
		// Copy the path.
		strcpy(Dt->fname, cw_dir);

		// Add the slash between.
		Dt->fname[strlen(cw_dir)] = '/';

		// Append basename.
		for(uint16_t pos = 0; pos < strlen(arg); pos++)
		{
			strcpy(&Dt->fname[strlen(cw_dir) + SLASH_SZ + pos], &arg[pos]);
		}
		free(cw_dir);
		cw_dir = NULL;
	}
	return Dt;
}

meta* read_file(meta* Dt)
{
	char chr;
	Dt->txtf = fopen(Dt->fname, "rt");

	if(Dt->txtf)
	{
		while((chr = (char) getc(Dt->txtf)) != EOF)
		{
			// Read all chars before end of file.
			Dt = recognize_key(chr, Dt);
		}
		fclose(Dt->txtf);
	}
	else // TODO
	{
		if(strcmp(Dt->fname, "noname.asdf") == 0)
		{
			fputs("Can't open the file, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
	}
	return Dt;
}

void save_file(meta* Dt)
{
	if(access(Dt->fname, F_OK) == -1)
	{
		// There is no file so create with -rw-rw-r-- file mode.
		int create = open(Dt->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			free_all_exit(1, Dt);
		}
	}
	Dt->txtf = fopen(Dt->fname, "wt");
	chk_ptr(Dt->txtf, "write to the file\0", Dt);

	for(buf_t ln = 0; ln <= Dt->lns; ln++)
	{
		// Write each line to the file. NULL terminator is omited.
		fputs(Dt->txt[ln], Dt->txtf);
	}
	fclose(Dt->txtf);
}

meta* add_mem_for_chrs(meta* Dt)
{
	// Eg. allocation for memblk = 4: ++------, ++++----, ++++++++.
	if(CURR_LN_LEN == 2)
	{
		// If there are 2 chars + terminator, extend to MEMBLK.
		CURR_LN = realloc(CURR_LN, MEMBLK);
		puts("ALLOC_EIGHT");
	}
	else if(CURR_LN_LEN > 2 && CURR_LN_LEN % MEMBLK == 0)
	{
		CURR_LN = realloc(CURR_LN, ((CURR_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
		printf("ALLOC: %d\n", ((CURR_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(CURR_LN, "extend a memblock for the current line\0", Dt);

	return Dt;
}

meta* free_mem_for_chrs(meta* Dt)
{
	if(CURR_LN_LEN == 2)
	{
		// If there are: char + terminator, shrink to 2 bytes.
		CURR_LN = realloc(CURR_LN, 1 + NTERM_SZ);
		puts("FREE TWO");
	}
	else if(CURR_LN_LEN == MEMBLK)
	{
		CURR_LN = realloc(CURR_LN, MEMBLK);
		puts("FREE EIGHT");
	}
	else if(CURR_LN_LEN > MEMBLK && CURR_LN_LEN % MEMBLK == 0)
	{
		// There is more memory needed.
		CURR_LN = realloc(CURR_LN, (CURR_LN_LEN / MEMBLK) * MEMBLK);
		printf("FREE %d\n", ((CURR_LN_LEN / MEMBLK) * MEMBLK));
	}
	chk_ptr(CURR_LN, "shrink a memblock for the current line\0", Dt);

	return Dt;
}

meta* alloc_mem_for_lns(meta* Dt)
{
	// Enhance pointer that contains pointers to lines.
	Dt->txt = realloc(Dt->txt, sizeof(Dt->txt) * (Dt->lns + MEMBLK));
	chk_ptr(Dt->txt, "extend the array with lines\0", Dt);

	// Enhance pointer that contains lines length indicators.
	Dt->ln_len = realloc(Dt->ln_len, Dt->lns + MEMBLK);
	chk_ptr(Dt->ln_len, "extend the array with lines length\0", Dt);

	// The new line is allocated with only 2 bytes.
	CURR_LN = malloc(1 + NTERM_SZ);
	chk_ptr(CURR_LN, "alloc 2 bytes for the initial line\0", Dt);

	return Dt;
}

meta* shift_txt_horizonally(char direction, meta* Dt)
{
	switch(direction)
	{
		// Move left.
		case 'l':
			if(Dt->cusr_x > CURR_LN_LEN - NTERM_SZ && CURR_LN_LEN > 0)
			{
				Dt->cusr_x = CURR_LN_LEN - NTERM_SZ;
			}
			if(CURR_LN_LEN > 0)
			{
				for(term_t x = CURR_LN_LEN - Dt->cusr_x; x <= CURR_LN_LEN; x++)
				{
					CURR_LN[x - INDEX] = CURR_LN[x];
				}
			}
			break;

		// Move right.
		case 'r':
			for(term_t x = CURR_LN_LEN; x >= CURR_LN_LEN - Dt->cusr_x; x--)
			{
				CURR_LN[x] = CURR_LN[x - INDEX];
			}
			break;
	}
	return Dt;
}

meta* recognize_key(char key, meta* Dt)
{
	switch(key)
	{
		case NEG:
			puts("\rPipe isn't supported by fiflo, exit(1).");
			free_all_exit(1, Dt);

		default:
			/* Only printable chars will be added. Combinations that aren't
			specified above will be omited. Set "key != ESCAPE" to enable. */
			if(key == NTERM || key >= 32)
			{
				Dt = add_chr_as_txt(key, Dt);
			}
			break;

		case LF:
			Dt = add_chr_as_txt(key, Dt);
			Dt = linefeed(Dt);
			break;

		case BACKSPACE:
			Dt = backspace(Dt);
			break;

		case CTRL_X:
			free_all_exit(0, Dt);

		case CTRL_D:
			save_file(Dt);
			break;

		case CTRL_G:
			Dt = ctrl_g(Dt);
			break;

		case CTRL_H:
			Dt = ctrl_h(Dt);
			break;
	}
//	printf("\rlast: %d cusr_x: %d\n", key, Dt->cusr_x); DEBUG
	return Dt;
}

