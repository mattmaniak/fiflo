#ifdef __linux__
#include "fiflo.h" // All typedefs are here.

//#include "keys.c"
//#include "api.c"
//#include "render.c"

_Noreturn void free_all_exit(meta* Dat, _Bool code)
{
	free(Dat->fname);
	Dat->fname = NULL;

	for(buf_t ln = 0; ln <= Dat->lns; ln++)
	{
		free(Dat->txt[ln]);
		Dat->txt[ln] = NULL;
	}
	free(Dat->ln_len);
	Dat->ln_len = NULL;

	free(Dat->txt);
	Dat->txt = NULL;

	free(Dat);
	Dat = NULL;

	exit(code);
}

void sig_handler(int nothing)
{
	if(nothing) {}
}

void check_ptr(meta* Dat, void* ptr, const char* err_msg)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exit(1).\n", err_msg);
		free_all_exit(Dat, 1);
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

meta* init(meta* Dat, const char* arg)
{
	Dat->fname = malloc(PATH_MAX);
	check_ptr(Dat, Dat->fname, "alloc memory for the filename\0");
	set_fname(Dat, arg);

	Dat->txt = malloc(sizeof(Dat->txt) * MEMBLK);
	check_ptr(Dat, Dat->txt, "alloc memory for lines\0");

	Dat->ln_len = malloc(MAX_CHRS + NTERM_SZ); // TODO: DYNAMIC.
	Dat->ln_len[0] = 0;
	Dat->chrs = 0;
	Dat->lns = 0;

	Dat->cusr_x = 0;
	Dat->cusr_y = 0;

	Dat->txt[0] = malloc(MEMBLK);
	check_ptr(Dat, Dat->txt, "alloc memory for the first line\0");

	return Dat;
}

_Noreturn void run(const char* arg)
{
	meta* Dat = malloc(sizeof(meta));
	check_ptr(Dat, Dat, "alloc memory for metadata\0");

	Dat = init(Dat, arg);
	Dat = read_file(Dat);
	char pressed = NTERM;

	// Main program loop.
	for(;;)
	{
		Dat = reco_key(Dat, pressed);
		window(Dat);
		pressed = getch();
		flush_win(Dat);
	}
}

int main(int argc, char** argv)
{
	if(argc != 1 && argc != 2)
	{
		fputs("Only one additional arg can be passed, exit(1).\n", stderr);
		exit(1);
	}
	struct sigaction sig;

    sig.sa_handler = sig_handler; // TODO: MACRO EXPANSION.
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0; // TODO: IS REQUIRED?
    sigaction(SIGINT, &sig, NULL);

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

