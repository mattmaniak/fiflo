#ifndef FILE_H
#define FILE_H

// File operations.

#include <linux/limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

extern bool keys_printable_char(Buff_t* Buff, char key);

// Sets the filename. If basename is passed as arg actent path will be added.
bool file_set_name(Buff_t* Buff, const char* arg);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
void file_live_edit_name(Buff_t* Buff, char key);

// Reads the file using "input_recognize_char".
bool file_load(Buff_t* Buff);

// Puts the whole text to the file.
void file_save(Buff_t* Buff);

//
bool file_load_editor_config(Conf_t* Conf);

//
bool file_get_git_branch(Buff_t* Buff);

#endif
