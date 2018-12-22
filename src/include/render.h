#ifndef RENDER_H
#define RENDER_H

// Renders the text itself. Cares about scrolling.

#include <stdnoreturn.h>

#define BOLD_LINE_NUM true
#define THIN_LINE_NUM false

#define LAST_RENDERED_LINE true
#define ANOTHER_LINE       false

// buffer.h
//extern noreturn void free_all_exit(f_mtdt* Buff, const bool status);

// window.h
extern void print_line_num(buff_t line, term_t line_num_len, const bool mode);

// Returns value of hidden lines.
buff_t set_start_line(f_mtdt* Buff, win_mtdt Ui);

// Scrolls chars. Used when the cursor is in static position.
void scroll_line_horizontally(f_mtdt* Buff, win_mtdt Ui);

// Decides how to show it. Can scroll it or the cursor.
void print_actual_line(f_mtdt* Buff, win_mtdt Ui);

// Renders a text when there is smaller amount of lines than the window height.
void fit_lines(f_mtdt* Buff, win_mtdt Ui);

// If there is more lines but they are scrolled to the start.
void shrink_lines(f_mtdt* Buff, win_mtdt Ui);

// Lines rendering starts at specified by user offset.
void scroll_lines(f_mtdt* Buff, win_mtdt Ui);

// Shows a text in the window.
void display_text(f_mtdt* Buff, win_mtdt Ui);

static const struct
{
	buff_t (*set_start_line)(f_mtdt*, win_mtdt);
	void   (*scroll_line_horizontally)(f_mtdt*, win_mtdt);
	void   (*print_actual_line)(f_mtdt*, win_mtdt);
	void   (*fit_lines)(f_mtdt*, win_mtdt);
	void   (*shrink_lines)(f_mtdt*, win_mtdt);
	void   (*scroll_lines)(f_mtdt*, win_mtdt);
	void   (*display_text)(f_mtdt*, win_mtdt);
}
render =
{
	set_start_line,
	scroll_line_horizontally,
	print_actual_line,
	fit_lines,
	shrink_lines,
	scroll_lines,
	display_text
};

#endif
