#ifdef __linux__

#include "fiflo.h" // All typedefs are here.

#include "handling.c"
#include "render.c"

void sighandler(int nothing)
{
	if(nothing) {}
}

void checkptr(buf* dt, void* ptr, const char* errmsg)
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
		"<NULL>        Set the filename to \"/<current_path>/noname.asdf\".",
		"basename      Open the textfile \"basename\" using your current path.",
		"/dir/bname    Open the textfile \"bname\" from the \"/dir\" folder.",
		"-h, --help    Show program help.",
		"-v, --version Display info about the current version.");
		exit(0);
	}
	else if(strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0)
	{
		printf("%s\n%s\n%s\n",
		"fiflo vX.Y.Z (WIP)",
		"https://gitlab.com/mattmaniak/fiflo.git",
		"(C) 2018 mattmaniak under the MIT License.");
		exit(0);
	}
	run(arg);
}

char nix_getch(void)
{
	struct termios old, new; // From sys/ioctl.h.
	char key;

	tcgetattr(STDIN_FILENO, &old); // Put the state of STDIN_FILENO into *old.
	new = old; // Create the copy of old terminal settings to modify it's.

	// Disable buffered I/O and echo mode.
	new.c_lflag &= (unsigned int) ~(ICANON | ECHO);
	// Immediately set the state of STDIN_FILENO to *new.
	tcsetattr(STDIN_FILENO, TCSANOW, &new); // Use new terminal I/O settings.

	key = (char) getchar();

	// Immediately restore the state of STDIN_FILENO to *new.
	tcsetattr(STDIN_FILENO, TCSANOW, &old);

	return key;
}

_Noreturn void run(const char* passed)
{
	buf* data = malloc(sizeof(buf));
	checkptr(data, data, "alloc memory for metadata\0");

	data->fname = malloc(PATH_MAX);
	data->txt = malloc(sizeof(data->txt) * MEMBLK);
	checkptr(data, data->txt, "alloc memory for lines\0");
	checkptr(data, data->fname, "alloc memory for the filename\0");

	data->chrs = 0;
	data->chrs_ln = 0;
	data->lns = 0;

	fnameset(data, passed);

	data->txt[0] = malloc(MEMBLK);
	checkptr(data, data->txt, "alloc memory for the first line\0");

	data = readfile(data);
	char pressed = NTERM; // Initializer.

	for(;;) // Main program loop.
	{
		data = recochar(data, pressed);
		window(data, pressed);
		pressed = nix_getch();
		flushwin(data);
	}
}

int main(int argc, char** argv)
{
	struct sigaction sig;

    sig.sa_handler = sighandler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
    sigaction(SIGINT, &sig, NULL);

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

