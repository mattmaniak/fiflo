#ifdef __linux__
#include "fiflo.h"

_Noreturn void free_all_exit(f_mtdt* Buff, const bool code)
{
	for(buff_t line_i = 0; line_i <= Buff->lines_i; line_i++)
	{
		free(Buff->text[line_i]);
		Buff->text[line_i] = NULL;
	}
	free(Buff->text);
	Buff->text = NULL;

	free(Buff->line_len_i);
	Buff->line_len_i = NULL;

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
		"<no_option>   Set the filename to \"/<current_path>/noname.flop\".",
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

char getch(f_mtdt* Buff)
{
	const int8_t error = -1;

	int echo_input_chars  = ECHO;
	int enable_signals    = ISIG;
	int canonical_mode_on = ICANON;
	int enable_xon        = IXON;

	struct termios old_term_settings;
	struct termios new_term_settings;

	char key;

	// Put the state of the STDIN_FILENO into the *old_term_settings.
	if(tcgetattr(STDIN_FILENO, &old_term_settings) == error)
	{
		fputs("Can't get the terminal termios attribiutes.", stderr);
		free_all_exit(Buff, 1);
	}

	// Create the copy of the old terminal settings to modify it's.
	new_term_settings = old_term_settings;

	// Look that the options of below flags are negated.
	new_term_settings.c_lflag &= (unsigned int)
	~(canonical_mode_on | echo_input_chars | enable_signals);

	new_term_settings.c_iflag &= (unsigned int) ~(enable_xon);

	/* Immediately set the state of the STDIN_FILENO to the *new_term_settings.
	Use the new terminal I/O settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_settings) == error)
	{
		fputs("Can't set the terminal state to the raw mode.", stderr);
		free_all_exit(Buff, 1);
	}

	key = (char) getchar();

	/* Immediately restore the state of the STDIN_FILENO to the
	*new_term_settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_settings) == error)
	{
		fputs("Can't restore the terminal state to the normal mode.", stderr);
		free_all_exit(Buff, 1);
	}
	return key;
}

f_mtdt* init_buffer(f_mtdt* Buff, const char* arg)
{
	Buff = set_fname(Buff, arg);

	Buff->text = malloc(sizeof(Buff->text));
	chk_ptr(Buff, Buff->text, "malloc the array with lines\0");

	Buff->line_len_i = malloc(sizeof(Buff->line_len_i));
	chk_ptr(Buff, Buff->line_len_i, "malloc the array with lines length\0");

	Buff->chars_i = 0;
	Buff->lines_i = 0;
	Buff->cusr_x  = 0;
	Buff->cusr_y  = 0;

	ACT_LINE_LEN_I = 0;
	ACT_LINE = malloc(sizeof(Buff->text));

	return Buff;
}

_Noreturn void run(const char* arg)
{
	// Initializer. Equal to the null terminator.
	char pressed = 0;

	f_mtdt* Buff = malloc(sizeof(f_mtdt));
	chk_ptr(Buff, Buff, "malloc the metadata buffer\0");

	Buff = init_buffer(Buff, arg);
	Buff = read_file(Buff);

	// The main program loop.
	for(;;)
	{
		Buff = parse_key(Buff, pressed);
		render_window(Buff);

		pressed = getch(Buff);
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

	// Sets the default basename and starts.
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
