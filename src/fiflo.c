#ifdef __linux__

#include "fiflo.h" // All typedefs are here.

#include "handling.c"
#include "render.c"

void run(char* name)
{
	buff data = {NULL, NULL, 0, 0}; // Just empty init for -Wuninitialized.
	data.filename = malloc(MAX_PATH + MAX_NAME + 1); // >~ 4 KiB.
	ptr_check(data.filename, "Cannot alloc filename in memory, exited.\n\0");
	set_filename(data, name);

	data = read_file(data);
	char pressed_key = TERMINATOR; // Initializer too.

	for(;;) // Main program loop.
	{
		data = alloc_text(data, pressed_key);
		limits(data);

		window(data, pressed_key);
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

int main(int argc, char* argv[])
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

