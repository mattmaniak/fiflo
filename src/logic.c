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
		SET_STATUS("read the file\0");
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
		for(buf_t line = 0; line <= Dt->lines; line++)
		{
			// Write each line to the file. NULL terminator is ignored.
			fputs(Dt->text[line], Dt->textf);
		}
		fclose(Dt->textf);

		SET_STATUS("saved\0");
	}
	else
	{
		SET_STATUS("can't write to the file");
	}
}

meta* extend_act_line_mem(meta* Dt)
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

meta* extend_prev_line_mem(meta* Dt)
{
	if(PREV_LN_LEN == INIT_MEMBLK)
	{
		// If there are 4/8 chars, extend to MEMBLK.
		PREV_LN = realloc(PREV_LN, MEMBLK);
	}
	else if(PREV_LN_LEN > INIT_MEMBLK && PREV_LN_LEN % MEMBLK == 0)
	{
		PREV_LN = realloc(PREV_LN, ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "extend a memblock for the previous line\0", Dt);

	return Dt;
}

meta* shrink_act_line_mem(meta* Dt)
{
	/* These cases are executed only when the backspace is pressed. Works in the
 	same way as "extend_act_line_mem". */
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

meta* shrink_prev_line_mem(meta* Dt)
{
	if(PREV_LN_LEN < INIT_MEMBLK)
	{
		// Set the previus' line memblock to initializing memblock.
		PREV_LN = realloc(PREV_LN, INIT_MEMBLK);
	}
	else if(PREV_LN_LEN >= INIT_MEMBLK && PREV_LN_LEN < MEMBLK)
	{
		// Set to the full memory block.
		PREV_LN = realloc(PREV_LN, MEMBLK);
	}
	else if(PREV_LN_LEN >= MEMBLK)
	{
		// Set the size of some MEMBLKs.
		PREV_LN = realloc(PREV_LN, ((PREV_LN_LEN / MEMBLK) * MEMBLK) + MEMBLK);
	}
	chk_ptr(PREV_LN, "shrink the upper line's memory\0", Dt);

	return Dt;
}

meta* extend_lines_array(meta* Dt)
{
	// Enhance the array that contains pointers to lines.
	Dt->text = realloc(Dt->text, (Dt->lines + INDEX) * sizeof(Dt->text));
	chk_ptr(Dt->text, "extend the array with lines\0", Dt);

	// Enhance the array that contains lines length indicators.
	Dt->line_len = realloc(Dt->line_len, (Dt->lines + INDEX) * sizeof(buf_t));
	chk_ptr(Dt->line_len, "extend the array with lines length\0", Dt);

	// The new line is allocated with only 4 or 8 bytes bytes.
	ACT_LN = malloc(INIT_MEMBLK);
	chk_ptr(ACT_LN, "alloc initial block for the newly created line\0", Dt);

	return Dt;
}

meta* shrink_lines_array(meta* Dt)
{
	Dt->text = realloc(Dt->text, (Dt->lines + INDEX) * sizeof(Dt->text));
	chk_ptr(Dt->text, "shrink the array with lines\0", Dt);

	Dt->line_len = realloc(Dt->line_len, (Dt->lines + INDEX) * sizeof(buf_t));
	chk_ptr(Dt->line_len, "shrink the array with lines length\0", Dt);

	return Dt;
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

