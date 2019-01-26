#include "buffer.h"
#include "config.h"
#include "ascii.h"
#include "file.h"

bool file__set_name(Buff_t* Buffer, const char* arg)
{
	const size_t nul_sz   = 1;
	const size_t slash_sz = 1;
	size_t       arg_len  = strlen(arg);

	bool arg_non_empty   = arg_len > 0;
	bool arg_as_abs_path = arg[0]  == '/';
	bool arg_as_dir      = (arg[arg_len - nul_sz] == '/')
	                        && (arg[arg_len] == '\0');

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
		strncpy(Buffer->fname, arg, PATH_MAX);
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
		strncpy(Buffer->fname, cwdir, PATH_MAX - NAME_MAX - slash_sz);

		Buffer->fname[strlen(cwdir)] = '/'; // Add the slash between.

		// Append the basename.
		for(uint16_t char_i = 0; char_i < arg_len; char_i++)
		{
			Buffer->fname[strlen(cwdir) + char_i + slash_sz] = arg[char_i];
		}
		free(cwdir);
	}
	Buffer->fname_length = strlen(Buffer->fname);

	return true;
}

void file__live_edit_name(Buff_t* Buffer, char key)
{
	const bool index = 1;

	if((key >= 32) && (key != BACKSPACE)
	&& ((Buffer->fname_length + index) < PATH_MAX))
	{
		Buffer->fname[Buffer->fname_length] = key;
		Buffer->fname_length++;
		Buffer->fname[Buffer->fname_length] = '\0';
	}
	else if((key == BACKSPACE) && (Buffer->fname_length > 0))
	{
		Buffer->fname_length--;
		Buffer->fname[Buffer->fname_length] = '\0';
	}
	else if(key == '\n')
	{
		Buffer->live_fname_edit = false;
		SET_STATUS("filename edited");
	}
}

bool file__convert_from_tab(Buff_t* Buffer, Conf_t* Config, char ch)
{
	printf("tab %u\n", Config->Tab_width.value);
	if(ch == '\t') // TODO: TAB_INSERT.
	{
		for(idx_t char_idx = 0;
		    char_idx < (idx_t) (Config->Tab_width.value - 1); char_idx++)
		{
			if(!keys__printable_char(Buffer, ch))
			{
				return false;
			}
		}
	}
	return true;
}

bool file__load(Buff_t* Buffer, Conf_t* Config)
{
	const size_t nul_sz = 1;
	FILE*        Textfile = fopen(Buffer->fname, "r");
	char         ch;

	if(Textfile != NULL)
	{
		if(Buffer->fname[Buffer->fname_length - nul_sz] == '/')
		{
			SET_STATUS("current directory set");
			fclose(Textfile);
			return true;
		}
		while((ch = (char) fgetc(Textfile)) != EOF)
		{
			if(!file__convert_from_tab(Buffer, Config, ch))
			{
				return false;
			}
			if(!keys__printable_char(Buffer, ch))
			{
				return false;
			}
		}
		fclose(Textfile);
		SET_STATUS("read the file");
	}
	else
	{
		SET_STATUS("the file will be created");
	}
	return true;
}

void file__convert_to_tab(Buff_t* Buffer, Conf_t* Config, idx_t line_idx,
                          idx_t* char_idx)
{
	const idx_t at_least_one_tab = 1;

	for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value; tab_idx++)
	{
		if(Buffer->text[line_idx][*char_idx + tab_idx] != '\t')
		{
			break; // No tab, so don't convert anything.
		}
		else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
		{
			*char_idx += (idx_t) Config->Tab_width.value - at_least_one_tab;
		}
	}
}

void file__save(Buff_t* Buffer, Conf_t* Config)
{
	FILE* Textfile;
	int   fd;

	if(access(Buffer->fname, F_OK) == ERROR)
	{
		// There is no file so create with -rw------- mode.
		fd = open(Buffer->fname, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if(fd == ERROR)
		{
			SET_STATUS("failed to create the file");
		}
		close(fd);
	}
	Textfile = fopen(Buffer->fname, "w");

	if(Textfile != NULL)
	{
		for(idx_t line_idx = 0; line_idx <= Buffer->lines_idx; line_idx++)
		{
			/* Using fputs or fprintf causes use-of-uninitialized-value using
			MSan because of there is more memory allocated than is needed. */
			for(idx_t char_idx = 0;
			    char_idx < Buffer->lines_length_idx[line_idx]; char_idx++)
			{
				file__convert_to_tab(Buffer, Config, line_idx, &char_idx);
				putc(Buffer->text[line_idx][char_idx], Textfile);
			}
		}
		fclose(Textfile);
		SET_STATUS("saved");
	}
	else
	{
		SET_STATUS("can't write to the file");
	}
}

bool file__load_config(Conf_t* Config)
{
	struct passwd* Account_info;
	char           path[PATH_MAX]; // TODO: MAX SIZE.

	Account_info = getpwuid(getuid());
	if(Account_info == NULL)
	{
		config__set_default(Config);
	}
	strcpy(path, Account_info->pw_dir);
	strcat(path, "/.config/fiflorc");

	if(access(path, F_OK) == ERROR)
	{
		config__set_default(Config);
	}

	Config->File = fopen(path, "r");
	if(Config->File != NULL)
	{
		config__set_custom(Config);
		fclose(Config->File);
	}
	return true;
}

bool file__get_git_branch(Buff_t* Buffer) // TODO
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

	// strcpy(cwdir, Account_info->pw_dir);
	strcat(cwdir, git_head_path);

	// puts(cwdir);

	if(access(cwdir, F_OK) == ERROR)
	{
		strcpy(Buffer->git_branch, "[branch not found]");
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
	if(fseek(git_head_file, (long) strlen("ref: refs/heads/"), 0) == ERROR)
	{
		fprintf(stderr, "Can't locate the branch in the \"%s\" file\n.",
		        git_head_path);
		return false;
	}
	while(fgets(Buffer->git_branch, NAME_MAX + 1, git_head_file) != NULL) // TODO: MAX_LEN

	if(Buffer->git_branch[strlen(Buffer->git_branch) - 1] == '\n')
	{
		Buffer->git_branch[strlen(Buffer->git_branch) - 1] = '\0'; // LF removal.
	}
	fclose(git_head_file);
	return true;
}
