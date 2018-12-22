#ifdef __linux__

#include "include/buffer.h"
#include "include/ascii.h"
#include "include/fiflo.h"

#include "include/file.h"

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
		"https://gitlab.com/mattmaniak/fiflo.git");
		exit(0);
	}
}

noreturn void run(const char* arg)
{
	char pressed = '\0'; // Initializer.

	f_mtdt* Buff = malloc(sizeof(f_mtdt));
	chk_ptr(Buff, Buff, "malloc the metadata buffer");

	Buff = buffer.init(Buff);
	Buff = file.set_name(Buff, arg);
	Buff = file.load(Buff);

	for(;;) // The main program loop.
	{
		Buff = parse_key(Buff, pressed);
		render_window(Buff);

		pressed = getch(Buff);
		flush_window(Buff);
	}
}

int main(const int argc, const char** argv)
{
	if(argc > 2)
	{
		fputs("Only one additional arg can be passed.\n", stderr);
		exit(1);
	}

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		fiflo.run("");
	}
	else
	{
		fiflo.options(argv[1]);
		fiflo.run(argv[1]);
	}
}

#else
#error "GNU/Linux is required."
#endif
