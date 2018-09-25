#include "api.h"

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
		const _Bool slash_sz = 1;
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
			strcpy(&dt->fname[strlen(abs_path) + slash_sz + pos], &passed[pos]);
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
	else
	{
		CURRLN[dt->chrs] = NTERM;
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

_Noreturn void freeallexit(buf* dt, _Bool code)
{
	free(dt->fname);
	for(buf_t ln = 0; ln <= dt->lns; ln++)
	{
		free(dt->txt[ln]);
	}
	free(dt->txt);
	free(dt);
	exit(code);
}

buf* freeblk(buf* dt)
{
	_Bool line_back = 0;
	if(dt->chrs_ln - 1 % MEMBLK == MEMBLK - 1)
	{
		CURRLN = realloc(CURRLN, (2 * dt->chrs_ln) - MEMBLK);
		checkptr(dt, CURRLN, "free the memblock with chars\0");
	}
	if(dt->chrs_ln > 0)
	{
		dt->chrs_ln--;
		dt->chrs--;
	}
	else if(dt->chrs_ln == 0)
	{
		line_back = 1;
	}
	CURRLN[dt->chrs_ln] = NTERM;

	if(line_back == 1 && dt->lns > 0
	&& UPLN[strlen(UPLN) - NTERM_SZ] == LF)
	{
		free(CURRLN);
		dt->lns--;
		dt->chrs_ln = (buf_t) strlen(CURRLN) - NTERM_SZ;
		CURRLN[dt->chrs_ln] = NTERM;
		if(dt->chrs > 0) // Just for the LF removal.
		{
			dt->chrs--;
		}
	}
	return dt;
}

buf* allocblk(buf* dt, char mode)
{
	switch(mode)
	{
		case 'c':
			dt->chrs++;
			dt->chrs_ln++;
			if(dt->chrs_ln % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				CURRLN = realloc(CURRLN, dt->chrs_ln + MEMBLK);
				checkptr(dt, CURRLN, "alloc new memblock for chars\0");
			}
			break;

		case 'l':
			if(dt->lns++ >= MAX_LNS)
			{
				dt->lns = MAX_LNS;
			}
			else
			{
				if(dt->lns % MEMBLK == 0) // Allocates with a one line reserve.
				{
					dt->txt =
					realloc(dt->txt, sizeof(dt->txt) * (dt->lns + MEMBLK));
				}
				CURRLN = malloc(MEMBLK);
				checkptr(dt, CURRLN, "alloc byte in the new line\0");
			}
			CURRLN[dt->chrs_ln = 0] = NTERM;
			break;
	}
	return dt;
}

buf* charadd(buf* dt, char key)
{
	if(dt->chrs <= MAX_CHRS)
	{
		dt = allocblk(dt, 'c');
		CURRLN[LASTCHR] = key;
		CURRLN[dt->chrs_ln] = NTERM;

		dt->cusr_x = dt->chrs_ln;
		if(dt->cusr_x > termgetsz(dt, 'X') - CURRENT - 5)
		{
			dt->cusr_x = termgetsz(dt, 'X') - CURRENT - 5;
		}
		switch(key)
		{
			case TAB:
				CURRLN[LASTCHR] = ' '; // Currently onverts TAB to SPACE.
				break;

			case LF:
				dt = allocblk(dt, 'l');
				dt->cusr_y = dt->lns;
				dt->cusr_x = 0;
				break;
		}
	}
	return dt;
}

buf* recochar(buf* dt, char key) // TODO: KEYMAP.
{
	if(key != ESCAPE)
	{
		switch(key)
		{
			case NTERM: // Only for the initialization.
				break;

			case NEG: // Pipe and signal prevention. TODO: FULL UPPER BAR FLUSH.
				freeallexit(dt, 0);

			case CTRL_D:
				savefile(dt);
				break;

			case CTRL_H:
				if(dt->cusr_x < termgetsz(dt, 'X') - CURRENT - 5)
				{
					dt->cusr_x++;
				}
				break;

			case CTRL_G:
				if(dt->cusr_x > 0)
				{
					dt->cusr_x--;
				}
				break;

			case CTRL_Y:
				if(dt->cusr_y > 0)
				{
					dt->cusr_y--;
				}
				break;

			case CTRL_B:
				if(dt->cusr_y < (termgetsz(dt, 'Y') - 2) - CURRENT)
				{
					dt->cusr_y++;
				}
				break;

			case BACKSPACE:
				dt->cusr_x = dt->chrs_ln;
				dt->cusr_y = dt->lns - 1;
				dt = freeblk(dt);
				break;

			default:
				dt = charadd(dt, key);
				break;
		}
	}
	printf("cusr_x: %d cusr_y: %d\n", dt->cusr_x, dt->cusr_y); // DEBUG
	return dt;
}

