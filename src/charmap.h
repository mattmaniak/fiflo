#ifndef CHARMAP_H
#define CHARMAP_H

#include <stdio.h>

#define NEG               -1 // Pipe.

// Formatting control chars.
#define NUL__CTRL_SHIFT_2 0x00 // Terminator.
#define BEL__CTRL_G       0x07 // Alarm beeper.
#define BS__CTRL_H        0x08 // Fake backspace.
#define HT__CTRL_I        0x09 // Horizontal tab.
#define LF__CTRL_J        0x0a // Linefeed/newline.
#define VT__CTRL_K        0x0b // Vertical tab.
#define FF__CTRL_L        0x0c // Form feed.

// Control characters that can be used as keyboard shortcuts.
#define STX__CTRL_A                0x01 // Start of header
#define SOT__CTRL_B                0x02 // Start of text.
#define ETX__CTRL_C                0x03 // End of text.
#define EOT__CTRL_D                0x04 // End of transmission.
#define	ENQ__CTRL_E                0x05 // Enquiry.
#define ACK__CTRL_F                0x06 // Acknowledge.
#define SO__CTRL_N                 0x0e // Shift out.
#define SI__CTRL_O                 0x0f // Shift in.
#define DLE_CTRL_P                 0x10 // Data link escape.
#define DC1__CTRL_Q                0x11 // Device control 1 (xon).
#define DC2__CTRL_R                0x12 // Device control 2.
#define DC3__CTRL_S                0x13 // Device control 3 (xoff).
#define DC4__CTRL_T                0x14 // Device control 4.
#define NAK__CTRL_U                0x15 // Negative acknowledge.
#define SYN__CTRL_V                0x16 // Synchronous idle.
#define ETB__CTRL_W                0x17 // End of transmission block.
#define CAN__CTRL_X                0x18 // Cancel.
#define EM__CTRL_Y                 0x19 // End of medium.
#define SUB__CTRL_Z                0x1a // Substitute.
#define ESC__CTRL_LEFT_SQR_BRACKET 0x1b // Escape for ANSI codes.
#define FS__CTRL_BACKSLASH         0x1c // File separator.
#define GS__CTRL_RIGHT_SQR_BRACKET 0x1d // Group separator.
#define RS__CTRL_CARRET            0x1e // Record separator.
#define US__CTRL_DASH              0x1f // Unit separator.
#define DEL__BACKSPACE             0x7f // Delete that is really backspace.

#define BUFFER_NOT_FULL      (Buff->lines_i < BUFF_MAX)
#define CURSOR_X_SCROLLED    (Buff->cusr_x > 0)
#define CURSOR_Y_SCROLLED    (Buff->cusr_y > 0)
#define EMPTY_LINE           (ACT_LINE_LEN_I == 0)
#define ONE_LINE             (ACT_LINE_I == 0)
#define CURSOR_AT_LINE_START (Buff->cusr_x == ACT_LINE_LEN_I)

// Doesn't work properly.
#define CR__CTRL_M 0x0d // Carriage return, converted to 10 (linefeed).

// memory.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool status);
extern void    chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);
extern void    safer_free(void* ptr);
extern char*   extend_line_mem(f_mtdt* Buff, buff_t line_i);
extern char*   shrink_act_line_mem(f_mtdt* Buff);
extern char*   shrink_prev_line_mem(f_mtdt* Buff);
extern f_mtdt* extend_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* shrink_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* copy_lines_mem_forward(f_mtdt* Buff);
extern f_mtdt* copy_lines_mem_backward(f_mtdt* Buff);

// file.h
extern f_mtdt* save_file(f_mtdt* Buff);

// edit.h
extern f_mtdt* delete_last_line      (f_mtdt* Buff);
extern f_mtdt* delete_line           (f_mtdt* Buff);
extern f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction);
extern f_mtdt* move_lines_forward    (f_mtdt* Buff);
extern f_mtdt* delete_last_empty_line(f_mtdt* Buff);
extern f_mtdt* delete_non_last_line  (f_mtdt* Buff);
extern f_mtdt* delete_char           (f_mtdt* Buff);

// Knows what to do next with pressed key or combination. Based on ASCII.
f_mtdt* key_action(f_mtdt* Buff, char key);

// Adds char when the pressed key is a printable one.
f_mtdt* printable_char(f_mtdt* Buff, char key);

// Initialize the new line.
f_mtdt* linefeed(f_mtdt* Buff);

// Removes a last char and optionally deletes the last line.
f_mtdt* backspace(f_mtdt* Buff);

#endif
