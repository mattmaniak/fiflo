#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <sys/ioctl.h>
#include <unistd.h>

// Some semantic macros.
#define CUR_SZ   1
#define LF_SZ    1
#define SLASH_SZ 1
#define SPACE_SZ 1

#define UBAR_SZ 3
#define LBAR_SZ 1 // Must be equal 1, otherwise will break rendering a little.

#define TOGGLED_PANE_SZ 5

// Text that is shown on the lower bar. Also defines the minimal terminal width.
#define LBAR_STR "CTRL^\\ - keyboard bindings"

#define WRAP_LINE() putchar('\n')

// ANSI escape codes.
#define ANSI_RED()             printf("\x1b[%2", " m")
#define ANSI_GREEN()           printf("\x1b[%2", " m")

#define ANSI_RESET()           printf("\x1b[%s", "0m")
#define ANSI_INVERT()          printf("\x1b[%s", "7m")
#define ANSI_CLEAN_LINE()      printf("\x1b[%s", "2K")
#define ANSI_CUR_UP(offset)    printf("\x1b[%uA", offset)
#define ANSI_CUR_DOWN(offset)  printf("\x1b[%uB", offset)
#define ANSI_CUR_RIGHT(offset) printf("\x1b[%uC", offset)
#define ANSI_CUR_LEFT(offset)  printf("\x1b[%uD", offset)
#define ANSI_SAVE_CUR_POS()    printf("\x1b[%s", "s")
#define ANSI_RESTORE_CUR_POS() printf("\x1b[%s", "u")

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

typedef struct
{
	char   line_num_str[16]; // Place for string of the highest line number.
	term_t line_num_len;     // Dynamic width of the lines numbers.
	term_t text_x;           // Horizontal space for the text (width: chars).
	term_t text_y;           // Vertical space for the text (lines).
	term_t lbar_h;           // Lower bar height (lines).
	term_t pane_h;           // As above but toggled.

	term_t win_w;
	term_t win_h;
}
Ui_t;

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
	void (*lower_bar)(Buff_t*, Ui_t* Ui);

	// Vertical fill between the text and lower bar. If there isn't many lines.
	void (*fill)(Buff_t*, Ui_t*);

	// Stupid wrapper for above things.
	void (*render)(Buff_t*);

	// Prints the line number.
	void (*print_line_num)(idx_t, term_t, const bool);

	// Sets the cursor position starting from the left bottom.
	void (*set_cursor_pos)(Buff_t*, Ui_t*);
}
namespace_window;

extern namespace_window window;

#endif
