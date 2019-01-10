#ifndef FILE_H
#define FILE_H

// File operations.

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Sets the filename. If basename is passed as arg actent path will be added.
	bool (*set_name)(Buff_t*, const char*);

	// When the CTRL^O is pressed, there is possibility to basically edit the fname.
	void (*live_edit_name)(Buff_t*, char);

	// Reads the file using "recognize_char".
	bool (*load)(Buff_t*);

	// Puts the whole text to the file.
	void (*save)(Buff_t*);

	//
	bool (*get_git_branch)(Buff_t*);
}
namespace_file;

extern namespace_file file;

#endif
