#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <sys/ioctl.h>
#include <unistd.h>

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Returns current terminal width and height and exits if is wrong.
	term_t (*get_term_sz)(Buff_t*, char);

	// Clean the whole rendered window.
	void (*flush)(Buff_t*);

	// Renders the upper bar with a filename and indicators.
	void (*upper_bar)(Buff_t*, Ui_t*);

	// Renders the lower bar that contains keyboard info.
	void (*lower_bar)(Buff_t*);

	// Vertical fill between the text and lower bar. If there isn't many lines.
	void (*fill)(Buff_t*, Ui_t*);

	// Stupid wrapper for above things.
	void (*display)(Buff_t*);

	// Prints the line number.
	void (*print_line_num)(idx_t, term_t, const bool);

	// Sets the cursor position starting from the left bottom.
	void (*set_cursor_pos)(Buff_t*, Ui_t*);
}
namespace_window;

extern namespace_window window;

#endif
