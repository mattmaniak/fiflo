#include "fiflo.h"
#include "logic.h"

meta* set_fname(const char* arg, meta* Dt)
{
	const _Bool slash_sz = 1;

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
	// Relative path or basename.
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
	Dt->txtf = fopen(Dt->fname, "rt");

	if(Dt->txtf)
	{
		while((chr = (char) getc(Dt->txtf)) != EOF)
		{
			// Read all chars before end of file.
			Dt = non_control_chr(chr, Dt);
		}
		fclose(Dt->txtf);
	}
	// Else will be created after save.

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
		// Write each line to the file. NULL terminator is ignored.
		fputs(Dt->txt[ln], Dt->txtf);
	}
	fclose(Dt->txtf);
}

meta* extend_curr_ln_mem(meta* Dt)
{
	if(ACT_LN_LEN == INIT_MEMBLK)
	{
		// Chars index == INIT_MEMBLK, extend to MEMBLK.
		ACT_LN = realloc(ACT_LN, MEMBLK);
	}
	else if(ACT_LN_LEN > INIT_MEMBLK && ACT_LN_LEN % MEMBLK == 0)
	{
		// If simply there is even more chars, append the new memblock.
		ACT_LN = realloc(ACT_LN, ((ACT_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(ACT_LN, "extend a memblock for the current line\0", Dt);

	return Dt;
}

meta* shrink_curr_ln_mem(meta* Dt)
{
	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_curr_ln_mem". */
	if(ACT_LN_LEN == INIT_MEMBLK)
	{
		// Shrink to INIT_MEMBLOCK bytes.
		ACT_LN = realloc(ACT_LN, INIT_MEMBLK);
	}
	else if(ACT_LN_LEN == MEMBLK)
	{
		// Shrink to size of the MEMBLK.
		ACT_LN = realloc(ACT_LN, MEMBLK);
	}
	else if(ACT_LN_LEN > MEMBLK && ACT_LN_LEN % MEMBLK == 0)
	{
		// Remove the newest memblock because isn't needed now.
		ACT_LN = realloc(ACT_LN, (ACT_LN_LEN / MEMBLK) * MEMBLK);
	}
	chk_ptr(ACT_LN, "shrink a memblock for the current line\0", Dt);

	return Dt;
}

meta* shrink_prev_ln_mem(meta* Dt)
{
	if(PREV_LN_LEN < INIT_MEMBLK)
	{
		PREV_LN = realloc(PREV_LN, INIT_MEMBLK);
		puts("FREE UPPER: 8");
	}
	else if(PREV_LN_LEN >= INIT_MEMBLK && PREV_LN_LEN < MEMBLK)
	{
		PREV_LN = realloc(PREV_LN, MEMBLK);
		puts("FREE UPPER: 16");
	}
	else if(PREV_LN_LEN >= MEMBLK)
	{
		PREV_LN = realloc(PREV_LN,
		((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "shrink the upper line's memory space\0", Dt);

	return Dt;
}

meta* extend_lns_array(meta* Dt)
{
	// Enhance the array that contains pointers to lines.
	Dt->txt = realloc(Dt->txt, (Dt->lns + INDEX) * sizeof(Dt->txt));
	chk_ptr(Dt->txt, "extend the array with lines\0", Dt);

	// Enhance the array that contains lines length indicators.
	Dt->ln_len = realloc(Dt->ln_len, (Dt->lns + INDEX) * sizeof(buf_t));
	chk_ptr(Dt->ln_len, "extend the array with lines length\0", Dt);

	// The new line is allocated with only 4 or 8 bytes bytes.
	ACT_LN = malloc(INIT_MEMBLK);
	chk_ptr(ACT_LN, "alloc 2 bytes for the newly created line\0", Dt);

	return Dt;
}

meta* shrink_lns_array(meta* Dt)
{
	Dt->txt = realloc(Dt->txt, (Dt->lns + INDEX) * sizeof(Dt->txt));
	chk_ptr(Dt->txt, "shrink the array with lines\0", Dt);

	Dt->ln_len = realloc(Dt->ln_len, (Dt->lns + INDEX) * sizeof(buf_t));
	chk_ptr(Dt->ln_len, "shrink the array with lines length\0", Dt);

	return Dt;
}

meta* shift_txt_horizonally(char direction, meta* Dt)
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
				for(term_t x = ACT_LN_LEN - Dt->cusr_x; x <= ACT_LN_LEN; x++)
				{
					ACT_LN[x - INDEX] = ACT_LN[x];
				}
			}
			break;

		case 'r':
			for(term_t x = ACT_LN_LEN; x >= ACT_LN_LEN - Dt->cusr_x; x--)
			{
				ACT_LN[x] = ACT_LN[x - INDEX];
			}
	}
	return Dt;
}

meta* recognize_key(char key, meta* Dt)
{
	switch(key)
	{
		case NEG:
			fputs("Pipe isn't supported by the fiflo, exit(1).\n", stderr);
			free_all_exit(1, Dt);

		default:
			/* Only printable chars will be added. Combinations that aren't
			specified above will be omited. Set "key != ESCAPE" to enable. */
			if(key == NTERM || key == LF || key >= 32)
			{
				Dt = non_control_chr(key, Dt);
			}
			break;

		case TAB:
			// Currently converts tab to one space.
			Dt = non_control_chr(' ', Dt);
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
	}
//	printf("\rlast: %d cusr_x: %d\n", key, Dt->cusr_x); // DEBUG
	return Dt;
}

