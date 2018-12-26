#ifndef RENDER_H
#define RENDER_H

// Renders the text itself. Cares about scrolling.

#define BOLD_LINE_NUM true
#define THIN_LINE_NUM false

#define LAST_RENDERED_LINE true
#define ANOTHER_LINE       false

// Returns value of hidden lines.
idx_t set_start_line(Buff_t* Buff, Ui_t* Ui);

// Scrolls chars. Used when the cursor is in static position.
void scroll_line_horizontally(Buff_t* Buff, Ui_t* Ui);

// Decides how to show it. Can scroll it or the cursor.
void print_actual_line(Buff_t* Buff, Ui_t* Ui);

// Renders a text when there is smaller amount of lines than the window height.
void fit_lines(Buff_t* Buff, Ui_t* Ui);

// If there is more lines but they are scrolled to the start.
void shrink_lines(Buff_t* Buff, Ui_t* Ui);

// Lines rendering starts at specified by user offset.
void scroll_lines(Buff_t* Buff, Ui_t* Ui);

// Shows a text in the window.
void display_text(Buff_t* Buff, Ui_t* Ui);

static const struct
{
	idx_t (*set_start_line)(Buff_t*, Ui_t*);
	void  (*scroll_line_horizontally)(Buff_t*, Ui_t*);
	void  (*print_actual_line)(Buff_t*, Ui_t*);
	void  (*fit_lines)(Buff_t*, Ui_t*);
	void  (*shrink_lines)(Buff_t*, Ui_t*);
	void  (*scroll_lines)(Buff_t*, Ui_t*);
	void  (*display_text)(Buff_t*, Ui_t*);
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
