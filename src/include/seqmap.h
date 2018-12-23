#ifndef SEQMAP_H
#define SEQMAP_H

// Handles with sequences that aren't single ASCII char.

// Converts the given letter by the parse_key and chooses the cursror direction.
F_mtdt* recognize_sequence(F_mtdt* Buff, char sequence[8]);

// These ones moves the cursor.
F_mtdt* cursor_left(F_mtdt* Buff);
F_mtdt* cursor_right(F_mtdt* Buff);
F_mtdt* cursor_up(F_mtdt* Buff);
F_mtdt* cursor_down(F_mtdt* Buff);

static const struct
{
	F_mtdt* (*recognize_sequence)(F_mtdt*, char[8]);
	F_mtdt* (*cursor_left)(F_mtdt*);
	F_mtdt* (*cursor_right)(F_mtdt*);
	F_mtdt* (*cursor_up)(F_mtdt*);
	F_mtdt* (*cursor_down)(F_mtdt*);
}
seqmap =
{
	recognize_sequence,
	cursor_left,
	cursor_right,
	cursor_up,
	cursor_down
};

#endif
