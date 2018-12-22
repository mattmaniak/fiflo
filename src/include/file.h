#ifndef FILE_H
#define FILE_H

// File operations.

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

// Sets the filename. If basename is passed as arg actent path will be added.
f_mtdt* set_name(f_mtdt* Buff, const char* passed);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
f_mtdt* edit_name(f_mtdt* Buff, char key);

// Reads the file using "recognize_char".
f_mtdt* load(f_mtdt* Buff);

// Puts the whole text to the file.
f_mtdt* save(f_mtdt* Buff);

static const struct
{
	f_mtdt* (*set_name)(f_mtdt*, const char*);
	f_mtdt* (*edit_name)(f_mtdt*, char);
	f_mtdt* (*load)(f_mtdt*);
	f_mtdt* (*save)(f_mtdt*);
}
file =
{
	set_name,
	edit_name,
	load,
	save
};

#endif
