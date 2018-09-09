#include "handling.h"

void fnameset(buff dt, const char* passed)
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

buff readfile(buff dt)
{
	FILE* textfile = fopen(dt.fname, "r");
	char chr;

	if(textfile)
	{
		while((chr = (char) getc(textfile)) != EOF)
		{
			dt = recochar(dt, chr);
		}
		fclose(textfile);
	}
	else
	{
		dt.txt[dt.lns][dt.chrs] = NTERM;
	}
	return dt;
}

void savefile(buff dt)
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

	for(buff_t ln = 0; ln <= dt.lns; ln++)
	{
		fputs(dt.txt[ln], textfile);
	}
	fclose(textfile);
}

buff allocblk(buff dt, char mode)
{
	switch(mode)
	{
		case 'c':
			if(dt.chrs < MAX_CHRS)
			{
				dt.chrs++;
				dt.chrs_ln++;
			}
			if(dt.chrs_ln % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				dt.txt[dt.lns] = realloc(dt.txt[dt.lns], dt.chrs_ln + MEMBLK);
				checkptr(dt, dt.txt[dt.lns], "alloc the new memblock for chars\0");
			}
		break;

		case 'l':
			if(dt.lns < MAX_LNS)
			{
				dt.lns++;
			}
			if(dt.lns % MEMBLK == 0) // Allocates with the one line reserve.
			{
				dt.txt = realloc(dt.txt, sizeof(dt.txt) * (dt.lns + MEMBLK));
			}
			dt.chrs_ln = 0;
			dt.txt[dt.lns] = malloc(MEMBLK);
			checkptr(dt, dt.txt[dt.lns], "malloc byte in the new line\0");
		break;
	}
//	limits(dt);
	return dt;
}

buff freeblk(buff dt)
{
	if(dt.chrs_ln - 1 % MEMBLK == MEMBLK - 1)
	{
		dt.txt[dt.lns] =
		realloc(dt.txt[dt.lns], (2 * dt.chrs_ln) - MEMBLK);
		checkptr(dt, dt.txt[dt.lns], "free the memory block with chars\0");
	}
	if(dt.chrs > 0)
	{
		dt.chrs_ln--;
		dt.chrs--;
	}
	// TODO: LINE DELETION.
	return dt;
}

void freeallexit(buff dt, _Bool code)
{
	free(dt.fname);
	for(buff_t ln = 0; ln <= dt.lns; ln++)
	{
		free(dt.txt[ln]);
	}
	free(dt.txt);
	exit(code);
}

buff charadd(buff dt, char key)
{
	dt = allocblk(dt, 'c');
	dt.txt[dt.lns][dt.chrs_ln - NTERM_SZ] = key;
	switch(key)
	{
		case TAB:
			if((dt.chrs_ln + INDEX) % 8 == 0) // TODO: TAB SUPPORT WITH CURSOR.
			{
				dt.cusr_x += 8;
			}
			else
			{
				dt.cusr_x += (dt.chrs_ln + INDEX) % 8;
			}
		break;			

		case LF:
			dt.txt[dt.lns][dt.chrs_ln] = NTERM;
			dt = allocblk(dt, 'l');
		break;
	}
	dt.txt[dt.lns][dt.chrs_ln] = NTERM;
	return dt;
}

buff keyboard_shortcut(buff dt, char key)
{
	switch(key)
	{
		case CTRL_D:
			savefile(dt);
		break;

		case CTRL_X:
			freeallexit(dt, 0);
		break;
	}
	return dt;
}

buff recochar(buff dt, char key)
{
	if(key == NEG || key == NTERM || key == CTRL_D || key == BELL
	|| key == BACKSPACE || key == TAB || key == LF || key == VTAB
	|| key == FORMFEED || key == CR || key == CTRL_X
	|| (key >= 32 && key < 127))
	{
		switch(key)
		{
			case NEG: // Eg. CTRL+C - singal.
			case NTERM: // Only for the initialization.
			break;

			case CTRL_D:
			case CTRL_X:
				dt = keyboard_shortcut(dt, key);
			break;

			case BACKSPACE:
				dt = freeblk(dt);
				dt.txt[dt.lns][dt.chrs_ln] = NTERM;
			break;

			default:
				dt = charadd(dt, key);
			break;
		}
	}
	return dt;
}

