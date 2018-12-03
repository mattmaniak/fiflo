#ifdef __linux__
#include "buffer.h"
#include "memory.h"
#include "fiflo.h"

f_mtdt* init_buffer(f_mtdt* Buff, const char* arg)
{
	Buff = set_fname(Buff, arg);

	Buff->text = malloc(ADDR_SZ);
	chk_ptr(Buff, Buff->text, "malloc the array with lines\0");

	Buff->line_len_i =
	malloc(((sizeof(Buff->line_len_i) / ADDR_SZ) * ADDR_SZ) + ADDR_SZ);

	chk_ptr(Buff, Buff->line_len_i, "malloc the array with lines length\0");

	Buff->chars_i         = 0;
	Buff->lines_i         = 0;
	Buff->cusr_x          = 0;
	Buff->cusr_y          = 0;
	ACT_LINE_LEN_I        = 0;
	Buff->live_fname_edit = false;

	ACT_LINE = malloc(ADDR_SZ);
	chk_ptr(Buff, ACT_LINE, "malloc the first line\0");

	return Buff;
}

void options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
	{
		printf("%s\n\n%s\n%s\n%s\n%s\n%s\n",
		"Usage: fiflo [option].",

		"Options:      Description:",
		"<no_option>   Don't set the filename.",
		"<name>        Specify the base/filename.",
		"-h, --help    Show program help.",
		"-v, --version Display information about a version You use.");
		exit(0);
	}
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.2.0 (WIP)",
		"https://gitlab.com/mattmaniak/fiflo.git",
		"(c) 2018 mattmaniak, MIT License.");
		exit(0);
	}
}

char getch(f_mtdt* Buff)
{
	const int8_t error = -1;

	const int echo_input_chars  = ECHO;
	const int enable_signals    = ISIG;
	const int canonical_mode_on = ICANON;
	const int enable_xon        = IXON;

	static struct termios old_term_settings;
	static struct termios new_term_settings;

	char key;

	// Put the state of the STDIN_FILENO into the *old_term_settings.
	if(tcgetattr(STDIN_FILENO, &old_term_settings) == error)
	{
		fputs("Can't get the terminal's termios attribiutes.", stderr);
		free_buff_exit(Buff, 1);
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
		fputs("Can't set the terminal state to it's raw mode.", stderr);
		free_buff_exit(Buff, 1);
	}

	key = (char) getchar();

	/* Immediately restore the state of the STDIN_FILENO to the
	*new_term_settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_settings) == error)
	{
		fputs("Can't restore the terminal state to it's normal mode.", stderr);
		free_buff_exit(Buff, 1);
	}
	return key;
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
		run("\0");
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
