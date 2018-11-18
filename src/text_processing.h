#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

// Formatting control chars.
#define NUL__CTRL_SHIFT_2 0  // Terminator.
#define BEL__CTRL_G       7  // Alarm beeper.
#define BS__CTRL_H        8  // Fake backspace.
#define HT__CTRL_I        9  // Horizontal tab.
#define LF__CTRL_J        10 // Linefeed/newline.
#define VT__CTRL_K        11 // Vertical tab.
#define FF__CTRL_L        12 // Form feed.

// Control characters can be used as keyboard shortcuts.
#define STX__CTRL_A                 1   // Start of header
#define SOT__CTRL_B                 2   // Start of text.
#define EOT__CTRL_D                 4   // End of transmission.
#define	ENQ__CTRL_E                 5   // Enquiry.
#define ACK__CTRL_F                 6   // Acknowledge.
#define SO__CTRL_N                  14  // Shift out.
#define SI__CTRL_O                  15  // Shift in.
#define DLE_CTRL_P                  16  // Data link escape.
#define DC1__CTRL_Q                 17  // Device control 1 (xon).
#define DC2__CTRL_R                 18  // Device control 2.
#define DC3__CTRL_S                 19  // Device control 3 (xoff).
#define DC4__CTRL_T                 20  // Device control 4.
#define NAK__CTRL_U                 21  // Negative acknowledge.
#define SYN__CTRL_V                 22  // Synchronous idle.
#define ETB__CTRL_W                 23  // End of transmission block.
#define CAN__CTRL_X                 24  // Cancel.
#define EM__CTRL_Y                  25  // End of medium.
#define ESC__CTRL_RIGHT_SQR_BRACKET 27  // Escape for ANSI codes.
#define GS__CTRL_LEFT_SQR_BRACKET   29  // Group separator.
#define RS__CTRL_CARRET             30  // Record separator.
#define US__CTRL_DASH               31  // Unit separator.
#define DEL__BACKSPACE              127 // Delete that is really backspace.

// Codes that doesn't work or breaks the program.
#define ETX__CTRL_C        3  // End of text, used by the signal handler.
#define CR__CTRL_M         13 // Carriage return, probably ignored by getchar.
#define SUB__CTRL_Z        26 // Substitute, used by the signal handler.
#define FS__CTRL_BACKSLASH 28 // File separator, used by the signal handler.

#define NEG     -1 // Pipe.
#define NUL_SZ   1
#define SLASH_SZ 1

// file.h
extern f_mtdt* save_file(f_mtdt* Buff);

// memory.h
extern char*   extend_line       (f_mtdt* Buff, buff_t line);
extern char*   shrink_act_line   (f_mtdt* Buff);
extern char*   shrink_prev_line  (f_mtdt* Buff);
extern f_mtdt* extend_lines_array(f_mtdt* Buff);
extern f_mtdt* shrink_lines_array(f_mtdt* Buff);

// Knows what to do next with pressed key or combination. Based on ASCII.
f_mtdt* recognize_key(f_mtdt* Buff, char key);

// Adds char when the pressed key is a printable one.
f_mtdt* text_char(f_mtdt* Buff, char key);

// Initialize the new line.
f_mtdt* linefeed(f_mtdt* Buff);

// Removes a last char and optionally deletes the last line.
f_mtdt* backspace(f_mtdt* Buff);

// Moves the cursor left.
f_mtdt* ctrl_g(f_mtdt* Buff);

// Moves the cursor right.
f_mtdt* ctrl_h(f_mtdt* Buff);

// Moves the cursor up.
f_mtdt* ctrl_y(f_mtdt* Buff);

// Moves the cursor down.
f_mtdt* ctrl_b(f_mtdt* Buff);

// Moves the text when the cursor is moved left and char is pressed.
f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction);

#endif

