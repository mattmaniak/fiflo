#include "ascii.h"
#include "buffer.h"
#include "keyboard.h"

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

f_mtdt* parse_key(f_mtdt* Buff, char key)
{
	const bool nul_sz = 1;
	enum {seq_len = 8};
	static char key_sequence[seq_len];
	static uint8_t char_i;

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
		(char_i < (seq_len - nul_sz)) ? char_i++ : 0;

		key_sequence[char_i] = '\0';

		Buff = recognize_sequence(Buff, key_sequence);
		(!Buff->key_sequence) ? char_i = 0 : 0;
	}
	else if(Buff->live_fname_edit)
	{
		Buff = edit_fname(Buff, key);
	}
	else
	{
		Buff = key_action(Buff, key);
	}
	return Buff;
}