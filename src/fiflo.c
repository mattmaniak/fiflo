#ifdef __linux__
#include "fiflo.h" // All typedefs are there.

_Noreturn void free_all_exit(_Bool code, f_mtdt* Buff)
{
	for(buff_t line = 0; line <= Buff->lines; line++)
	{
		free(Buff->text[line]);
		Buff->text[line] = NULL;
	}
	free(Buff->text);
	Buff->text = NULL;

	free(Buff->line_len);
	Buff->line_len = NULL;

	free(Buff);
	Buff = NULL;

	exit(code);
}

void ignore_sig(int sig_num)
{
	if(sig_num) {}
}

void chk_ptr(void* ptr, const char* err_msg, f_mtdt* Buff)
{
	if(!ptr)
	{
		fprintf(stderr, "Can't %s, exit(1).\n", err_msg);
		free_all_exit(1, Buff);
	}
}

void options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
	{
		printf("%s\n\n%s\n%s\n%s\n%s\n%s\n%s\n",
		"Usage: fiflo [option].",

		"Options:      Description:",
		"<no_option>   Set a filename to \"/<current_path>/noname.asdf\".",
		"basename      Set a basename to \"basename\" using current path.",
		"/dir/bname    Set a filename to \"/dir/bname\"",
		"-h, --help    Show program help.",
		"-v, --version Display information about the used version.");
		exit(0);
	}
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.1.0 (WIP)",
		"https://gitlab.com/mattmaniak/fiflo.git",
		"(C) 2018 mattmaniak under the MIT License.");
		exit(0);
	}
}

char getch(void)
{
	char key;

	struct termios old;
	struct termios new;

	// Put the state of the STDIN_FILENO into *old.
	tcgetattr(STDIN_FILENO, &old);

	// Create the copy of the old terminal settings to modify it's.
	new = old;

	// Disable buffered I/O and echo mode.
	new.c_lflag &= ~(unsigned int) (ICANON | ECHO);

	/* Immediately set the state of the STDIN_FILENO to the *new. Use new
	terminal I/O settings. */
	tcsetattr(STDIN_FILENO, TCSANOW, &new);

	key = (char) getchar();

	// Immediately restore the state of STDIN_FILENO to *new.
	tcsetattr(STDIN_FILENO, TCSANOW, &old);

	return key;
}

f_mtdt* init(const char* arg, f_mtdt* Buff)
{
	Buff = set_fname(arg, Buff);

	Buff->text = malloc(sizeof(Buff->text));
	chk_ptr(Buff->text, "alloc memory for an array that contains lines\0", Buff);

	Buff->line_len = malloc(sizeof(Buff->line_len));
	chk_ptr(Buff->line_len, "alloc memory for an array with lines length\0", Buff);

	Buff->chars = 0;
	Buff->lines = 0;
	Buff->cusr_x = 0;

	ACT_LN_LEN = 0;
	ACT_LN = malloc(sizeof(Buff->text));

	return Buff;
}

_Noreturn void run(const char* arg)
{
	f_mtdt* Buff = malloc(sizeof(f_mtdt));
	chk_ptr(Buff, "alloc memory for a file metadata\0", Buff);

	Buff = init(arg, Buff);
	Buff = read_file(Buff);

	// Initializer. Equal to the null terminator.
	char pressed = 0;

	// Main program loop.
	for(;;)
	{
		Buff = recognize_key(pressed, Buff);
		window(Buff);

		pressed = getch();
		flush_window(Buff);
	}
}

int main(int argc, char** argv)
{
	// Catch CTRL^Z, CTRL^C and CTRL^\.
	if(signal(SIGTSTP, ignore_sig) == SIG_ERR
	|| signal(SIGINT, ignore_sig) == SIG_ERR
	|| signal(SIGQUIT, ignore_sig) == SIG_ERR)
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

