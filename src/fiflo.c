#ifdef __linux__
#include "fiflo.h" // All typedefs are here.

_Noreturn void free_all_exit(_Bool code, meta* Dt)
{
	free(Dt->fname);
	Dt->fname = NULL;

	for(buf_t ln = 0; ln <= Dt->lns; ln++)
	{
		free(Dt->txt[ln]);
		Dt->txt[ln] = NULL;
	}
	free(Dt->txt);
	Dt->txt = NULL;

	free(Dt->ln_len);
	Dt->ln_len = NULL;

	free(Dt);
	Dt = NULL;

	exit(code);
}

void ignore_sig(int nothing)
{
	if(nothing) {}
}

void chk_ptr(void* ptr, const char* err_msg, meta* Dt)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exit(1).\n", err_msg);
		free_all_exit(1, Dt);
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

meta* init(const char* arg, meta* Dt)
{
	Dt = set_fname(arg, Dt);

	Dt->txt = malloc(sizeof(Dt->txt) * MEMBLK);
	chk_ptr(Dt->txt, "alloc memory for an array that contains lines\0", Dt);

	Dt->ln_len = malloc(MAX_LNS);
	chk_ptr(Dt->ln_len, "alloc memory for an array with lines length\0", Dt);

	Dt->chrs = 0;
	Dt->lns = 0;
	Dt->cusr_x = 0;

	CURR_LN_LEN = 0;
	CURR_LN = malloc(INIT_MEMBLK);
	return Dt;
}

_Noreturn void run(const char* arg)
{
	meta* Dt = malloc(sizeof(meta));
	chk_ptr(Dt, "alloc memory for a file metadata\0", Dt);

	Dt = init(arg, Dt);
	Dt = read_file(Dt);

	// Initializer. Equal to the null terminator.
	char pressed = 0;

	// Main program loop.
	for(;;)
	{
		Dt = recognize_key(pressed, Dt);
		window(Dt);
		pressed = getch();
		flush_window(Dt);
	}
}

int main(int argc, char** argv)
{
	// Catch CTRL+C and CTRL+Z interrupts.
	signal(SIGINT, ignore_sig);
	signal(SIGTSTP, ignore_sig);

	if(argc != 1 && argc != 2)
	{
		fputs("Only one additional arg can be passed, exit(1).\n", stderr);
		exit(1);
	}

	// Sets the default basename to "noname.asdf".
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
#error "Linux-based desktop is required."
#endif

