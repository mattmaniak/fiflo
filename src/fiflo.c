#ifdef __linux__

#include "buffer.h"
#include "ascii.h"
#include "fiflo.h"

#include "file.h"
#include "input.h"
#include "window.h"

int options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
	{
		printf("%s\n\n%s\n%s\n%s\n",
		"Usage: fiflo [basename/filename/option].",

		"Options:      Description:",
		"-h, --help    Show a program help.",
		"-v, --version Display information about this version.");
		return 0;
	}
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n%s\n%s\n",
		"|``",
		"|``",
		"fiflo v3.0.0 (WIP)",
		"(C) 2018-2019 mattmaniak, MIT License",
		"https://gitlab.com/mattmaniak/fiflo.git");
		return 0;
	}
	return run(arg);
}

int run(const char* arg)
{
	char   pressed = '\0'; // Initializer.
	Buff_t Buff;

	if(buffer.init(&Buff) == -1)
	{
		return -1;
	}
	if(file.set_name(&Buff, arg) == -1)
	{
		return -1;
	}
	if(file.load(&Buff) == -1)
	{
		return -1;
	}

	for(;;) // The main program loop.
	{
		if(file.get_git_branch(&Buff))
		{

		}
		if(input.parse_key(&Buff, pressed))
		{
			// buffer.free_all(&Buff);
			// return false;
		}
		window.render(&Buff);

		if((pressed = input.getch()) == -1)
		{
			return false;
		}
		window.flush();
	}
}

int main(const int argc, const char** argv)
{
	if(argc > 2)
	{
		fputs("Only one additional arg can be passed.\n", stderr);
		return -1;
	}

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		return run("");
	}
	else if(strlen(argv[1]) <= _POSIX_ARG_MAX)
	{
		return options(argv[1]);
	}
	else
	{
		fprintf(stderr, "Maximum length of a parameter is %u\n", PATH_MAX);
		return -1;
	}
	return 0;
}

#else
#error "GNU/Linux is required."
#endif
