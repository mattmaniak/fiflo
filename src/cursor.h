#ifndef CURSOR_H
#define CURSOR_H

// These ones moves ther cursor.
f_mtdt* cursor_left (f_mtdt* Buff);
f_mtdt* cursor_right(f_mtdt* Buff);
f_mtdt* cursor_up   (f_mtdt* Buff);
f_mtdt* cursor_down (f_mtdt* Buff);

f_mtdt* ansi_escape_code_from_keyboard(f_mtdt* Buff, char key);

#endif
