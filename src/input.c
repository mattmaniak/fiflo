#include "ascii.h"
#include "buffer.h"
#include "input.h"
// #include "keys.h"



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
	if(tcgetattr(STDIN_FILENO, &old_term_params) == -1)
	{
		window_flush();
		fprintf(stderr, "Stdin params error. Pipe isn't supported.\n");
		return -1;
	}
	new_term_params = old_term_params;

	// Look that the options of below flags are negated.
	new_term_params.c_iflag &= ~(enable_xon);
	new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

	/* Immediately set the state of the STDIN_FILENO to the* new_term_params.
	Use the new terminal I/O settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == -1)
	{
		window_flush();
		fprintf(stderr, "Can't set the terminal's raw mode.\n");
		return -1;
	}

	if((key = (char) getchar()) < 0) // TODO: RESTORE TERM SETTINGS ON EXIT.
	{
		window_flush();
		fprintf(stderr, "A negative char passed to the stdin.\n");
		return -1;
	}

	// Immediately restore the state of the stdin (0) to the* new_term_params.
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == -1)
	{
		window_flush();
		fprintf(stderr, "Can't restore the terminal's normal mode.\n");
		return -1;
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
		keys_arrow_up(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_down))
	{
		keys_arrow_down(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_right))
	{
		keys_arrow_right(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_left))
	{
		keys_arrow_left(Buff);
		Buff->key_sequence = false;
	}
	else if(strlen(sequence) > seq_max)
	{
		Buff->key_sequence = false;
	}

#ifdef SHOW_VALUES
	printf("cusr_x %d, cusr_y %d.\n", Buff->cusr_x, Buff->cusr_y);
#endif

}

bool input_parse_key(Buff_t* Buff, char key)
{
	const size_t  nul_sz = 1;
	enum          {seq_len = 8}; // TODO.
	static char   key_sequence[seq_len];
	static size_t char_i;

	if((key == CTRL_LEFT_BRACKET) && (!Buff->live_fname_edit))
	{

#ifndef VALUES_INSTEAD_OF_ACTIONS
		Buff->key_sequence = true;
#endif

		char_i = 0;
	}
	if(Buff->key_sequence)
	{
		key_sequence[char_i] = key;
		if(char_i < (seq_len - nul_sz))
		{
			char_i++;
		}
		key_sequence[char_i] = '\0';

		input_recognize_sequence(Buff, key_sequence);
		if(!Buff->key_sequence)
		{
			char_i = 0;
		}
	}
	else if(Buff->live_fname_edit)
	{
		file_live_edit_name(Buff, key);
	}
	else
	{
		return keys_key_action(Buff, key);
	}
	return true;
}