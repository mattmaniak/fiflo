#ifndef FIFLO_H
#define FIFLO_H

#include <stdbool.h>

// Some shit that starts the program.

extern char input_getch(void);
extern bool input_parse_key(Buff_t* Buff, char key);

extern bool file_set_name(Buff_t* Buff, const char* arg);
extern void file_live_edit_name(Buff_t* Buff, char key);
extern bool file_load(Buff_t* Buff);
extern void file_save(Buff_t* Buff);
extern bool file_load_editor_config(Conf_t* Conf);
extern bool file_get_git_branch(Buff_t* Buff);

extern bool window_render(Buff_t* Buff);
extern void window_flush(void);

// Program parameters, eg. "--help".
bool options(const char* arg);

// Some initializers and the main program loop.
void run(const char* arg);

int main(int argc, char** argv);

#endif
