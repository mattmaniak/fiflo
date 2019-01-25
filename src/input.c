#include "ascii.h"
#include "buffer.h"
#include "input.h"

char input_getch(void)
{
	const unsigned int canonical_mode_on = ICANON;
	const unsigned int echo_input        = ECHO;
	const unsigned int enable_sigs       = ISIG;
	const unsigned int enable_xon        = IXON;

	struct termios old_term_params;
	struct termios new_term_params;

	char key;

	// Get the state of the STDIN_FILENO.
	if(tcgetattr(STDIN_FILENO, &old_term_params) == ERROR)
	{
		window__flush();
		fprintf(stderr, "Stdin params error. Pipe isn't supported.\n");
		return ERROR;
	}
	new_term_params = old_term_params;

	// Look that the options of below flags are negated.
	new_term_params.c_iflag &= ~(enable_xon);
	new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

	/* Immediately set the state of the STDIN_FILENO to the* new_term_params.
	Use the new terminal I/O settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == ERROR)
	{
		window__flush();
		fprintf(stderr, "Can't set the terminal's raw mode.\n");
		return ERROR;
	}

	if((key = (char) getchar()) < 0) // TODO: RESTORE TERM SETTINGS ON EXIT.
	{
		window__flush();
		fprintf(stderr, "A negative char passed to the stdin.\n");
		return ERROR;
	}

	// Immediately restore the state of the stdin (0) to the* new_term_params.
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == ERROR)
	{
		window__flush();
		fprintf(stderr, "Can't restore the terminal's normal mode.\n");
		return ERROR;
	}
	return key;
}

void input_recognize_sequence(Buff_t* Buff, char* sequence)
{
	const size_t seq_max = 3;

	/* Notice that the structure of these sequences is: <ansi_esc_code> + '['
	+ <some_unique_numbers_and_letters>. */
	const char arrow_up[]    = "\x1b[A";
	const char arrow_down[]  = "\x1b[B";
	const char arrow_right[] = "\x1b[C";
	const char arrow_left[]  = "\x1b[D";

	if(!strcmp(sequence, arrow_up))
	{
		keys__arrow_up(Buff);
		Buff->chars_sequence = false;
	}
	else if(!strcmp(sequence, arrow_down))
	{
		keys__arrow_down(Buff);
		Buff->chars_sequence = false;
	}
	else if(!strcmp(sequence, arrow_right))
	{
		keys__arrow_right(Buff);
		Buff->chars_sequence = false;
	}
	else if(!strcmp(sequence, arrow_left))
	{
		keys__arrow_left(Buff);
		Buff->chars_sequence = false;
	}
	else if(strlen(sequence) > seq_max)
	{
		Buff->chars_sequence = false;
	}

#ifdef DEBUG_INPUT
	printf("cusr_x %d, cusr_y %d.\n", Buff->cusr_x, Buff->cusr_y);
#endif

}

bool input_parse_key(Buff_t* Buff, char key)
{
	const size_t  nul_sz = 1;
	enum          {seq_len = 8}; // TODO.
	static char   chars_sequence[seq_len];
	static size_t char_i;

	if((key == CTRL_LEFT_BRACKET) && (!Buff->live_fname_edit))
	{
		Buff->chars_sequence = true;

#ifdef DEBUG_INPUT
		Buff->chars_sequence = false;
#endif

		char_i = 0;
	}
	if(Buff->chars_sequence)
	{
		chars_sequence[char_i] = key;
		if(char_i < (seq_len - nul_sz))
		{
			char_i++;
		}
		chars_sequence[char_i] = '\0';

		input_recognize_sequence(Buff, chars_sequence);
		if(!Buff->chars_sequence)
		{
			char_i = 0;
		}
	}
	else if(Buff->live_fname_edit)
	{
		file__live_edit_name(Buff, key);
	}
	else
	{
		return keys__key_action(Buff, key);
	}
	return true;
}
