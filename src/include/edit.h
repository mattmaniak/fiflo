#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

// As in the name.
f_mtdt* delete_last_line(f_mtdt* Buff);

// Deletes the current line and decrements the lines index.
f_mtdt* delete_line(f_mtdt* Buff);

// Moves the text when the cursor is moved left and char is pressed.
f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
f_mtdt* move_lines_forward(f_mtdt* Buff);

// Simply as in the name.
f_mtdt* delete_last_empty_line(f_mtdt* Buff);

// Also shifts lines from the bottom up.
f_mtdt* delete_non_last_line(f_mtdt* Buff);

// Optionally shifts the text horizontally.
f_mtdt* delete_char(f_mtdt* Buff);

static const struct
{
	f_mtdt* (*delete_last_line)(f_mtdt*);
	f_mtdt* (*delete_line)(f_mtdt*);
	f_mtdt* (*shift_text_horizonally)(f_mtdt*, char);
	f_mtdt* (*move_lines_forward)(f_mtdt*);
	f_mtdt* (*delete_last_empty_line)(f_mtdt*);
	f_mtdt* (*delete_non_last_line)(f_mtdt*);
	f_mtdt* (*delete_char)(f_mtdt*);
}
edit =
{
	delete_last_line,
	delete_line,
	shift_text_horizonally,
	move_lines_forward,
	delete_last_empty_line,
	delete_non_last_line,
	delete_char
};

#endif
