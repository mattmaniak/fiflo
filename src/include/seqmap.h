#ifndef SEQMAP_H
#define SEQMAP_H

// Handles with sequences that aren't single ASCII char.

// fiflo.h
extern char getch(f_mtdt* Buff);

// Converts the given letter by the parse_key and chooses the cursror direction.
f_mtdt* recognize_sequence(f_mtdt* Buff, char sequence[8]);

// These ones moves the cursor.
f_mtdt* cursor_left(f_mtdt* Buff);
f_mtdt* cursor_right(f_mtdt* Buff);
f_mtdt* cursor_up(f_mtdt* Buff);
f_mtdt* cursor_down(f_mtdt* Buff);

#endif