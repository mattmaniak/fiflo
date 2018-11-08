#ifndef FILE_H
#define FILE_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <linux/limits.h>

#define NEG      -1
#define NTERM     0
#define CTRL_D    4
#define TAB       9
#define LF        10
#define CTRL_X    24
#define ESCAPE    27 // '\033' - ANSI escape code initial sequence.
#define BACKSPACE 127

// From the "keys.h".
extern meta* text_char(char key, meta* Dt);
extern meta* linefeed(meta* Dt);
extern meta* backspace(meta* Dt);
extern meta* ctrl_g(meta* Dt);
extern meta* ctrl_h(meta* Dt);

// Sets the filename. If basename is passed as arg actent path will be added.
meta* set_fname(const char* arg, meta* Dt);

// Reads the file using "recognize_char".
meta* read_file(meta* Dt);

// Puts the whole text to the file.
void save_file(meta* Dt);

#endif

