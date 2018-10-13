#include "fiflo.h"
#include "api.h"

void fnameset(meta* Dt, const char* arg)
{
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

		ptrcheck((getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0", Dt);

		// Exceeded 4096 chars.
		if((strlen(abs_path) + strlen(arg) + NTERM_SZ) > PATH_MAX)
		{
			fputs("Given filename is too long, exit(1).\n", stderr);
			freeallexit(1, Dt);
		}
		// Copy the path.
		strcpy(Dt->fname, abs_path);

		// Add the slash between.
		Dt->fname[strlen(abs_path)] = '/';

		// Append a basename.
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

meta* freeblk(meta* Dt)
{
	_Bool line_back = 0;
	if(LAST_LN_LEN - 1 % MEMBLK == MEMBLK - 1)
	{
		LAST_LN = realloc(LAST_LN, (2 * LAST_LN_LEN) - MEMBLK);
		ptrcheck(LAST_LN, "free the memblock with a line\0", Dt);
		Dt->ln_len = realloc(Dt->ln_len, Dt->lns + MEMBLK);
		ptrcheck(LAST_LN, "free the memblock with a line\0", Dt);
	}
	if(LAST_LN_LEN > 0 && Dt->cusr_x != (LAST_LN_LEN + INDEX))
	{
		LAST_LN_LEN--;
		Dt->chrs--;
	}
	else if(LAST_LN_LEN == 0)
	{
		line_back = 1;
		Dt->cusr_y = 0;
	}
	LAST_LN[LAST_LN_LEN] = NTERM;

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
	}
	return Dt;
}

meta* allocblk(meta* Dt, char mode)
{
	switch(mode)
	{
		case 'c':
			Dt->chrs++;
			LAST_LN_LEN++;
			if(LAST_LN_LEN % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				LAST_LN = realloc(LAST_LN, LAST_LN_LEN + MEMBLK);
				ptrcheck(LAST_LN, "alloc new memblock for chars\0", Dt);
			}
			break;

		case 'l':
			Dt->lns++;
			LAST_LN_LEN = 0;
			if(Dt->lns >= MAX_LNS)
			{
				// TODO: COMMENTS.
				Dt->lns = MAX_LNS;
			}
			else
			{
				if(Dt->lns % MEMBLK == 0) // Allocates with a one line reserve.
				{
					Dt->txt =
					realloc(Dt->txt, sizeof(Dt->txt) * (Dt->lns + MEMBLK));
					ptrcheck(LAST_LN,
					"alloc the memblock with lines numbers\0", Dt);

					Dt->ln_len = realloc(Dt->ln_len, Dt->lns + MEMBLK);
					ptrcheck(Dt->ln_len,
					"alloc the memblock for sizes of lines.\0", Dt);
				}
				LAST_LN = malloc(MEMBLK);
				ptrcheck(LAST_LN, "alloc memblock in the line\0", Dt);
			}
			LAST_LN[LAST_LN_LEN = 0] = NTERM;
			break;
	}
	return Dt;
}

meta* txtshift(meta* Dt, char direction)
{
	switch(direction)
	{
		// Move left.
		case '<':
			if(Dt->cusr_x > LAST_LN_LEN - 1 && LAST_LN_LEN > 0)
			{
				Dt->cusr_x = LAST_LN_LEN - 1;
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
		case '>':
			for(term_t x = LAST_LN_LEN; x >= LAST_LN_LEN - Dt->cusr_x; x--)
			{
				LAST_LN[x] = LAST_LN[x - INDEX];
			}
			break;

		default:
			fputs("Check \"txtshift\" function params, exit(1).\n", stderr);
			freeallexit(1, Dt);
	}
	return Dt;
}

meta* addchar(char key, meta* Dt)
{
	if(Dt->chrs <= MAX_CHRS)
	{
		Dt = allocblk(Dt, 'c');

		/* If the cursor is moved to the left and a char is inserted, rest of
		the text will be shifted to the right side. */
		if(Dt->cusr_x > 0)
		{
			txtshift(Dt, '>');
		}

		Dt->txt[Dt->lns - Dt->cusr_y]
		[LAST_LN_LEN  - NTERM_SZ - Dt->cusr_x] = key;

		Dt->txt[Dt->lns][LAST_LN_LEN] = NTERM;

		// Fixes incremented value by terminator.
		if(key == NTERM && Dt->chrs > 0)
		{
			Dt->chrs--;
			LAST_LN_LEN--;
		}
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

