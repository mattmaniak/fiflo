#ifndef FILE_H
#define FILE_H

// File operations.

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

// Sets the filename. If basename is passed as arg actent path will be added.
F_mtdt* set_name(F_mtdt* Buff, const char* passed);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
F_mtdt* edit_name(F_mtdt* Buff, char key);

// Reads the file using "recognize_char".
F_mtdt* load(F_mtdt* Buff);

// Puts the whole text to the file.
F_mtdt* save(F_mtdt* Buff);

static const struct
{
	F_mtdt* (*set_name)(F_mtdt*, const char*);
	F_mtdt* (*edit_name)(F_mtdt*, char);
	F_mtdt* (*load)(F_mtdt*);
	F_mtdt* (*save)(F_mtdt*);
}
file =
{
	set_name,
	edit_name,
	load,
	save
};

#endif
