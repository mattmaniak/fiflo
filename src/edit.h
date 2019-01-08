#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// As in the name.
	int (*delete_last_line)(Buff_t*);

	// Deletes the current line and decrements the lines index.
	int (*delete_line)(Buff_t*);

	// Moves the text when the cursor is moved left and char is pressed.
	void (*shift_text_horizonally)(Buff_t*, char);

	// As in the name, but supports Y shift with the cursor scrolled horizontally.
	int (*move_lines_forward)(Buff_t*);

	// Simply as in the name.
	int (*delete_last_empty_line)(Buff_t*);

	// Also shifts lines from the bottom up.
	int (*delete_non_last_line)(Buff_t*);

	// Optionally shifts the text horizontally.
	int (*delete_char)(Buff_t*);
}
namespace_edit;

extern namespace_edit edit;

#endif
