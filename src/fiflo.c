#ifdef __linux__

#include "fiflo.h"

#include "handling.c"
#include "render.c"

void run(char *name)
{
	buff data = read_file(data, name);
	char pressed_key = TERMINATOR;
	term_t y;

	for(;;)
	{
		data = window(data, pressed_key);
		for(y = 0; y <= get_term_sz(Y) - BAR_SZ - data.lines; y++)
		{
//			printf("%s", CURSOR_UP);
		}
		pressed_key = nix_getch();
		flush_window(data.lines);
	}
}

void ignore_sig(int nothing) // Arg for "‘__sighandler_t {aka void (*)(int)}".
{
	if(nothing == 0) {}
}

void argc_check(int arg_count)
{
	if(arg_count > 2)
	{
		fputs("Fiflo can handle max. one additional arg, exited.\n", stderr);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGTSTP, ignore_sig); // CTRL_Z
	signal(SIGINT, ignore_sig); // CTRL_C

	argc_check(argc);
	if(argv[1] == NULL)
	{
		run("noname.asdf\0");
	}
	else if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		help();
		exit(0);
	}
	else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
	{
		version();
		exit(0);
	}
	else
	{
		run(argv[1]);
	}
	return 0;
}

#else

#include <stdio.h>

int main(void)
{
	fputs("Only Linux-based systems are supported, exited.\n", stderr);
	return 0;
}

#endif

