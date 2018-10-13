#ifdef __linux__
#include "fiflo.h" // All typedefs are here.

_Noreturn void freeallexit(_Bool code, meta* Dt)
{
	free(Dt->fname);
	Dt->fname = NULL;

	for(buf_t ln = 0; ln <= Dt->lns; ln++)
	{
		free(Dt->txt[ln]);
		Dt->txt[ln] = NULL;
	}
	free(Dt->ln_len);
	Dt->ln_len = NULL;

	free(Dt->txt);
	Dt->txt = NULL;

	free(Dt);
	Dt = NULL;

	exit(code);
}

void handlesig(int nothing)
{
	if(nothing) {}
}

void ptrcheck(void* ptr, const char* err_msg, meta* Dt)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exit(1).\n", err_msg);
		freeallexit(1, Dt);
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
		"-v, --version Display information about the version.");
		exit(0);
	}
	else if(strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0)
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.1.0 (WIP)",
		"https://gitlab.com/mattmaniak/fiflo.git",
		"(C) 2018 mattmaniak under the MIT License.");
		exit(0);
	}
}

char getch(void) // TODO: COMMENT.
{
	struct termios old, new;
	char key;

	// Put the state of STDIN_FILENO into *old.
	tcgetattr(STDIN_FILENO, &old);

	// Create the copy of old terminal settings to modify it's.
	new = old;

	// Disable buffered I/O and echo mode.
	new.c_lflag &= (unsigned int) ~(ICANON | ECHO);

	// Immediately set the state of STDIN_FILENO to *new.
	// Use new terminal I/O settings.
	tcsetattr(STDIN_FILENO, TCSANOW, &new);

	key = (char) getchar();

	// Immediately restore the state of STDIN_FILENO to *new.
	tcsetattr(STDIN_FILENO, TCSANOW, &old);

	return key;
}

meta* init(meta* Dt, const char* arg)
{
	Dt->fname = malloc(PATH_MAX);
	ptrcheck(Dt->fname, "alloc memory for the filename\0", Dt);
	fnameset(Dt, arg);

	Dt->txt = malloc(sizeof(Dt->txt) * MEMBLK);
	ptrcheck(Dt->txt, "alloc memory for lines\0", Dt);

	Dt->ln_len = malloc(MAX_CHRS + NTERM_SZ); // TODO: DYNAMIC.
	Dt->ln_len[0] = 0;
	Dt->chrs = 0;
	Dt->lns = 0;

	Dt->cusr_x = 0;
	Dt->cusr_y = 0;

	Dt->txt[0] = malloc(MEMBLK);
	ptrcheck(Dt->txt, "alloc memory for the first line\0", Dt);

	return Dt;
}

_Noreturn void run(const char* arg)
{
	meta* Dt = malloc(sizeof(meta));
	ptrcheck(Dt, "alloc memory for metadata\0", Dt);


	Dt = init(Dt, arg);
	Dt = readfile(Dt);
	char pressed = NTERM;

	// Main program loop.
	for(;;)
	{
		Dt = keymap(Dt, pressed);
		window(Dt);
		pressed = getch();
		flushwin(Dt);
	}
}

int main(int argc, char** argv)
{
	// Catch CTRL+C.
	signal(SIGINT, handlesig);
	// Catch CTRL+Z.
	signal(SIGTSTP, handlesig);

	if(argc != 1 && argc != 2)
	{
		fputs("Only one additional arg can be passed, exit(1).\n", stderr);
		exit(1);
	}

	if(argv[1] == NULL)
	{
		run("noname.asdf\0");
	}
	else
	{
		options(argv[1]);
		run(argv[1]);
	}
}

#else
#error "Only Linux-based systems are supported."
#endif

