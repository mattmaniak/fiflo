#ifndef WINDOW_H
#define WINDOW_H

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define INDEX 1

// buffer.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool code);

// text_processing.h
extern buff_t set_start_line   (f_mtdt* Buff);
extern void   scroll_line_x    (f_mtdt* Buff, win_mtdt Ui);
extern void   print_actual_line(f_mtdt* Buff, win_mtdt Ui);
extern void   display_text     (f_mtdt* Buff, win_mtdt Ui);

// Returns current terminal width and height and exits if is wrong.
term_t get_term_sz(f_mtdt* Buff, char axis);

// Clean the whole rendered window.
void flush_window(f_mtdt* Buff);

// Renders the upper bar with a filename and indicators.
void upper_bar(f_mtdt* Buff, win_mtdt Ui);

// Renders the lower bar that contains keyboard info.
void lower_bar(f_mtdt* Buff);

// Vertical fill between the text and lower bar. If there isn't many lines.
void fill(f_mtdt* Buff, win_mtdt Ui);

// Stupid wrapper for above things.
void render_window(f_mtdt* Buff);

// Prints the line number.
void print_line_num(buff_t line_i, uint8_t line_num_len, const bool act_line);

// Sets the cursor position from the left bottom.
void set_cursor_pos(f_mtdt* Buff, win_mtdt Ui);

#endif
