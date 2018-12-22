#ifndef CHARMAP_H
#define CHARMAP_H

// Currently supported ASCII characters and their actions inside.

// Knows what to do next with pressed key or combination. Based on ASCII.
f_mtdt* key_action(f_mtdt* Buff, char key);

// Adds char when the pressed key is a printable one.
f_mtdt* printable_char(f_mtdt* Buff, char key);

// Initialize the new line.
f_mtdt* linefeed(f_mtdt* Buff);

// Removes a last char and optionally deletes the last line.
f_mtdt* backspace(f_mtdt* Buff);

static const struct
{
	f_mtdt* (*key_action)(f_mtdt*, char);
	f_mtdt* (*printable_char)(f_mtdt*, char);
	f_mtdt* (*linefeed)(f_mtdt*);
	f_mtdt* (*backspace)(f_mtdt*);
}
charmap =
{
	key_action,
	printable_char,
	linefeed,
	backspace
};

#endif
