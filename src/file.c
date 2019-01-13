#include "buffer.h"
#include "config.h"
#include "ascii.h"
#include "file.h"

bool file_set_name(Buff_t* Buff, const char* arg)
{
	const size_t nul_sz   = 1;
	const size_t slash_sz = 1;
	size_t     arg_len  = strlen(arg);

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
		char* cw_dir = malloc(PATH_MAX - NAME_MAX - slash_sz);
		if(cw_dir == NULL)
		{
			fputs("Can't alloc a memory for the directory.\n", stderr);
			return false;
		}

		cw_dir = getcwd(cw_dir, PATH_MAX - NAME_MAX - slash_sz);
		if(cw_dir == NULL)
		{
			fputs("Can't get the current directory. Maybe too long.\n", stderr);
			return false;
		}

		if((strlen(cw_dir) + arg_len) >= PATH_MAX)
		{
			fputs("Current directory is too long.\n", stderr); // TODO.
			return false;
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
	Buff->fname_len_i = strlen(Buff->fname);

	return true;
}

void file_live_edit_name(Buff_t* Buff, char key)
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

bool file_load(Buff_t* Buff)
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
			if(!keys_printable_char(Buff, ch))
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

void file_save(Buff_t* Buff)
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

bool file_load_editor_config(Conf_t* Conf)
{
	struct passwd* account_info = getpwuid(getuid());
	char           path[PATH_MAX]; // TODO: MAX SIZE.

	if(account_info == NULL)
	{
		config_set_default(Conf);
	}

	strcpy(path, account_info->pw_dir);
	strcat(path, "/.config/fiflo.conf");

	if(access(path, F_OK) == -1)
	{
		config_set_default(Conf);
	}

	Conf->file = fopen(path, "r");
	if(Conf->file != NULL)
	{
		config_set_custom(Conf);
		fclose(Conf->file);
	}
	return true;
}

bool file_get_git_branch(Buff_t* Buff) // TODO
{
/* fatal: not a git repository (or any parent up to mount point /)
Stopping at filesystem boundary (GIT_DISCOVERY_ACROSS_FILESYSTEM not set). */

	FILE* pipe;

	if(access("/usr/bin/git", F_OK) == -1)
	{
		strncpy(Buff->git_branch, "VCS not installed", 256);
		return true;
	}

	pipe = popen("/usr/bin/git rev-parse --abbrev-ref HEAD", "r");
	if(pipe == NULL)
	{
		strncpy(Buff->git_branch, "can't get a branch\n", 256);
		return true;
	}

	while(fgets(Buff->git_branch, 256 - 1, pipe) != NULL) // TODO: MAX_LEN

	if(!strncmp(Buff->git_branch, "fatal: not a git repository", 23)) // TODO
	{
		strncpy(Buff->git_branch, "repo not found", 256);
	}
	if(pclose(pipe) == -1)
	{
		fputs("Can't...", stderr); // TODO
		return false;
	}

	if(Buff->git_branch[strlen(Buff->git_branch) - 1] == '\n')
	{
		Buff->git_branch[strlen(Buff->git_branch) - 1] = '\0';
	}
	return true;
}
