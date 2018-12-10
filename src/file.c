#include "ascii.h"
#include "buffer.h"
#include "file.h"

f_mtdt* set_fname(f_mtdt* Buff, const char* arg)
{
	const bool slash_sz = 1;
	uint16_t   arg_len  = (uint16_t) strlen(arg);

	bool arg_non_empty = arg_len > 0;
	bool arg_as_dir    =
	(arg[arg_len - NUL_SZ] == '/') && (arg[arg_len] == NUL__CTRL_SHIFT_2);

	bool arg_as_abs_path = arg[0] == '/';

	if(arg_non_empty)
	{
		if(arg_as_dir)
		{
			fputs("Can't open the directory as a file.\n", stderr);
			free_buff_exit(Buff, 1);
		}
	}

	if(arg_as_abs_path)
	{
		if((arg_len + NUL_SZ) > PATH_MAX)
		{
			fputs("Passed filename is too long.\n", stderr);
			free_buff_exit(Buff, 1);
		}
		strncpy(Buff->fname, arg, PATH_MAX);
	}
	// Relative path or the basename.
	else
	{
		char* cw_dir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		chk_ptr(Buff, cw_dir, "malloc for the current path\0");

		cw_dir = getcwd(cw_dir, PATH_MAX - NAME_MAX - slash_sz);
		chk_ptr(Buff, cw_dir, "get current path. Too long\0");

		if((strlen(cw_dir) + arg_len) >= PATH_MAX)
		{
			fputs("Current directory is too long.\n", stderr);
			free_buff_exit(Buff, 1);
		}
		// Copy the path.
		strncpy(Buff->fname, cw_dir, PATH_MAX - NAME_MAX - slash_sz);

		// Add the slash between.
		Buff->fname[strlen(cw_dir)] = '/';

		// Append the basename.
		for(uint16_t char_i = 0; char_i < arg_len; char_i++)
		{
			Buff->fname[strlen(cw_dir) + char_i + slash_sz] = arg[char_i];
		}
		safer_free(cw_dir);
	}
	Buff->fname_len_i = (uint16_t) strlen(Buff->fname);
	return Buff;
}

f_mtdt* read_file(f_mtdt* Buff)
{
	FILE* textfile = fopen(Buff->fname, "r");
	char  ch;

	if(textfile != NULL)
	{
		if(Buff->fname[Buff->fname_len_i - NUL_SZ] == '/')
		{
			SET_STATUS("current directory set\0");
			fclose(textfile);
			return Buff;
		}
		while((ch = (char) fgetc(textfile)) != EOF)
		{
			// Temponary and ugly tab to two spaces conversion.
			if(ch == HT__CTRL_I)
			{
				ch   = ' ';
				Buff = printable_char(Buff, ch);
			}

			// Read all chars before end of file.
			Buff = printable_char(Buff, ch);
		}
		fclose(textfile);
		SET_STATUS("read the file\0");
	}
	else
	{
		SET_STATUS("the file will be created\0");
	}
	return Buff;
}

f_mtdt* save_file(f_mtdt* Buff)
{
	const int8_t not_created = -1;

	int   file_status = access(Buff->fname, F_OK);
	FILE* textfile;

	if(file_status == not_created)
	{
		// There is no file so create with -rw------- mode.
		int create = open(Buff->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		(create == not_created) ? SET_STATUS("failed to create the file\0") : 0;
	}
	textfile = fopen(Buff->fname, "w");

	if(textfile != NULL)
	{
		// Write each line to the file. NULL terminator is ignored.
		for(buff_t line_i = 0; line_i <= Buff->lines_i; line_i++)
		{
			/* Using fputs or fprintf causes use-of-uninitialized-value using
			MSan because of there is more memory allocated than is needed. */
			for(buff_t char_i = 0; char_i < Buff->line_len_i[line_i]; char_i++)
			{
				putc(Buff->text[line_i][char_i], textfile);
			}
		}
		fclose(textfile);
		SET_STATUS("saved\0");
	}
	else
	{
		SET_STATUS("can't write to the file\0");
	}
	return Buff;
}

f_mtdt* edit_fname(f_mtdt* Buff, char key)
{
	const bool index = 1;

	if((key >= 32) && (key != DEL__BACKSPACE)
	&& ((Buff->fname_len_i + index) < PATH_MAX))
	{
		Buff->fname[Buff->fname_len_i] = key;
		Buff->fname_len_i++;
		Buff->fname[Buff->fname_len_i] = NUL__CTRL_SHIFT_2;
	}
	else if((key == DEL__BACKSPACE) && (Buff->fname_len_i > 0))
	{
		Buff->fname_len_i--;
		Buff->fname[Buff->fname_len_i] = NUL__CTRL_SHIFT_2;
	}
	else if(key == LF__CTRL_J)
	{
		Buff->live_fname_edit = false;
		SET_STATUS("filename edited\0");
	}
	return Buff;
}
