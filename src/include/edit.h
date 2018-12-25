#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

// As in the name.
F_mtdt* delete_last_line(F_mtdt* Buff);

// Deletes the current line and decrements the lines index.
F_mtdt* delete_line(F_mtdt* Buff);

// Moves the text when the cursor is moved left and char is pressed.
F_mtdt* shift_text_horizonally(F_mtdt* Buff, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
F_mtdt* move_lines_forward(F_mtdt* Buff);

// Simply as in the name.
F_mtdt* delete_last_empty_line(F_mtdt* Buff);

// Also shifts lines from the bottom up.
F_mtdt* delete_non_last_line(F_mtdt* Buff);

// Optionally shifts the text horizontally.
F_mtdt* delete_char(F_mtdt* Buff);

static const struct
{
	F_mtdt* (*delete_last_line)(F_mtdt*);
	F_mtdt* (*delete_line)(F_mtdt*);
	F_mtdt* (*shift_text_horizonally)(F_mtdt* , char);
	F_mtdt* (*move_lines_forward)(F_mtdt* );
	F_mtdt* (*delete_last_empty_line)(F_mtdt* );
	F_mtdt* (*delete_non_last_line)(F_mtdt* );
	F_mtdt* (*delete_char)(F_mtdt* );
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
