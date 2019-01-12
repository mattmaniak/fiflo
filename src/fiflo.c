#ifdef __linux__

#include "buffer.h"
#include "fiflo.h"

#include "file.h"
#include "input.h"
#include "window.h"

bool options(const char* arg)
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

void run(const char* arg)
{
	char   pressed_key = '\0'; // Initializer.
	Buff_t Buff;

	if(!buffer.init(&Buff))
	{
		goto free;
	}
	if(!file.set_name(&Buff, arg))
	{
		goto free;
	}
	if(!file.load(&Buff))
	{
		goto free;
	}

	for(;;) // The main program loop.
	{
		if(!file.get_git_branch(&Buff))
		{
			break;
		}
		if(!input.parse_key(&Buff, pressed_key))
		{
			break;
		}
		if(!window.render(&Buff))
		{
			break;
		}
		if((pressed_key = input.getch()) == -1)
		{
			break;
		}
		window.flush();
	}
	free:
	buffer.free_all(&Buff); // TODO: FREE.
}

int main(const int argc, const char** argv)
{
	if(argc > 2)
	{
		fprintf(stderr, "Only one additional arg can be passed.\n");
		goto exit;
	}

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		run("");
		goto exit;
	}
	else if(strlen(argv[1]) <= _POSIX_ARG_MAX)
	{
		if(!options(argv[1]))
		{
			goto exit;
		}
		run(argv[1]);
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
