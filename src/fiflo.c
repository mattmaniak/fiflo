#ifdef __linux__

#include "buffer.h"
#include "ascii.h"
#include "fiflo.h"

#include "file.h"
#include "input.h"
#include "window.h"

void options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
	{
		printf("%s\n\n%s\n%s\n%s\n",
		"Usage: fiflo [basename/filename/option].",

		"Options:      Description:",
		"-h, --help    Show a program help.",
		"-v, --version Display information about the version You use.");
		exit(0);
	}
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.3.0 (WIP)",
		"(C) 2018 mattmaniak, MIT License",
		"https://gitlab.com/mattmaniak/git");
		exit(0);
	}
}

noreturn void run(const char* arg)
{
	char pressed = '\0'; // Initializer.

	Buff_t Buff;
	Buff = *buffer.init(&Buff);
	Buff = *file.set_name(&Buff, arg);
	Buff = *file.load(&Buff);

	for(;;) // The main program loop.
	{
		Buff = *input.parse_key(&Buff, pressed);
		window.display(&Buff);

		pressed = input.getch(&Buff);
		window.flush(&Buff);
	}
}

int main(const int argc, const char** argv)
{
	if(argc > 2)
	{
		fputs("Only one additional arg can be passed.\n", stderr);
		return 1;
	}

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		run("");
	}
	else if(strlen(argv[1]) <= PATH_MAX)
	{
		options(argv[1]);
		run(argv[1]);
	}
	else
	{
		fprintf(stderr, "Maximum length of a parameter is %u\n", PATH_MAX);
		return 1;
	}
}

#else
#error "GNU/Linux is required."
#endif
