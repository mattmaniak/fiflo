#include "ascii.h"
#include "buffer.h"
#include "input.h"

#include "file.h"
#include "window.h"
#include "keymap.h"

static char getch(void)
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
		window.flush();
		fputs("Stdin params error. Pipe isn't supported.\n", stderr);
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
		window.flush();
		fprintf(stderr, "Can't set the terminal's raw mode.\n");
		return -1;
	}

	if((key = (char) getchar()) < 0) // TODO: RESTORE TERM SETTINGS ON EXIT.
	{
		window.flush();
		fprintf(stderr, "A negative char passed to the stdin.\n");
		return -1;
	}

	// Immediately restore the state of the stdin (0) to the* new_term_params.
	if(tcsetattr(STDIN_FILENO, TCSANOW, &old_term_params) == -1)
	{
		window.flush();
		fprintf(stderr, "Can't restore the terminal's normal mode.\n");
		return -1;
	}
	return key;
}

static void recognize_sequence(Buff_t* Buff, char sequence[8])
{
	const uint8_t seq_max = 3;

	/* Notice that the structure of these sequences is: <ansi_esc_code> + '['
	+ <some_unique_numbers_and_letters>. */
	const char arrow_up[]    = "\x1b[A";
	const char arrow_down[]  = "\x1b[B";
	const char arrow_right[] = "\x1b[C";
	const char arrow_left[]  = "\x1b[D";

	if(!strcmp(sequence, arrow_up))
	{
		keymap.arrow_up(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_down))
	{
		keymap.arrow_down(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_right))
	{
		keymap.arrow_right(Buff);
		Buff->key_sequence = false;
	}
	else if(!strcmp(sequence, arrow_left))
	{
		keymap.arrow_left(Buff);
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

static int parse_key(Buff_t* Buff, char key)
{
	const bool     nul_sz = 1;
	enum           {seq_len = 8};
	static char    key_sequence[seq_len];
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

		recognize_sequence(Buff, key_sequence);
		(!Buff->key_sequence) ? char_i = 0 : 0;
	}
	else if(Buff->live_fname_edit)
	{
		file.live_edit_name(Buff, key);
	}
	else
	{
		return keymap.key_action(Buff, key);
	}
	return 0;
}

namespace_input input =
{
	getch,
	recognize_sequence,
	parse_key
};
