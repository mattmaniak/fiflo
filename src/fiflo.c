#ifdef __linux__
#include "fiflo.h" // All typedefs are here.

_Noreturn void free_all_exit(_Bool code, meta* Dt)
{
	free(Dt->fname);
	Dt->fname = NULL;

	for(buf_t line = 0; line <= Dt->lines; line++)
	{
		free(Dt->text[line]);
		Dt->text[line] = NULL;
	}
	free(Dt->text);
	Dt->text = NULL;

	free(Dt->line_len);
	Dt->line_len = NULL;

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
		"<no_option>   Set the filename to \"/<current_path>/noname.asdf\".",
		"basename      Set the basename to \"basename\" using current path.",
		"/dir/bname    Set the filename to \"/dir/bname\"",
		"-h, --help    Show program help.",
		"-v, --version Display information about the used version.");
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

	Dt->text = malloc(sizeof(Dt->text));
	chk_ptr(Dt->text, "alloc memory for an array that contains lines\0", Dt);

	Dt->line_len = malloc(sizeof(Dt->line_len));
	chk_ptr(Dt->line_len, "alloc memory for an array with lines length\0", Dt);

	Dt->chars = 0;
	Dt->lines = 0;
	Dt->cusr_x = 0;

	ACT_LN_LEN = 0;
	ACT_LN = malloc(INIT_MEMBLK);

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
	if(signal(SIGINT,  ignore_sig) == SIG_ERR
	|| signal(SIGTSTP, ignore_sig) == SIG_ERR)
	{
		fputs("Can't catch one of the signals, exit(1)\n", stderr);
		exit(1);
	}

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

