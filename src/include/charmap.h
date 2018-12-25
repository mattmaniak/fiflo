#ifndef CHARMAP_H
#define CHARMAP_H

// Currently supported ASCII characters and their actions inside.

// Knows what to do next with pressed key or combination. Based on ASCII.
F_mtdt* key_action(F_mtdt* Buff, char key);

// Adds char when the pressed key is a printable one.
F_mtdt* printable_char(F_mtdt* Buff, char key);

// Initialize the new line.
F_mtdt* linefeed(F_mtdt* Buff);

// Removes a last char and optionally deletes the last line.
F_mtdt* backspace(F_mtdt* Buff);

static const struct
{
	F_mtdt* (*key_action)(F_mtdt* , char);
	F_mtdt* (*printable_char)(F_mtdt* , char);
	F_mtdt* (*linefeed)(F_mtdt* );
	F_mtdt* (*backspace)(F_mtdt* );
}
charmap =
{
	key_action,
	printable_char,
	linefeed,
	backspace
};

#endif
