#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>

#define CUR_SZ 1
#define LF_SZ  1
#define LF     10
#define INDEX  1

#define BOLD_LINE_NUM true
#define THIN_LINE_NUM false

#define LAST_RENDERED_LINE    true
#define ANOTHER_RENDERED_LINE false

// buffer.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool status);

// window.h
extern void print_line_num(buff_t line, uint8_t line_num_len, const bool mode);

// Returns value of hidden lines.
buff_t set_start_line(f_mtdt* Buff, win_mtdt Ui);

// Scrolls chars. Used when the cursor is in static position.
void scroll_line_horizontally(f_mtdt* Buff, win_mtdt Ui);

// Decides how to show it. Can scroll it or the cursor.
void print_actual_line(f_mtdt* Buff, win_mtdt Ui, const bool mode);

// Renders a text when there is smaller amount of lines than the window height.
void fit_lines(f_mtdt* Buff, win_mtdt Ui);

// If there is more lines but they are scrolled to a start.
void shrink_lines(f_mtdt* Buff, win_mtdt Ui);

// Lines rendering starts at specified by user offset.s
void scroll_lines(f_mtdt* Buff, win_mtdt Ui);

// Shows a text in the window.
void display_text(f_mtdt* Buff, win_mtdt Ui);

#endif
