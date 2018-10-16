#include "fiflo.h"
#include "api.h"

void fnameset(const char* arg, meta* Dt)
{
	Dt->fname = malloc(PATH_MAX);
	ptrcheck(Dt->fname, "malloc for the filename\0", Dt);

	if(arg[strlen(arg) - NTERM_SZ] == '/')
	{
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		freeallexit(1, Dt);
	}
	// Is absolute path.
	if(arg[0] == '/')
	{
		if(strlen(arg) + NTERM_SZ > PATH_MAX)
		{
			fputs("Given path is too long, exit(1).\n", stderr);
			freeallexit(1, Dt);
		}
		strcpy(Dt->fname, arg);
	}
	// Is relative path or basename.
	else
	{
		char* abs_path = malloc(PATH_MAX);
		ptrcheck(abs_path, "abs. path. Can be too long\0", Dt);

		ptrcheck((getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0", Dt);

		// Exceeded 4096 chars.
		if((strlen(abs_path) + strlen(arg)) >= PATH_MAX)
		{
			fputs("Given filename is too long, exit(1).\n", stderr);
			freeallexit(1, Dt);
		}
		// Copy the path.
		strcpy(Dt->fname, abs_path);

		// Add the slash between.
		Dt->fname[strlen(abs_path)] = '/';

		// Append basename.
		for(uint16_t pos = 0; pos < strlen(arg); pos++)
		{
			strcpy(&Dt->fname[strlen(abs_path) + SLASH_SZ + pos], &arg[pos]);
		}
		free(abs_path);
		abs_path = NULL;
	}
}

meta* readfile(meta* Dt)
{
	char chr;
	Dt->txtf = fopen(Dt->fname, "rt");

	if(Dt->txtf)
	{
		while((chr = (char) getc(Dt->txtf)) != EOF)
		{
			// Read all chars before end of file.
			Dt = addchar(chr, Dt);
		}
		fclose(Dt->txtf);
	}
	else
	{
		if(strcmp(Dt->fname, "noname.asdf") == 0)
		{
			fputs("Can't open the file, exit(1).\n", stderr);
			freeallexit(1, Dt);
		}
	}
	return Dt;
}

void savefile(meta* Dt)
{
	if(access(Dt->fname, F_OK) == -1)
	{
		// There is no file so create with -rw-rw-r-- file mode.
		int create = open(Dt->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			freeallexit(1, Dt);
		}
	}
	Dt->txtf = fopen(Dt->fname, "wt");
	ptrcheck(Dt->txtf, "write to the file\0", Dt);

	for(buf_t ln = 0; ln <= Dt->lns; ln++)
	{
		// Write each line to the file. NULL terminator is omited.
		fputs(Dt->txt[ln], Dt->txtf);
	}
	fclose(Dt->txtf);
}

meta* freeblk(meta* Dt) // TODO: SIMPLIFY.
{
//	_Bool line_back = 0;
	if(LAST_LN_LEN % MEMBLK == 0)
	{
		LAST_LN = realloc(LAST_LN, LAST_LN_LEN);
		ptrcheck(LAST_LN, "realloc memblock in the last line\0", Dt);

//		Dt->ln_len = realloc(Dt->ln_len, Dt->lns);
//		ptrcheck(LAST_LN, "array with lines length\0", Dt);
//		printf("%d\n", LAST_LN_LEN);
	}
	if(LAST_LN_LEN > 0 && Dt->cusr_x != (LAST_LN_LEN + INDEX))
	{
		LAST_LN_LEN--;
		Dt->chrs--;
	}
	else if(LAST_LN_LEN == 0)
	{
//		line_back = 1;
		Dt->cusr_y = 0;
	}
	LAST_LN[LAST_LN_LEN] = NTERM;
/*
	if(line_back == 1 && Dt->lns > 0 && PRE_LAST_LN[strlen(PRE_LAST_LN) - NTERM_SZ] == LF)
	{
		free(LAST_LN);
		LAST_LN = NULL;
		Dt->lns--;
		LAST_LN_LEN = (buf_t) strlen(LAST_LN) - NTERM_SZ;
		LAST_LN[LAST_LN_LEN] = NTERM;
		if(Dt->chrs > 0) // Just for the LF removal.
		{
			Dt->chrs--;
		}
	}*/
	return Dt;
}

meta* chrsalloc(meta* Dt)
{
	// Eg. allocation for memblk = 4: ++------, ++++----, ++++++++.
	if(CURR_LN_LEN == 2)
	{
		// If there are 2 chars + terminator, extend to MEMBLK.
		CURR_LN = realloc(CURR_LN, MEMBLK);
	}
	else if(CURR_LN_LEN > 2 && CURR_LN_LEN % MEMBLK == 0)
	{
		CURR_LN = realloc(CURR_LN, CURR_LN_LEN + MEMBLK);
	}
	ptrcheck(CURR_LN, "extend memblock for the current line\0", Dt);

	return Dt;
}

meta* txtshift(char direction, meta* Dt)
{
	switch(direction)
	{
		// Move left.
		case 'l':
			if(Dt->cusr_x > LAST_LN_LEN - NTERM_SZ && LAST_LN_LEN > 0)
			{
				Dt->cusr_x = LAST_LN_LEN - NTERM_SZ;
			}
			if(LAST_LN_LEN > 0)
			{
				for(term_t x = LAST_LN_LEN - Dt->cusr_x; x <= LAST_LN_LEN; x++)
				{
					LAST_LN[x - INDEX] = LAST_LN[x];
				}
			}
			break;

		// Move right.
		case 'r':
			for(term_t x = LAST_LN_LEN; x >= LAST_LN_LEN - Dt->cusr_x; x--)
			{
				LAST_LN[x] = LAST_LN[x - INDEX];
			}
			break;
	}
	return Dt;
}

meta* keymap(meta* Dt, char key) // TODO: KEYMAP.
{
	// Prevent inputting ANSI escape sequences.
	if(key != ESCAPE)
	{
		switch(key)
		{
			// Pipe prevention.
			case NEG:
				puts("\rPipe isn't supported by fiflo, exit(1).");
				freeallexit(1, Dt);

			case CTRL_X:
				freeallexit(0, Dt);

			case CTRL_D:
				savefile(Dt);
				break;

			case CTRL_H:
				Dt = ctrlh(Dt);
				break;

			case CTRL_G:
				Dt = ctrlg(Dt);
				break;

			case CTRL_Y:
				Dt = ctrly(Dt);
				break;

			case CTRL_B:
				Dt = ctrlb(Dt);
				break;

			default:
				Dt = addchar(key, Dt);
				break;

			case BACKSPACE:
				Dt = backspace(Dt);
				break;

			case LF:
				Dt = addchar(key, Dt);
				Dt = linefeed(Dt);
				break;				
		}
	}
	// DEBUG ONLY - BREAKS RENDERING AND FLUSHING.
//	printf("\rlast: %d cusr_x: %d cusr_y: %d\n", key, Dt->cusr_x, Dt->cusr_y);
	return Dt;
}

