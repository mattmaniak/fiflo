#ifndef CHARMAP_H
#define CHARMAP_H

// Currently supported ASCII characters and their actions inside.

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Knows what to do next with pressed key or combination. Based on ASCII.
	Buff_t* (*key_action)(Buff_t*, char);

	// Adds char when the pressed key is a printable one.
	Buff_t* (*printable_char)(Buff_t*, char);

	// Initialize the new line.
	Buff_t* (*linefeed)(Buff_t*);

	// Removes a last char and optionally deletes the last line.
	Buff_t* (*backspace)(Buff_t*);
}
namespace_charmap;

extern namespace_charmap charmap;

#endif
