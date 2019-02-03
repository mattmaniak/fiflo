#include "buffer.h"
#include "config.h"
#include "shortcuts.h"
#include "file.h"

bool file__set_name(Buff_t* Buffer, const char* arg)
{
	size_t arg_length  = strlen(arg);

	bool arg_non_empty   = arg_length > 0;
	bool arg_as_abs_path = arg[0]  == '/';
	bool arg_as_dir      = (arg[arg_length - NUL_SZ] == '/')
	                       && (arg[arg_length] == '\0');

	if(arg_non_empty && arg_as_dir)
	{
		fprintf(stderr, "Can't open the directory as a file.\n");
		return false;
	}

	if(arg_as_abs_path) // TODO
	{
		if(arg_length >= PATH_MAX)
		{
			fprintf(stderr, "Passed filename is too long.\n");
			return false;
		}
		strncpy(Buffer->fname, arg, PATH_MAX);
	}
	else // Relative path or the basename.
	{
		char* cwdir = malloc(PATH_MAX - NAME_MAX - SLASH_SZ);
		if(cwdir == NULL)
		{
			fprintf(stderr, "Can't alloc a memory for the directory.\n");
			return false;
		}

		cwdir = getcwd(cwdir, PATH_MAX - NAME_MAX - SLASH_SZ);
		if(cwdir == NULL)
		{
			fprintf(stderr, "Can't get the current directory. Too long.\n");
			return false;
		}

		if((strlen(cwdir) + arg_length) >= PATH_MAX)
		{
			fprintf(stderr, "Current directory is too long.\n"); // TODO.
			return false;
		}
		// Copy the path.
		strncpy(Buffer->fname, cwdir, PATH_MAX - NAME_MAX - SLASH_SZ);

		Buffer->fname[strlen(cwdir)] = '/'; // Add the slash between.

		// Append the basename.
		for(uint16_t char_idx = 0; char_idx < arg_length; char_idx++)
		{
			Buffer->fname[strlen(cwdir) + char_idx + SLASH_SZ] = arg[char_idx];
		}
		free(cwdir);
	}
	Buffer->fname_length = strlen(Buffer->fname);

	return true;
}

void file__live_edit_name(Buff_t* Buffer, Conf_t* Config, char key)
{
	const char enter  = '\n';
	const char escape = '\033';

	if((key >= 32) && (key != BACKSPACE)
	&& ((Buffer->fname_length + IDX) < PATH_MAX))
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
	else if(key == enter)
	{

		Buffer->live_fname_edit = false;
		file__save(Buffer, Config);

		SET_STATUS("filename edited and saved as");

		if(!strcmp(Buffer->fname, Buffer->orig_fname))
		{
			SET_STATUS("saved as");
		}
		strncpy(Buffer->orig_fname, Buffer->fname, PATH_MAX);
	}
	else if(key == escape)
	{
		strncpy(Buffer->fname, Buffer->orig_fname, PATH_MAX);
		Buffer->fname_length = strlen(Buffer->fname);

		Buffer->live_fname_edit = false;
		SET_STATUS("filename unchanged");
	}
}

bool file__convert_tab_from_file(Buff_t* Buffer, Conf_t* Config, char ch)
{
	/* Converts in-file '\t' in to sequence of e.g. "\t\t\t\t" if the tab width
	is set to 4. */
	if(ch == '\t')
	{
		for(idx_t char_idx = 0;
		    char_idx < (idx_t) (Config->Tab_width.value - AT_LEAST_ONE_TAB);
		    char_idx++)
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
	FILE* Textfile = fopen(Buffer->fname, "r");
	char  ch;

	if(Textfile != NULL)
	{
		if(Buffer->fname[Buffer->fname_length - NUL_SZ] == '/')
		{
			SET_STATUS("current directory set");
			fclose(Textfile);
			return true;
		}
		while((ch = (char) fgetc(Textfile)) != EOF)
		{
			if(!file__convert_tab_from_file(Buffer, Config, ch))
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

void file__convert_tab_to_file(Buff_t* Buffer, Conf_t* Config, idx_t line_idx,
                               idx_t* char_idx) // TODO: RETURN INSTEAD OF PTR.
{
	// Converts editor-friendly e.g. "\t\t\t\t" into the file-friendly '\t'.
	for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value; tab_idx++)
	{
		if(Buffer->text[line_idx][*char_idx + tab_idx] != '\t')
		{
			break; // No tab, so don't convert anything.
		}
		else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
		{
			// Some in-memory tabs converted
			*char_idx += (idx_t) Config->Tab_width.value - AT_LEAST_ONE_TAB;
		}
	}
}

void file__save(Buff_t* Buffer, Conf_t* Config)
{
	FILE* Textfile = fopen(Buffer->fname, "w");

	if(Textfile != NULL)
	{
		for(idx_t line_idx = 0; line_idx <= Buffer->lines_idx; line_idx++)
		{
			/* Using fputs or fprintf causes use-of-uninitialized-value using
			MSan because of there is more memory allocated than is needed. */
			for(idx_t char_idx = 0;
			    char_idx < Buffer->lines_length_idx[line_idx]; char_idx++)
			{
				file__convert_tab_to_file(Buffer, Config, line_idx, &char_idx);
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
	struct passwd Account_info; // 73 allocs here.
	char          path[PATH_MAX]; // TODO: MAX SIZE.

	Account_info = *getpwuid(getuid());

	strcpy(path, Account_info.pw_dir);
	strcat(path, "/.config/fiflorc");

	if(access(path, F_OK) == ERROR)
	{
		config__set_default(Config);
	}

	Config->File = fopen(path, "r");
	if(Config->File != NULL)
	{
		config__load_custom(Config);
		fclose(Config->File);
	}
	return true;
}

bool file__get_git_branch(Buff_t* Buffer) // TODO: MEMLEAKS.
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
		free(cwdir);
		return false;
	}

	// strcpy(cwdir, Account_info->pw_dir);
	strcat(cwdir, git_head_path);

	// puts(cwdir);

	if(access(cwdir, F_OK) == ERROR)
	{
		strcpy(Buffer->git_branch, "[branch not found]");
		free(cwdir);
		return true;
	}

	git_head_file = fopen(cwdir, "r");
	if(git_head_file == NULL)
	{
		fprintf(stderr, "Can't open the \"%s\" to get a Git branch.\n",
		        git_head_path);

		free(cwdir);
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
