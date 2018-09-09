#ifdef __linux__

#include "fiflo.h" // All typedefs are here.

#include "handling.c"
#include "render.c"

void sigignore(int nothing) // Arg for "‘__sighandler_t {aka void (*)(int)}".
{
	if(nothing == 0) {}
}

void checkptr(buf dt, void* ptr, const char* errmsg)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exited.\n", errmsg);
		freeallexit(dt, 1);
	}
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

char nix_getch(void)
{
	struct termios old, new;
	char key;

	tcgetattr(STDIN_FILENO, &old); // Put the state of STDIN_FILENO into *old.
	new = old;
	new.c_lflag &= ~(ICANON | ECHO); // Disable buffered I/O and echo mode.
	
	tcsetattr(STDIN_FILENO, TCSANOW, &new); // Use new terminal I/O settings.
	key = (char) getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &old); // Restore terminal settings.

	return key;
}

_Noreturn void run(const char* passed)
{
	buf dt = {malloc(PATH_MAX), malloc(sizeof(dt.txt) * MEMBLK), 0, 0, 0, 0};
	checkptr(dt, dt.fname, "alloc memory for the filename\0");
	checkptr(dt, dt.txt, "alloc memory for lines\0");

	fnameset(dt, passed);

	dt.txt[dt.lns] = malloc(MEMBLK);
	checkptr(dt, dt.txt, "alloc memory for chars in the first line\0");

	dt = readfile(dt);
	char pressed = NTERM; // Initializer too.

	for(;;) // Main program loop.
	{
		signal(SIGTSTP, sigignore); // CTRL_Z
		signal(SIGINT, sigignore); // CTRL_C

		dt = recochar(dt, pressed);
		window(dt, pressed);
		pressed = nix_getch();
		flushwin();
	}
}

int main(int argc, char** argv)
{
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

