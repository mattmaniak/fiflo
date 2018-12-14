#ifdef __linux__

#include "ascii.h"
#include "buffer.h"
#include "memory.h"
#include "fiflo.h"

f_mtdt* init_buffer(f_mtdt* Buff, const char* arg)
{
	Buff->text = malloc(ADDR_SZ);
	chk_ptr(Buff, Buff->text, "malloc the array with lines\0");

	Buff->line_len_i = malloc(((sizeof(buff_t) / ADDR_SZ) * ADDR_SZ) + ADDR_SZ);
	chk_ptr(Buff, Buff->line_len_i, "malloc the array with lines length\0");

	Buff->chars_i  = 0;
	Buff->lines_i  = 0;
	Buff->cusr_x   = 0;
	Buff->cusr_y   = 0;
	ACT_LINE_LEN_I = 0;

	Buff->live_fname_edit = false;
	Buff->key_sequence    = false;
	Buff->pane_toggled    = false;

	Buff = set_fname(Buff, arg);

	ACT_LINE = malloc(ADDR_SZ);
	chk_ptr(Buff, ACT_LINE, "malloc the first line\0");

	return Buff;
}

void options(const char* arg)
{
	if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
	{
		printf("%s\n\n%s\n%s\n%s\n",
		"Usage: fiflo [basename/filename/option].",

		"Options:      Description:",
		"-h, --help    Show a program help.",
		"-v, --version Display information about a version You use.");
		exit(0);
	}
	else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
	{
		printf("%s\n%s\n%s\n",
		"fiflo v2.2.0 (WIP)",
		"(c) 2018 mattmaniak, MIT License",
		"https://gitlab.com/mattmaniak/fiflo.git");
		exit(0);
	}
}

char getch(f_mtdt* Buff)
{
	const int8_t error = -1;

	const unsigned int canonical_mode_on = ICANON;
	const unsigned int echo_input        = ECHO;
	const unsigned int enable_sigs       = ISIG;
	const unsigned int enable_xon        = IXON;

	struct termios old_term_params;
	struct termios new_term_params;

	char key;

	// Get the state of the STDIN_FILENO.
	if(tcgetattr(STDIN_FILENO, &old_term_params) == error)
	{
		flush_window(Buff);
		fputs("Can't get the stdin attribiutes. Pipe isn't supported.\n",
		stderr);
		free_buff_exit(Buff, 1);
	}
	new_term_params = old_term_params;

	// Look that the options of below flags are negated.
	new_term_params.c_iflag &= ~(enable_xon);
	new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

	/* Immediately set the state of the STDIN_FILENO to the *new_term_params.
	Use the new terminal I/O settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == error)
	{
		flush_window(Buff);
		fputs("Can't set the terminal's raw mode.\n", stderr);
		free_buff_exit(Buff, 1);
	}

	if((key = (char) getchar()) < 0)
	{
		flush_window(Buff);
		fputs("Negative char has been passed to the stdin.\n", stderr);
		free_buff_exit(Buff, 1);
	}

	// Immediately restore the state of the stdin (0) to the *new_term_params.
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == error)
	{
		flush_window(Buff);
		fputs("Can't restore the terminal to a normal mode.\n", stderr);
		free_buff_exit(Buff, 1);
	}
	return key;
}

_Noreturn void run(const char* arg)
{
	// Initializer.
	char pressed = '\0';

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

int main(const int argc, const char** argv)
{
	if(argc > 2)
	{
		fputs("Only one additional arg can be passed.\n", stderr);
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
