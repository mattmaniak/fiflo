#ifdef __linux__

#include "fiflo.h" // All typedefs are here.

#include "handling.c"
#include "render.c"

// TODO:
void ignore_sig(int nothing) // Arg for "‘__sighandler_t {aka void (*)(int)}".
{
	if(nothing == 0) {}
}

void run(char* passed)
{
	buff dt = {malloc(PATH_MAX), malloc(MEMBLOCK), 0, 0, 0};

	ptr_check(dt.fname, "Cannot allocate memory for the filename, exited.\0");
	set_fname(dt, passed);

	ptr_check(dt.txt, "malloc memory for the text\0");
	dt.txt[dt.lns] = malloc(MEMBLOCK);
	ptr_check(dt.txt, "malloc memory for first line\0");
	dt.txt[dt.lns][dt.chrs] = NTERM;

	dt = read_file(dt);
	char pressed_key = NTERM; // Initializer too.

	for(;;) // Main program loop.
	{
		dt = handle_key(dt, pressed_key);
		window(dt, pressed_key);

		pressed_key = nix_getch();
		flush_window(dt);
	}
}

void argc_check(int arg_count)
{
	if(arg_count > 2)
	{
		fputs("Fiflo can handle max. one additional arg, exited.\n", stderr);
		exit(1);
	}
}

int main(int argc, char** argv)
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

