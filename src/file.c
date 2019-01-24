#include "buffer.h"
#include "config.h"
#include "ascii.h"
#include "file.h"

bool file__set_name(Buff_t* Buff, const char* arg)
{
	const size_t nul_sz   = 1;
	const size_t slash_sz = 1;
	size_t       arg_len  = strlen(arg);

	bool arg_non_empty   = arg_len > 0;
	bool arg_as_abs_path = arg[0]  == '/';
	bool arg_as_dir      = (arg[arg_len - nul_sz] == '/') && (arg[arg_len] == '\0');

	if(arg_non_empty && arg_as_dir)
	{
		fputs("Can't open the directory as a file.\n", stderr);
		return false;
	}

	if(arg_as_abs_path) // TODO
	{
		if((arg_len + nul_sz) > PATH_MAX)
		{
			fputs("Passed filename is too long.\n", stderr);
			return false;
		}
		strncpy(Buff->fname, arg, PATH_MAX);
	}
	else // Relative path or the basename.
	{
		char* cwdir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		if(cwdir == NULL)
		{
			fputs("Can't alloc a memory for the directory.\n", stderr);
			return false;
		}

		cwdir = getcwd(cwdir, PATH_MAX - NAME_MAX - slash_sz);
		if(cwdir == NULL)
		{
			fputs("Can't get the current directory. Maybe too long.\n", stderr);
			return false;
		}

		if((strlen(cwdir) + arg_len) >= PATH_MAX)
		{
			fputs("Current directory is too long.\n", stderr); // TODO.
			return false;
		}
		// Copy the path.
		strncpy(Buff->fname, cwdir, PATH_MAX - NAME_MAX - slash_sz);

		Buff->fname[strlen(cwdir)] = '/'; // Add the slash between.

		// Append the basename.
		for(uint16_t char_i = 0; char_i < arg_len; char_i++)
		{
			Buff->fname[strlen(cwdir) + char_i + slash_sz] = arg[char_i];
		}
		free(cwdir);
	}
	Buff->fname_len_i = strlen(Buff->fname);

	return true;
}

void file__live_edit_name(Buff_t* Buff, char key)
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
}

bool file__load(Buff_t* Buff)
{
	const size_t nul_sz = 1;
	FILE*        textfile = fopen(Buff->fname, "r");
	char         ch;

	if(textfile != NULL)
	{
		if(Buff->fname[Buff->fname_len_i - nul_sz] == '/')
		{
			SET_STATUS("current directory set");
			fclose(textfile);
			return true;
		}
		while((ch = (char) fgetc(textfile)) != EOF)
		{
			if(ch == '\t') // TODO: TAB_INSERT.
			{
				for(idx_t char_i = 0; char_i < (4 - 1); char_i++)
				{
					if(!keys__printable_char(Buff, ch))
					{
						return false;
					}
				}
			}
			if(!keys__printable_char(Buff, ch))
			{
				return false;
			}
		}
		fclose(textfile);
		SET_STATUS("read the file");
	}
	else
	{
		SET_STATUS("the file will be created");
	}
	return true;
}

void file__save(Buff_t* Buff)
{
	const int error = -1;
	int       file_descriptor;
	FILE*     textfile;

	if(access(Buff->fname, F_OK) == error)
	{
		// There is no file so create with -rw------- mode.
		file_descriptor = open(Buff->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(file_descriptor == error)
		{
			SET_STATUS("failed to create the file");
		}
		close(file_descriptor);
	}
	textfile = fopen(Buff->fname, "w");

	if(textfile != NULL)
	{
		for(idx_t line_i = 0; line_i <= Buff->lines_i; line_i++)
		{
			/* Using fputs or fprintf causes use-of-uninitialized-value using
			MSan because of there is more memory allocated than is needed. */
			for(idx_t char_i = 0; char_i < Buff->line_len_i[line_i]; char_i++)
			{
				if((Buff->text[line_i][char_i] == '\t')
				|| (Buff->text[line_i][char_i + 1] == '\t')
				|| (Buff->text[line_i][char_i + 2] == '\t')
				|| (Buff->text[line_i][char_i + 3] == '\t'))
				{
					char_i += (4 - 1);
				}
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
}

bool file__load_editor_config(Conf_t* Config)
{
	struct passwd* account_info;
	char           path[PATH_MAX]; // TODO: MAX SIZE.

	account_info  = getpwuid(getuid());
	if(account_info == NULL)
	{
		config__set_default(Config);
	}
	strcpy(path, account_info->pw_dir);
	strcat(path, "/.config/fiflorc");

	if(access(path, F_OK) == -1)
	{
		config__set_default(Config);
	}

	Config->file = fopen(path, "r");
	if(Config->file != NULL)
	{
		config__set_custom(Config);
		fclose(Config->file);
	}
	return true;
}

bool file__get_git_branch(Buff_t* Buff) // TODO
{
	const char git_head_path[] = "/.git/HEAD";
	// char       path[PATH_MAX]; // TODO: MAX SIZE.
	FILE*      git_head_file;

	char* cwdir = malloc(PATH_MAX - NAME_MAX - 1);
	if(cwdir == NULL)
	{
		fputs("Can't alloc a memory for the directory.\n", stderr);
		return false;
	}
	cwdir = getcwd(cwdir, PATH_MAX - NAME_MAX - 1);
	if(cwdir == NULL)
	{
		fputs("Can't get the current directory. Maybe too long.\n", stderr);
		return false;
	}

	// strcpy(cwdir, account_info->pw_dir);
	strcat(cwdir, git_head_path);

	// puts(cwdir);

	if(access(cwdir, F_OK) == -1)
	{
		strcpy(Buff->git_branch, "[branch not found]");
		return true;
	}

	git_head_file = fopen(cwdir, "r");
	if(git_head_file == NULL)
	{
		fprintf(stderr, "Can't open the \"%s\" to get a Git branch.\n",
		        git_head_path);
		return false;
	}
	free(cwdir);

	// Ignore the passed string in the file, to get the branch after the slash.
	if(fseek(git_head_file, (long) strlen("ref: refs/heads/"), 0) == -1)
	{
		fprintf(stderr, "Can't locate the branch in the \"%s\" file\n.",
		        git_head_path);
		return false;
	}
	while(fgets(Buff->git_branch, NAME_MAX + 1, git_head_file) != NULL) // TODO: MAX_LEN

	if(Buff->git_branch[strlen(Buff->git_branch) - 1] == '\n')
	{
		Buff->git_branch[strlen(Buff->git_branch) - 1] = '\0'; // LF removal.
	}
	fclose(git_head_file);
	return true;
}
