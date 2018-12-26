#ifndef CHARMAP_H
#define CHARMAP_H

// Currently supported ASCII characters and their actions inside.

// Knows what to do next with pressed key or combination. Based on ASCII.
Buff_t* key_action(Buff_t* Buff, char key);

// Adds char when the pressed key is a printable one.
Buff_t* printable_char(Buff_t* Buff, char key);

// Initialize the new line.
Buff_t* linefeed(Buff_t* Buff);

// Removes a last char and optionally deletes the last line.
Buff_t* backspace(Buff_t* Buff);

static const struct
{
	Buff_t* (*key_action)(Buff_t*, char);
	Buff_t* (*printable_char)(Buff_t*, char);
	Buff_t* (*linefeed)(Buff_t*);
	Buff_t* (*backspace)(Buff_t*);
}
charmap =
{
	key_action,
	printable_char,
	linefeed,
	backspace
};

#endif
