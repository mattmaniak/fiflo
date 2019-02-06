#ifndef INPUT_H
#define INPUT_H

#include <termios.h>

#define SEQ_MAX 8

extern void file__live_edit_name(Buff_t*, Conf_t*, Mod_t*, char);

extern bool keys__key_action(Buff_t*, Conf_t*, Mod_t*, char);
extern void keys__arrow_left(Buff_t*, Conf_t*);
extern void keys__arrow_right(Buff_t*, Conf_t*);
extern void keys__arrow_up(Buff_t*);
extern void keys__arrow_down(Buff_t*);

extern void window__flush(void);

// Reads one char wthout confirming by enter the key. Termios based.
char input__getch(void);

// Converts the given letter by the parse_key and chooses the cursror direction.
void input__recognize_sequence(Buff_t*, Conf_t*, char*);

// Saves the last pressed key to the temponary buffer and analyzes it.
bool input__parse_key(Buff_t*, Conf_t*, Mod_t*, char);

#endif
