#ifndef RENDER_H
#define RENDER_H

// Renders the text itself. Cares about scrolling.

#define BOLD_LINE_NUM true
#define THIN_LINE_NUM false

#define LAST_RENDERED_LINE true
#define ANOTHER_LINE       false

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Returns value of hidden lines.
	idx_t (*set_start_line)(Buff_t*, Ui_t*);

	// Scrolls chars. Used when the cursor is in static position.
	void (*scroll_line_horizontally)(Buff_t*, Ui_t*);

	// Decides how to show it. Can scroll it or the cursor.
	void (*print_actual_line)(Buff_t*, Ui_t*);

	// Renders a text when there is smaller amount of lines than the window height.
	void (*fit_lines)(Buff_t*, Ui_t*);

	// If there is more lines but they are scrolled to the start.
	void (*shrink_lines)(Buff_t*, Ui_t*);

	// Lines rendering starts at specified by user offset.
	void (*scroll_lines)(Buff_t*, Ui_t*);

	// Shows a text in the window.
	void (*display_text)(Buff_t*, Ui_t*);
}
namespace_render;

extern namespace_render render;

#endif
