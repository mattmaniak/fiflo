#ifndef FIFLO_H
#define FIFLO_H

#include <stdbool.h>

// Some shit that starts the program.

extern char input__getch(void);
extern bool input__parse_key(Buff_t*, Conf_t*, char);

extern bool file__set_name(Buff_t*, const char*);
extern void file__live_edit_name(Buff_t*, char);
extern bool file__load(Buff_t*, Conf_t*);
extern void file__save(Buff_t*);
extern bool file__load_config(Conf_t*);
extern bool file__get_git_branch(Buff_t*);

extern bool window__render(Buff_t*, Conf_t*);
extern void window__flush(void);

extern bool options__parse_and_print(const char*);

// Some initializers and the main program loop.
void fiflo__run(const char*);

int main(int, char**);

#endif
