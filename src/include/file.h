#ifndef FILE_H
#define FILE_H

// File operations.

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

// Sets the filename. If basename is passed as arg actent path will be added.
Buff_t* set_name(Buff_t* Buff, const char* passed);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
Buff_t* live_edit_name(Buff_t* Buff, char key);

// Reads the file using "recognize_char".
Buff_t* load(Buff_t* Buff);

// Puts the whole text to the file.
Buff_t* save(Buff_t* Buff);

static const struct
{
	Buff_t* (*set_name)(Buff_t*, const char*);
	Buff_t* (*live_edit_name)(Buff_t*, char);
	Buff_t* (*load)(Buff_t*);
	Buff_t* (*save)(Buff_t*);
}
file =
{
	set_name,
	live_edit_name,
	load,
	save
};

#endif
