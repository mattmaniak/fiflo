#ifndef TEXT_RENDERING_H
#define TEXT_RENDERING_H

#define CUR_SZ   1
#define LF_SZ    1

// TODO: EXTERN?
#pragma pack(push, 1)
typedef struct
{
	char    line_num_str[16]; // Place for string of the highest line number.
	uint8_t line_num_len;     // Dynamic width of the lines numbers.
	term_t  text_x;           // Horizontal space for the text (width: chars).
	term_t  text_y;           // Vertical space for the text (lines).
}
win_mtdt;
#pragma pack(pop)

extern void print_line_num(buff_t line, uint8_t line_num_len);

// Scrolls chars. Used when the cursor is in static position.
void scroll_line_x(f_mtdt* Buff, win_mtdt Ui);

// Returns value of hidden lines.
buff_t set_start_line(f_mtdt* Buff, win_mtdt Ui);

// TODO: COMMENT.
void print_actual_line(f_mtdt* Buff, win_mtdt Ui);

// Shows a text in the window.
void display_text(f_mtdt* Buff, win_mtdt Ui);

#endif

