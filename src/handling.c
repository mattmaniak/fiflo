#include "handling.h"

void fnameset(buf dt, const char* passed)
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
		strcpy(dt.fname, passed); // Malloc'ed so doesn't need 'n' for size.
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

		strcpy(dt.fname, abs_path); // Copy the path.
		dt.fname[strlen(abs_path)] = '/'; // Add the slash between.
		for(uint16_t pos = 0; pos < strlen(passed); pos++) // Append basename.
		{
			strcpy(&dt.fname[strlen(abs_path) + slash_sz + pos], &passed[pos]);
		}
		free(abs_path);
	}
}

buf readfile(buf dt)
{
	FILE* textfile = fopen(dt.fname, "r");
	char chr;

	if(textfile)
	{
		while((chr = (char) getc(textfile)) != EOF)
		{
			dt = charadd(dt, chr);
		}
		fclose(textfile);
	}
	else
	{
		CURRLN[dt.chrs] = NTERM;
	}
	return dt;
}

void savefile(buf dt)
	{
	if(access(dt.fname, F_OK) == -1) // There is no file.
	{
		int create = open(dt.fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(create == -1)
		{
			fputs("Failed to create the new file, exited.\n", stderr);
			freeallexit(dt, 1);
		}		
	}
	FILE* textfile = fopen(dt.fname, "w");
	checkptr(dt, textfile, "write to the file\0");

	for(buf_t ln = 0; ln <= dt.lns; ln++)
	{
		fputs(dt.txt[ln], textfile);
	}
	fclose(textfile);
}

_Noreturn void freeallexit(buf dt, _Bool code)
{
	free(dt.fname);
	for(buf_t ln = 0; ln <= dt.lns; ln++)
	{
		free(dt.txt[ln]);
	}
	free(dt.txt);
	exit(code);
}

buf freeblk(buf dt)
{
	_Bool line_back = 0;
	if(dt.chrs_ln - 1 % MEMBLK == MEMBLK - 1)
	{
		CURRLN =
		realloc(CURRLN, (2 * dt.chrs_ln) - MEMBLK);
		checkptr(dt, CURRLN, "free the memblock with chars\0");
	}
	if(dt.chrs_ln > 0)
	{
		dt.chrs_ln--;
		dt.chrs--;
	}
	else if(dt.chrs_ln == 0)
	{
		line_back = 1;
	}
	CURRLN[dt.chrs_ln] = NTERM;

	if(line_back == 1 && dt.lns > 0
	&& UPLN[strlen(UPLN) - NTERM_SZ] == LF)
	{
		free(CURRLN);
		dt.lns--;
		dt.chrs_ln = (buf_t) strlen(CURRLN) - NTERM_SZ;
		CURRLN[dt.chrs_ln] = NTERM;
		if(dt.chrs > 0) // Just for the LF removal.
		{
			dt.chrs--;
		}
	}
	return dt;
}

buf allocblk(buf dt, char mode)
{
	switch(mode)
	{
		case 'c':
			dt.chrs++;
			dt.chrs_ln++;
			if(dt.chrs_ln % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				CURRLN = realloc(CURRLN, dt.chrs_ln + MEMBLK);
				checkptr(dt, CURRLN, "alloc new memblock for chars\0");
			}
		break;

		case 'l':
			if(dt.lns++ >= MAX_LNS)
			{
				dt.lns = MAX_LNS;
			}
			else
			{
				if(dt.lns % MEMBLK == 0) // Allocates with the one line reserve.
				{
					dt.txt =
					realloc(dt.txt, sizeof(dt.txt) * (dt.lns + MEMBLK));
				}
				CURRLN = malloc(MEMBLK);
				checkptr(dt, CURRLN, "alloc byte in the new line\0");
			}
			CURRLN[dt.chrs_ln = 0] = NTERM;
		break;
	}
	return dt;
}

buf charadd(buf dt, char key)
{
	if(dt.chrs <= MAX_CHRS)
	{
		dt = allocblk(dt, 'c');
		CURRLN[dt.chrs_ln - NTERM_SZ] = key;
		CURRLN[dt.chrs_ln] = NTERM;
		switch(key)
		{
			case TAB:
				CURRLN[dt.chrs_ln - NTERM_SZ] = ' '; // Converts TAB to SPACE.
			break;			

			case LF:
				dt = allocblk(dt, 'l');
			break;
		}	
	}
	return dt;
}

buf recochar(buf dt, char key)
{
	if(key != ESCAPE)
	{
		switch(key)
		{
			case NEG: // Eg. CTRL+C - singal.
			case NTERM: // Only for the initialization.
			break;

			case CTRL_D:
				savefile(dt);
			break;

			case CTRL_X:
				freeallexit(dt, 0);
			break;

			case BACKSPACE:
				dt = freeblk(dt);
			break;

			default:
				dt = charadd(dt, key);
			break;
		}
	}
	return dt;
}

