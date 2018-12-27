#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

// As in the name.
// Buff_t* delete_last_line(Buff_t* Buff);

// Deletes the current line and decrements the lines index.
// Buff_t* delete_line(Buff_t* Buff);

// Moves the text when the cursor is moved left and char is pressed.
// Buff_t* shift_text_horizonally(Buff_t* Buff, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
// Buff_t* move_lines_forward(Buff_t* Buff);

// Simply as in the name.
// Buff_t* delete_last_empty_line(Buff_t* Buff);

// Also shifts lines from the bottom up.
// Buff_t* delete_non_last_line(Buff_t* Buff);

// Optionally shifts the text horizontally.
// Buff_t* delete_char(Buff_t* Buff);

typedef const struct
{
	Buff_t* (*delete_last_line)(Buff_t*);
	Buff_t* (*delete_line)(Buff_t*);
	Buff_t* (*shift_text_horizonally)(Buff_t*, char);
	Buff_t* (*move_lines_forward)(Buff_t*);
	Buff_t* (*delete_last_empty_line)(Buff_t*);
	Buff_t* (*delete_non_last_line)(Buff_t*);
	Buff_t* (*delete_char)(Buff_t*);
}
namespace_edit;

extern namespace_edit edit;

#endif
