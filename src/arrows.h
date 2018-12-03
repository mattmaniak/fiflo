#ifndef ARROWS_H
#define ARROWS_H

// Converts the given letter by the parse_key and chooses the cursror direction.
f_mtdt* recognize_arrow(f_mtdt* Buff, char key);

// These ones moves the cursor.
f_mtdt* move_cursor_left (f_mtdt* Buff);
f_mtdt* move_cursor_right(f_mtdt* Buff);
f_mtdt* move_cursor_up   (f_mtdt* Buff);
f_mtdt* move_cursor_down (f_mtdt* Buff);

#endif
