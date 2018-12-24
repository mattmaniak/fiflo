#include "include/ascii.h"
#include "include/buffer.h"
#include "include/keyboard.h"

#include "include/file.h"
#include "include/ui.h"
#include "include/window.h"

char getch(F_mtdt* Buff)
{
	const int8_t error = -1;

	const unsigned int canonical_mode_on = ICANON;
	const unsigned int echo_input = ECHO;
	const unsigned int enable_sigs = ISIG;
	const unsigned int enable_xon = IXON;

	struct termios old_term_params;
	struct termios new_term_params;

	char key;

	// Get the state of the STDIN_FILENO.
	if(tcgetattr(STDIN_FILENO, &old_term_params) == error)
	{
		window.flush(Buff);
		buffer.throw_error(Buff, "Stdin params error. Pipe isn't supported.");
	}
	new_term_params = old_term_params;

	// Look that the options of below flags are negated.
	new_term_params.c_iflag &= ~(enable_xon);
	new_term_params.c_lflag &= ~(canonical_mode_on | echo_input | enable_sigs);

	/* Immediately set the state of the STDIN_FILENO to the *new_term_params.
	Use the new terminal I/O settings. */
	if(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_params) == error)
	{
		window.flush(Buff);
		buffer.throw_error(Buff, "Can't set the terminal's raw mode.");
	}

	if((key = (char) getchar()) < 0) // TODO: RESTORE TERM SETTINGS ON EXIT.
	{
		window.flush(Buff);
		buffer.throw_error(Buff, "A negative char passed to the stdin.");
	}

	// Immediately restore the state of the stdin (0) to the *new_term_params.
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == error)
	{
		window.flush(Buff);
		buffer.throw_error(Buff, "Can't restore the terminal's normal mode.");
	}
	return key;
}

F_mtdt* parse_key(F_mtdt* Buff, char key)
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
		Buff = file.live_edit_name(Buff, key);
	}
	else
	{
		Buff = key_action(Buff, key);
	}
	return Buff;
}
