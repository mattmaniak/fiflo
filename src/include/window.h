#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <sys/ioctl.h>
#include <unistd.h>

// Returns current terminal width and height and exits if is wrong.
term_t get_term_sz(Buff_t* Buff, char axis);

// Clean the whole rendered window.
void flush(Buff_t* Buff);

// Renders the upper bar with a filename and indicators.
void upper_bar(Buff_t* Buff, Ui_t* Ui);

// Renders the lower bar that contains keyboard info.
void lower_bar(Buff_t* Buff);

// Vertical fill between the text and lower bar. If there isn't many lines.
void fill(Buff_t* Buff, Ui_t* Ui);

// Stupid wrapper for above things.
void display(Buff_t* Buff);

// Prints the line number.
void print_line_num(idx_t line_i, term_t line_num_len, const bool act_line);

// Sets the cursor position from the left bottom.
void set_cursor_pos(Buff_t* Buff, Ui_t* Ui);

static const struct
{
	term_t (*get_term_sz)(Buff_t*, char);
	void   (*flush)(Buff_t*);
	void   (*upper_bar)(Buff_t*, Ui_t*);
	void   (*lower_bar)(Buff_t*);
	void   (*fill)(Buff_t*, Ui_t*);
	void   (*display)(Buff_t*);
	void   (*print_line_num)(idx_t, term_t, const bool);
	void   (*set_cursor_pos)(Buff_t*, Ui_t*);
}
window =
{
	get_term_sz,
	flush,
	upper_bar,
	lower_bar,
	fill,
	display,
	print_line_num,
	set_cursor_pos
};

#endif
