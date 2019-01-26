#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

extern bool keys__key_action(Buff_t* Buffer, Conf_t* Config, char key);
extern void keys__arrow_left(Buff_t* Buffer);
extern void keys__arrow_right(Buff_t* Buffer);
extern void keys__arrow_up(Buff_t* Buffer);
extern void keys__arrow_down(Buff_t* Buffer);

extern void file__live_edit_name(Buff_t* Buffer, char key);

extern void window__flush(void);

// Reads one char wthout confirming by enter the key. Termios based.
char input_getch(void);

// Converts the given letter by the parse_key and chooses the cursror direction.
void input_recognize_sequence(Buff_t* Buffer, char* sequence);

// Saves the last pressed key to the temponary buffer and analyzes it.
bool input_parse_key(Buff_t* Buffer, Conf_t* Config, char key);

#endif
