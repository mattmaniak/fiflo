#ifndef SEQMAP_H
#define SEQMAP_H

// Handles with sequences that aren't single ASCII char.

// Converts the given letter by the parse_key and chooses the cursror direction.
Buff_t* recognize_sequence(Buff_t* Buff, char sequence[8]);

// These ones moves the cursor.
Buff_t* cursor_left(Buff_t* Buff);
Buff_t* cursor_right(Buff_t* Buff);
Buff_t* cursor_up(Buff_t* Buff);
Buff_t* cursor_down(Buff_t* Buff);

static const struct
{
	Buff_t* (*recognize_sequence)(Buff_t*, char[8]);
	Buff_t* (*cursor_left)(Buff_t*);
	Buff_t* (*cursor_right)(Buff_t*);
	Buff_t* (*cursor_up)(Buff_t*);
	Buff_t* (*cursor_down)(Buff_t*);
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
