#ifdef __linux__

#include "include/buffer.h"
#include "include/ascii.h"
#include "include/fiflo.h"

f_mtdt* init_buffer(f_mtdt* Buff, const char* arg)
{
	Buff->text = malloc(ADDR_SZ);

	if(Buff->text == NULL)
	{
		fputs("Can't malloc the array with lines.", stderr);
		exit(1);
	}
	Buff->line_len_i = malloc(((sizeof(buff_t) / ADDR_SZ) * ADDR_SZ) + ADDR_SZ);

	if(Buff->line_len_i == NULL)
	{
		fputs("Can't malloc the array with lines length.", stderr);
		free(Buff->text);
		exit(1);
	}
	Buff->chars_i = 0;
	Buff->lines_i = 0;
	Buff->cusr_x = 0;
	Buff->cusr_y = 0;
	ACT_LINE_LEN_I = 0;

	Buff->live_fname_edit = false;
	Buff->key_sequence = false;
	Buff->pane_toggled = false;

	ACT_LINE = malloc(ADDR_SZ);

	if(ACT_LINE == NULL)
	{
		fputs("Can't malloc the array with lines length.", stderr);
		free(Buff->text);
		free(Buff->line_len_i);
		exit(1);
	}
	Buff = set_fname(Buff, arg);

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
		"fiflo v2.2.2",
		"(C) 2018 mattmaniak, MIT License",
		"https://gitlab.com/mattmaniak/fiflo.git");
		exit(0);
	}
}

noreturn void run(const char* arg)
{
	char pressed = '\0'; // Initializer.

	f_mtdt* Buff = malloc(sizeof(f_mtdt));
	chk_ptr(Buff, Buff, "malloc the metadata buffer");

	Buff = init_buffer(Buff, arg);
	Buff = read_file(Buff);

	for(;;) // The main program loop.
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

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		run("");
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
