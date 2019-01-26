#ifdef __linux__

#include "buffer.h"
#include "config.h"
#include "fiflo.h"

bool fiflo__options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
	{
		printf("%s\n\n%s\n%s\n%s\n",
		"Usage: fiflo [basename/filename/option].",

		"Options:      Description:",
		"-h, --help    Show a program help.",
		"-v, --version Display information about this version.");
		return false;
	}
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n%s\n%s\n",
		"|``",
		"|``",
		"fiflo v3.0.0 (WIP)",
		"(C) 2018-2019 mattmaniak, MIT License",
		"https://gitlab.com/mattmaniak/fiflo.git");
		return false;
	}
	return true;
}

void fiflo__run(const char* arg)
{
	char   pressed_key = '\0'; // Initializer.
	Buff_t Buffer;
	Conf_t Config;

	if(!buffer__init(&Buffer))
	{
		goto free;
	}
	if(!file__load_editor_config(&Config))
	{
		goto free;
	}
	if(!file__set_name(&Buffer, arg))
	{
		goto free;
	}
	if(!file__load(&Buffer, &Config))
	{
		goto free;
	}

	for(;;) // The main program loop.
	{
		if(!file__get_git_branch(&Buffer))
		{
			break;
		}
		if(!input_parse_key(&Buffer, &Config, pressed_key))
		{
			break;
		}
		if(!window__render(&Buffer, &Config))
		{
			break;
		}
		if((pressed_key = input_getch()) == ERROR)
		{
			break;
		}
		window__flush();
	}
	free:
	buffer__free(&Buffer);
}

int main(int argc, char** argv)
{
	if(argc > 2)
	{
		fprintf(stderr, "Only one additional arg can be passed.\n");
		goto exit;
	}

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		fiflo__run("");
		goto exit;
	}
	else if(strlen(argv[1]) <= PATH_MAX)
	{
		if(!fiflo__options(argv[1]))
		{
			goto exit;
		}
		fiflo__run(argv[1]);
		goto exit;
	}
	else
	{
		fprintf(stderr, "Maximum length of a parameter is %u\n", PATH_MAX);
		goto exit;
	}
	exit:
	fflush(NULL);
	return 0;
}

#else
#error "GNU/Linux is required."
#endif
