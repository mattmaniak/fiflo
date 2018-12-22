#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <stdnoreturn.h>
#include <sys/ioctl.h>
#include <unistd.h>

// render.h
extern buff_t set_start_line(f_mtdt* Buff);
extern void scroll_line_x(f_mtdt* Buff, win_mtdt Ui);
extern void print_actual_line(f_mtdt* Buff, win_mtdt Ui);
extern void display_text(f_mtdt* Buff, win_mtdt Ui);

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
void print_line_num(buff_t line_i, term_t line_num_len, const bool act_line);

// Sets the cursor position from the left bottom.
void set_cursor_pos(f_mtdt* Buff, win_mtdt Ui);

static const struct
{
	term_t (*get_term_sz)(f_mtdt*, char);
	void   (*flush_window)(f_mtdt*);
	void   (*upper_bar)(f_mtdt*, win_mtdt);
	void   (*lower_bar)(f_mtdt*);
	void   (*fill)(f_mtdt*, win_mtdt);
	void   (*render_window)(f_mtdt*);
	void   (*print_line_num)(buff_t, term_t, const bool);
	void   (*set_cursor_pos)(f_mtdt*, win_mtdt);
}
window =
{
	get_term_sz,
	flush_window,
	upper_bar,
	lower_bar,
	fill,
	render_window,
	print_line_num,
	set_cursor_pos
};

#endif
