#include "fiflo.h"
#include "api.h"

void fnameset(meta* Dat, const char* arg)
{
	// Is directory.
	if(arg[strlen(arg) - NTERM_SZ] == '/')
	{
		fputs("Can't open the directory as a file, exit(1).\n", stderr);
		freeallexit(1, Dat);
	}
	// Is absolute path.
	if(arg[0] == '/')
	{
		if(strlen(arg) + NTERM_SZ > PATH_MAX)
		{
			fputs("Given path is too long, exit(1).\n", stderr);
			freeallexit(1, Dat);
		}
		// Malloc'ed so doesn't need 'n' for size.
		strcpy(Dat->fname, arg);
	}
	// Is relative path or basename.
	else
	{
		char* abs_path = malloc(PATH_MAX);

		ptrcheck((getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0", Dat);

		// Exceeded 4096 chars.
		if((strlen(abs_path) + strlen(arg) + NTERM_SZ) > PATH_MAX)
		{
			fputs("Given filename is too long, exit(1).\n", stderr);
			freeallexit(1, Dat);
		}

		// Copy the path.
		strcpy(Dat->fname, abs_path);

		// Add the slash between.
		Dat->fname[strlen(abs_path)] = '/';

		// Append a basename.
		for(uint16_t pos = 0; pos < strlen(arg); pos++)
		{
			strcpy(&Dat->fname[strlen(abs_path) + SLASH_SZ + pos], &arg[pos]);
		}
		free(abs_path);
	}
}

meta* readfile(meta* Dat)
{
	char chr;
	Dat->txtf = fopen(Dat->fname, "rt");

	if(Dat->txtf)
	{
		while((chr = (char) getc(Dat->txtf)) != EOF)
		{
			// Read all chars before end of file.
			Dat = addchar(Dat, chr);
		}
		fclose(Dat->txtf);
	}
	return Dat;
}

void savefile(meta* Dat)
{
	if(access(Dat->fname, F_OK) == -1)
	{
		// There is no file so create with -rw-rw-r-- file mode.
		int create = open(Dat->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exit(1).\n", stderr);
			freeallexit(1, Dat);
		}
	}
	Dat->txtf = fopen(Dat->fname, "wt");
	ptrcheck(Dat->txtf, "write to the file\0", Dat);

	for(buf_t ln = 0; ln <= Dat->lns; ln++)
	{
		// Write each line to the file. NULL terminator is omited.
		fputs(Dat->txt[ln], Dat->txtf);
	}
	fclose(Dat->txtf);
}

meta* freeblk(meta* Dat)
{
	_Bool line_back = 0;
	if(Dat->ln_len[Dat->lns] - 1 % MEMBLK == MEMBLK - 1)
	{
		CURR_LN = realloc(CURR_LN, (2 * Dat->ln_len[Dat->lns]) - MEMBLK);
		ptrcheck(CURR_LN, "free the memblock with a line\0", Dat);
		Dat->ln_len = realloc(Dat->ln_len, Dat->lns + MEMBLK);
		ptrcheck(CURR_LN, "free the memblock with a line\0", Dat);
	}
	if(Dat->ln_len[Dat->lns] > 0 && Dat->cusr_x != (Dat->ln_len[Dat->lns] + INDEX))
	{
		Dat->ln_len[Dat->lns]--;
		Dat->chrs--;
	}
	else if(Dat->ln_len[Dat->lns] == 0)
	{
		line_back = 1;
		Dat->cusr_y = 0;
	}
	CURR_LN[Dat->ln_len[Dat->lns]] = NTERM;

	if(line_back == 1 && Dat->lns > 0 && LN_ABOVE[strlen(LN_ABOVE) - NTERM_SZ] == LF)
	{
		free(CURR_LN);
		CURR_LN = NULL;
		Dat->lns--;
		Dat->ln_len[Dat->lns] = (buf_t) strlen(CURR_LN) - NTERM_SZ;
		CURR_LN[Dat->ln_len[Dat->lns]] = NTERM;
		if(Dat->chrs > 0) // Just for the LF removal.
		{
			Dat->chrs--;
		}
	}
	return Dat;
}

meta* allocblk(meta* Dat, char mode)
{
	switch(mode)
	{
		case 'c':
			Dat->chrs++;
			Dat->ln_len[Dat->lns]++;
			if(Dat->ln_len[Dat->lns] % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				CURR_LN = realloc(CURR_LN, Dat->ln_len[Dat->lns] + MEMBLK);
				ptrcheck(CURR_LN, "alloc new memblock for chars\0", Dat);
			}
			break;

		case 'l':
			if(Dat->lns++ >= MAX_LNS)
			{
				// TODO: COMMENTS.
				Dat->lns = MAX_LNS;
			}
			else
			{
				if(Dat->lns % MEMBLK == 0) // Allocates with a one line reserve.
				{
					Dat->txt =
					realloc(Dat->txt, sizeof(Dat->txt) * (Dat->lns + MEMBLK));
					ptrcheck(CURR_LN,
					"alloc the memblock with lines numbers\0", Dat);

					Dat->ln_len = realloc(Dat->ln_len, Dat->lns + MEMBLK);
					ptrcheck(Dat->ln_len,
					"alloc the memblock for sizes of lines.\0", Dat);
				}
				CURR_LN = malloc(MEMBLK);
				ptrcheck(CURR_LN, "alloc memblock in the line\0", Dat);
			}
			CURR_LN[Dat->ln_len[Dat->lns] = 0] = NTERM;
			break;
	}
	return Dat;
}

meta* txtshift(meta* Dat, char direction)
{
	switch(direction)
	{
		// Move left.
		case '<':
			if(Dat->cusr_x > Dat->ln_len[Dat->lns] - 1
			&& Dat->ln_len[Dat->lns] > 0)
			{
				Dat->cusr_x = Dat->ln_len[Dat->lns] - 1;
			}
			if(Dat->ln_len[Dat->lns] > 0)
			{
				for(term_t x = Dat->ln_len[Dat->lns] - Dat->cusr_x;
				x <= Dat->ln_len[Dat->lns]; x++)
				{
					CURR_LN[x - 1] = CURR_LN[x];
				}
			}
			break;

		// Move right.
		case '>':
			for(term_t x = Dat->ln_len[Dat->lns];
			x >= Dat->ln_len[Dat->lns] - Dat->cusr_x; x--)
			{
				CURR_LN[x] = CURR_LN[x - 1];
			}
			break;

		default:
			fputs("Check \"txtshift\" function params, exit(1).\n", stderr);
			freeallexit(1, Dat);
	}
	return Dat;
}

meta* addchar(meta* Dat, char key)
{
	if(Dat->chrs <= MAX_CHRS)
	{
		Dat = allocblk(Dat, 'c');

		/* If the cursor is moved to the left and a char is inserted, rest of
		the text will be shifted to the right side. */
		if(Dat->cusr_x > 0)
		{
			txtshift(Dat, '>');
		}

		Dat->txt[Dat->lns - Dat->cusr_y][Dat->ln_len[Dat->lns]
		 - NTERM_SZ - Dat->cusr_x] = key;

		Dat->txt[Dat->lns - Dat->cusr_y][Dat->ln_len[Dat->lns]] = NTERM;

		// Fixes incremented value by terminator.
		if(key == NTERM && Dat->chrs > 0)
		{
			Dat->chrs--;
			Dat->ln_len[Dat->lns]--;
		}

		if(key == LF)
		{
			Dat = allocblk(Dat, 'l');
			if(Dat->cusr_x > 0)
			{
				for(term_t x = (term_t) (strlen(LN_ABOVE) - Dat->cusr_x);
				x <= strlen(LN_ABOVE); x++)
				{
					CURR_LN[Dat->ln_len[Dat->lns]++] = LN_ABOVE[x];
				}
				LN_ABOVE[strlen(LN_ABOVE) - Dat->cusr_x] = NTERM;
				Dat->cusr_x = Dat->ln_len[Dat->lns];
			}
		}
	}
	return Dat;
}

meta* keymap(meta* Dat, char key) // TODO: KEYMAP.
{
	// Prevent inputting ANSI escape sequences.
	if(key != ESCAPE)
	{
		switch(key)
		{
			// Pipe and signal prevention. TODO: FULL UPPER BAR FLUSH (PIPE).
			case NEG:
				freeallexit(0, Dat);

			default:
				Dat = addchar(Dat, key);
				break;

			case BACKSPACE:
				Dat = backspace(Dat);
				break;

			case CTRL_D:
				savefile(Dat);
				break;

			case CTRL_H:
				Dat = ctrlh(Dat);
				break;

			case CTRL_G:
				Dat = ctrlg(Dat);
				break;

			case CTRL_Y:
				Dat = ctrly(Dat);
				break;

			case CTRL_B:
				Dat = ctrlb(Dat);
				break;
		}
	}
	// DEBUG
//	printf("\rlast: %d cusr_x: %d cusr_y: %d\n", key, Dat->cusr_x, Dat->cusr_y);
	return Dat;
}

