#ifdef __linux__

#include "fiflo.h" // All typedefs are here.

#include "handling.c"
#include "render.c"

// TODO:
void ignore_sig(int nothing) // Arg for "‘__sighandler_t {aka void (*)(int)}".
{
	if(nothing == 0) {}
}

void argc_check(int arg_count)
{
	if(arg_count != 1 && arg_count != 2)
	{
		fputs("Fiflo can handle max. one additional arg, exited.\n", stderr);
		exit(1);
	}
}

void options(const char* arg)
{
	if(strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
	{
		printf("%s\n\n%s\n%s\n%s\n%s\n%s\n%s\n",
		"Usage: fiflo [option].",

		"Options:      Description:",
		"<NULL>        Set the filename to \"/<current_path>/noname.asdf\"",
		"basename      Open the textfile \"basename\" using your current path.",
		"/dir/bname    Open the textfile \"bname\" from the \"/dir\" folder.",
		"-h, --help    Show program help.",
		"-v, --version Display some info about the current version.");
		exit(0);
	}
	else if(strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0)
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.0.0 (WIP)",
		"https://gitlab.com/mattmaniak/fiflo.git",
		"(C) 2018 mattmaniak under the MIT License.");
		exit(0);
	}
	run(arg);
}

void run(const char* passed)
{
	buff dt = {malloc(PATH_MAX), malloc(sizeof(dt.txt) * MEMBLK), 0, 0, 0};
	ptr_check(dt.fname, "Cannot allocate memory for the filename, exited.\0");
	ptr_check(dt.txt, "malloc memory for the text\0");

	set_fname(dt, passed);

	dt.txt[dt.lns] = malloc(MEMBLK);
	ptr_check(dt.txt, "malloc memory for first line\0");

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

int main(int argc, char** argv)
{
	signal(SIGTSTP, ignore_sig); // CTRL_Z
	signal(SIGINT, ignore_sig); // CTRL_C


	argc_check(argc);
	if(argv[1] == NULL)
	{
		run("noname.asdf\0");
	}
	else
	{
		options(argv[1]);
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

