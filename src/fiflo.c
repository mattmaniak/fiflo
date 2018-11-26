#ifdef __linux__
#include "fiflo.h"

_Noreturn void free_all_exit(f_mtdt* Buff, _Bool code)
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

void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg)
{
	if(ptr == NULL)
	{
		fprintf(stderr, "Can't %s, exit(1).\n", err_msg);
		free_all_exit(Buff, 1);
	}
}

void options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
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
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.2.0 (WIP)",
		"https://gitlab.com/mattmaniak/fiflo.git",
		"(C) 2018 mattmaniak under the MIT License.");
		exit(0);
	}
}

char getch(void)
{
	int echo_input_chars  = ECHO;
	int enable_signals    = ISIG;
	int canonical_mode_on = ICANON;
	int enable_xon        = IXON;

	struct termios old_term_settings;
	struct termios new_term_settings;

	char key;

	// Put the state of the STDIN_FILENO into the *old_term_settings.
	tcgetattr(STDIN_FILENO, &old_term_settings);

	// Create the copy of the old terminal settings to modify it's.
	new_term_settings = old_term_settings;

	// Look that the options of below flags are negated.
	new_term_settings.c_lflag &= (unsigned int)
	~(canonical_mode_on | echo_input_chars | enable_signals);

	new_term_settings.c_iflag &= (unsigned int) ~(enable_xon);

	/* Immediately set the state of the STDIN_FILENO to the *new_term_settings.
	Use the new terminal I/O settings. */
	tcsetattr(STDIN_FILENO, TCSANOW, &new_term_settings);

	key = (char) getchar();

	/* Immediately restore the state of the STDIN_FILENO to the
	*new_term_settings. */
	tcsetattr(STDIN_FILENO, TCSANOW, &old_term_settings);

	return key;
}

f_mtdt* init_buffer(f_mtdt* Buff, const char* arg)
{
	Buff = set_fname(Buff, arg);

	Buff->text = malloc(sizeof(Buff->text));
	chk_ptr(Buff, Buff->text, "malloc the array with lines\0");

	Buff->line_len = malloc(sizeof(Buff->line_len));
	chk_ptr(Buff, Buff->line_len, "malloc the array with lines length\0");

	Buff->chars  = 0;
	Buff->lines  = 0;
	Buff->cusr_x = 0;
	Buff->cusr_y = 0;

	ACT_LN_LEN = 0;
	ACT_LN = malloc(sizeof(Buff->text));

	return Buff;
}

_Noreturn void run(const char* arg)
{
	// Initializer. Equal to the null terminator.
	char pressed = 0;

	f_mtdt* Buff = malloc(sizeof(f_mtdt));
	chk_ptr(Buff, Buff, "malloc the file metadata\0");

	Buff = init_buffer(Buff, arg);
	Buff = read_file(Buff);

	// The main program loop.
	for(;;)
	{
		Buff = recognize_key(Buff, pressed);
		render_window(Buff);

		pressed = getch();
		flush_window(Buff);
	}
}

int main(int argc, char** argv)
{
	if((argc != 1) && (argc != 2))
	{
		fputs("Only one additional arg can be passed, exit(1).\n", stderr);
		exit(1);
	}

	// Sets the default basename to "noname.asdf".
	if(argv[1] == NULL)
	{
		run("noname.flop\0");
	}
	else
	{
		options(argv[1]);
		run(argv[1]);
	}
}

#else
#error "GNU/Linux is required."
#endif
