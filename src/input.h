#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

extern bool keys_key_action(Buff_t* Buff, char key);
extern void keys_arrow_left(Buff_t* Buff);
extern void keys_arrow_right(Buff_t* Buff);
extern void keys_arrow_up(Buff_t* Buff);
extern void keys_arrow_down(Buff_t* Buff);

extern void file_live_edit_name(Buff_t* Buff, char key);

extern void window_flush(void);

// Reads one char wthout confirming by enter the key. Termios based.
char input_getch(void);

// Converts the given letter by the parse_key and chooses the cursror direction.
void input_recognize_sequence(Buff_t* Buff, char* sequence);

// Saves the last pressed key to the temponary buffer and analyzes it.
bool input_parse_key(Buff_t* Buff, char key);

#endif
