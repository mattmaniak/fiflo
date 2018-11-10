#ifndef FILE_H
#define FILE_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <linux/limits.h>

// keys.h
extern f_mtdt* text_char(char key, f_mtdt* Buff);
extern f_mtdt* linefeed(f_mtdt* Buff);
extern f_mtdt* backspace(f_mtdt* Buff);
extern f_mtdt* ctrl_g(f_mtdt* Buff);
extern f_mtdt* ctrl_h(f_mtdt* Buff);

// Sets the filename. If basename is passed as arg actent path will be added.
f_mtdt* set_fname(const char* arg, f_mtdt* Buff);

// Reads the file using "recognize_char".
f_mtdt* read_file(f_mtdt* Buff);

// Puts the whole text to the file.
void save_file(f_mtdt* Buff);

#endif

