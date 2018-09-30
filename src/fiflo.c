#ifdef __linux__

#include "fiflo.h" // All typedefs are here.
#include "files.c"
#include "api.c"
#include "render.c"

_Noreturn void free_all_exit(meta* dt, _Bool code)
{
	free(dt->fname);
	for(buf_t ln = 0; ln <= dt->lns; ln++)
	{
		free(dt->txt[ln]);
	}
	free(dt->txt);
	free(dt);
	exit(code);
}

void sig_handler(int nothing)
{
	if(nothing) {}
}

void check_ptr(meta* dt, void* ptr, const char* err_msg)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exit(1).\n", err_msg);
		free_all_exit(dt, 1);
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

meta* init(meta* dt, const char* passed)
{
	dt->fname = malloc(PATH_MAX);
	dt->txt = malloc(sizeof(dt->txt) * MEMBLK);
	check_ptr(dt, dt->txt, "alloc memory for lines\0");
	check_ptr(dt, dt->fname, "alloc memory for the filename\0");

	dt->chrs = 0;
	dt->chrs_ln = 0;
	dt->lns = 0;

	dt->cusr_x = 0;
	dt->cusr_y = 0;

	set_fname(dt, passed);
	dt->txt[0] = malloc(MEMBLK);
	check_ptr(dt, dt->txt, "alloc memory for the first line\0");

	return dt;
}

_Noreturn void run(const char* passed)
{
	meta* data = malloc(sizeof(meta));
	check_ptr(data, data, "alloc memory for metadata\0");
	data = init(data, passed);
	data = read_file(data);

	char pressed = NTERM; // Initializer.
	// Main program loop.
	for(;;)
	{
		data = recognize_char(data, pressed);
		window(data, pressed);
		pressed = getch();
		flush_win(data);
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

    sig.sa_handler = sig_handler;
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
	}
	return 0;
}

#else
#error "Only Linux-based systems are supported."
#endif

