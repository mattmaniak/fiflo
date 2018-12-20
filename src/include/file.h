#ifndef FILE_H
#define FILE_H

// File operations.

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

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

static const struct
{
	f_mtdt* (*set_fname)(f_mtdt* Buff, const char* passed);
	f_mtdt* (*read_file)(f_mtdt* Buff);
	f_mtdt* (*save_file)(f_mtdt* Buff);
	f_mtdt* (*edit_fname)(f_mtdt* Buff, char key);
}
file =
{
	set_fname,
	read_file,
	save_file,
	edit_fname
};

#endif
