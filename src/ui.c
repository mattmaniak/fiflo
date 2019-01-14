#include "buffer.h"
#include "ui.h"

void ui_print_line_number(idx_t line_i, term_t line_num_len,
                          const bool current_line)
{
	ANSI_MAGENTA_BRIGHT();

	if(current_line) // Higlight a current line.
	{
		ANSI_MAGENTA();
	}
	printf("%*d ", line_num_len - SPACE_SZ, ++line_i); // Increment the index.

	ANSI_RESET();
	ANSI_GREEN(); // Text color.
}
