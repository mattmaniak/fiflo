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

extern bool keys__printable_char(Buff_t* Buffer, char key);

// Sets the filename. If basename is passed as arg actent path will be added.
bool file__set_name(Buff_t* Buffer, const char* arg);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
void file__live_edit_name(Buff_t* Buffer, char key);

// If there is tab char in a file, load e.g. 4 tabs to a memory.
bool file__convert_from_tab(Buff_t* Buffer, Conf_t* Config, char ch);

// Reads the file using "input_recognize_char".
bool file__load(Buff_t* Buffer, Conf_t* Config);

// Converts in-fiflo tabs [e.g. "\t\t\t\t"] to the original '\t'.
void file__convert_to_tab(Buff_t* Buffer, Conf_t* Config, idx_t line_idx,
                          idx_t* char_idx);

// Puts the whole text to the file.
void file__save(Buff_t* Buffer, Conf_t* Config);

//
bool file__load_editor_config(Conf_t* Conf);

//
bool file__get_git_branch(Buff_t* Buffer);

#endif
