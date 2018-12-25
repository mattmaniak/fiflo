#include "include/buffer.h"
#include "include/ascii.h"
#include "include/file.h"

#include "include/memory.h"
#include "include/charmap.h"

F_mtdt* set_name(F_mtdt* Buff, const char* arg)
{
	const bool nul_sz   = 1;
	const bool slash_sz = 1;
	uint16_t   arg_len  = (uint16_t) strlen(arg);

	bool arg_non_empty   = arg_len > 0;
	bool arg_as_abs_path = arg[0]  == '/';
	bool arg_as_dir      = (arg[arg_len - nul_sz] == '/') && (arg[arg_len] == '\0');

	if(arg_non_empty && arg_as_dir)
	{
		buffer.throw_error(Buff, "Can't open the directory as a file.");
	}

	if(arg_as_abs_path)
	{
		if((arg_len + nul_sz) > PATH_MAX)
		{
			buffer.throw_error(Buff, "Passed filename is too long.");
		}
		strncpy(Buff->fname, arg, PATH_MAX);
	}
	else // Relative path or the basename.
	{
		char* cw_dir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		memory.chk_ptr(Buff, cw_dir, "malloc for the current path");

		cw_dir = getcwd(cw_dir, PATH_MAX - NAME_MAX - slash_sz);
		memory.chk_ptr(Buff, cw_dir, "get current path. Too long");

		if((strlen(cw_dir) + arg_len) >= PATH_MAX)
		{
			buffer.throw_error(Buff, "Current directory is too long.");
		}
		// Copy the path.
		strncpy(Buff->fname, cw_dir, PATH_MAX - NAME_MAX - slash_sz);

		Buff->fname[strlen(cw_dir)] = '/'; // Add the slash between.

		// Append the basename.
		for(uint16_t char_i = 0; char_i < arg_len; char_i++)
		{
			Buff->fname[strlen(cw_dir) + char_i + slash_sz] = arg[char_i];
		}
		free(cw_dir);
	}
	Buff->fname_len_i = (uint16_t) strlen(Buff->fname);
	return Buff;
}

F_mtdt* live_edit_name(F_mtdt* Buff, char key)
{
	const bool index = 1;

	if((key >= 32) && (key != BACKSPACE)
	&& ((Buff->fname_len_i + index) < PATH_MAX))
	{
		Buff->fname[Buff->fname_len_i] = key;
		Buff->fname_len_i++;
		Buff->fname[Buff->fname_len_i] = '\0';
	}
	else if((key == BACKSPACE) && (Buff->fname_len_i > 0))
	{
		Buff->fname_len_i--;
		Buff->fname[Buff->fname_len_i] = '\0';
	}
	else if(key == '\n')
	{
		Buff->live_fname_edit = false;
		SET_STATUS("filename edited");
	}
	return Buff;
}

F_mtdt* load(F_mtdt* Buff)
{
	const bool nul_sz = 1;
	FILE* textfile = fopen(Buff->fname, "r");
	char ch;

	if(textfile != NULL)
	{
		if(Buff->fname[Buff->fname_len_i - nul_sz] == '/')
		{
			SET_STATUS("current directory set");
			fclose(textfile);
			return Buff;
		}
		while((ch = (char) fgetc(textfile)) != EOF)
		{
			if(ch == '\t') // Temponary and ugly tab to two spaces conversion.
			{
				ch = ' ';
				Buff = charmap.printable_char(Buff, ch);
			}
			Buff = charmap.printable_char(Buff, ch);
		}
		fclose(textfile);
		SET_STATUS("read the file");
	}
	else
	{
		SET_STATUS("the file will be created");
	}
	return Buff;
}

F_mtdt* save(F_mtdt* Buff)
{
	const int8_t not_created = -1;

	int status = access(Buff->fname, F_OK);
	FILE* textfile;

	if(status == not_created)
	{
		// There is no file so create with -rw------- mode.
		int create = open(Buff->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		(create == not_created) ? SET_STATUS("failed to create the file") : 0;
	}
	textfile = fopen(Buff->fname, "w");

	if(textfile != NULL)
	{
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
		SET_STATUS("saved");
	}
	else
	{
		SET_STATUS("can't write to the file");
	}
	return Buff;
}
