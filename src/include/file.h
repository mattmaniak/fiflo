#ifndef FILE_H
#define FILE_H

// File operations.

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

// memory.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool status);
extern void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);

// charmap.h
extern f_mtdt* printable_char(f_mtdt* Buff, char key);
extern f_mtdt* linefeed(f_mtdt* Buff);
extern f_mtdt* backspace(f_mtdt* Buff);

// Sets the filename. If basename is passed as arg actent path will be added.
f_mtdt* set_fname(f_mtdt* Buff, const char* passed);

// Reads the file using "recognize_char".
f_mtdt* read_file(f_mtdt* Buff);

// Puts the whole text to the file.
f_mtdt* save_file(f_mtdt* Buff);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
f_mtdt* edit_fname(f_mtdt* Buff, char key);

#endif
