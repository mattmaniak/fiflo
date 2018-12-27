#ifndef SEQMAP_H
#define SEQMAP_H

// Handles with sequences that aren't single ASCII char.

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// These ones moves the cursor.
	Buff_t* (*cursor_left)(Buff_t*);
	Buff_t* (*cursor_right)(Buff_t*);
	Buff_t* (*cursor_up)(Buff_t*);
	Buff_t* (*cursor_down)(Buff_t*);

	// Converts the given letter by the parse_key and chooses the cursror direction.
	Buff_t* (*recognize_sequence)(Buff_t*, char[8]);
}
namespace_seqmap;

extern namespace_seqmap seqmap;

#endif
