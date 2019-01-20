#ifndef FIFLO_H
#define FIFLO_H

#include <stdbool.h>

// Some shit that starts the program.

extern char input_getch(void);
extern bool input_parse_key(Buff_t* Buff, char key);

extern bool file__set_name(Buff_t* Buff, const char* arg);
extern void file__live_edit_name(Buff_t* Buff, char key);
extern bool file__load(Buff_t* Buff);
extern void file__save(Buff_t* Buff);
extern bool file__load_editor_config(Conf_t* Conf);
extern bool file__get_git_branch(Buff_t* Buff);

extern bool window__render(Buff_t* Buff, Conf_t* Config);
extern void window__flush(void);

// Program parameters, eg. "--help".
bool fiflo__options(const char* arg);

// Some initializers and the main program loop.
void fiflo__run(const char* arg);

int main(int argc, char** argv);

#endif
