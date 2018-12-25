#ifndef RENDER_H
#define RENDER_H

// Renders the text itself. Cares about scrolling.

#define BOLD_LINE_NUM true
#define THIN_LINE_NUM false

#define LAST_RENDERED_LINE true
#define ANOTHER_LINE       false

// Returns value of hidden lines.
buff_t set_start_line(F_mtdt* Buff, Win_mtdt* Ui);

// Scrolls chars. Used when the cursor is in static position.
void scroll_line_horizontally(F_mtdt* Buff, Win_mtdt* Ui);

// Decides how to show it. Can scroll it or the cursor.
void print_actual_line(F_mtdt* Buff, Win_mtdt* Ui);

// Renders a text when there is smaller amount of lines than the window height.
void fit_lines(F_mtdt* Buff, Win_mtdt* Ui);

// If there is more lines but they are scrolled to the start.
void shrink_lines(F_mtdt* Buff, Win_mtdt* Ui);

// Lines rendering starts at specified by user offset.
void scroll_lines(F_mtdt* Buff, Win_mtdt* Ui);

// Shows a text in the window.
void display_text(F_mtdt* Buff, Win_mtdt* Ui);

static const struct
{
	buff_t (*set_start_line)(F_mtdt*, Win_mtdt*);
	void   (*scroll_line_horizontally)(F_mtdt*, Win_mtdt*);
	void   (*print_actual_line)(F_mtdt *, Win_mtdt*);
	void   (*fit_lines)(F_mtdt*, Win_mtdt*);
	void   (*shrink_lines)(F_mtdt*, Win_mtdt*);
	void   (*scroll_lines)(F_mtdt*, Win_mtdt*);
	void   (*display_text)(F_mtdt*, Win_mtdt*);
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
