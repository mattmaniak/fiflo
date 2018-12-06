#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>
#include <stdlib.h>

#define NUL_SZ 1

// seqmap.h
extern f_mtdt* recognize_arrow(f_mtdt* Buff, char sequence[8]);

// file.h
extern f_mtdt* save_file (f_mtdt* Buff);
extern f_mtdt* edit_fname(f_mtdt* Buff, char key);

// memory.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool status);
extern void    chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);
extern void    safer_free(void* ptr);
extern char*   extend_line_mem       (f_mtdt* Buff, buff_t line_i);
extern char*   shrink_act_line_mem   (f_mtdt* Buff);
extern char*   shrink_prev_line_mem  (f_mtdt* Buff);
extern f_mtdt* extend_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* shrink_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* copy_lines_forward    (f_mtdt* Buff);
extern f_mtdt* copy_lines_backward   (f_mtdt* Buff);

// Saves the last pressed key to the temponary buffer and analyzes it.
f_mtdt* parse_key(f_mtdt* Buff, char key);

// As in the name.
f_mtdt* delete_last_line(f_mtdt* Buff);

// Deletes the current line and decrements the lines index.
f_mtdt* delete_line(f_mtdt* Buff);

// Moves the text when the cursor is moved left and char is pressed.
f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
f_mtdt* move_lines_forward(f_mtdt* Buff);

#endif
