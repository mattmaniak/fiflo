#ifndef KEYS_H
#define KEYS_H

// Formatting control chars.
#define NUL 0   // CTRL_@, terminator.
#define BEL 7   // CTRL_G, alarm beeper.
#define BS  8   // CTRL_H, fake backspace.
#define HT  9   // CTRL_I, horizontal tab.
#define LF  10  // CTRL_J, linefeed/newline.
#define VT  11  // CTRL_K, vertical tab.
#define FF  12  // CTRL_L, form feed.

// Control characters can be used as keyboard shortcuts.
#define STX 1   // CTRL_A, start of header
#define SOT 2   // CTRL_B, start of text.
#define EOT 4   // CTRL_D, end of transmission.
#define	ENQ 5   // CTRL_E, enquiry.
#define ACK 6   // CTRL_F, acknowledge.
#define SO  14  // CTRL_N, shift out.
#define SI  15  // CTRL_O, shift in.
#define DLE 16  // CTRL_P, data link escape.
#define DC2 18  // CTRL_R, device control 2.
#define DC4 20  // CTRL_T, device control 4.
#define NAK 21  // CTRL_U, negative acknowledge.
#define SYN 22  // CTRL_V, synchronous idle.
#define ETB 23  // CTRL_W, end of transmission block.
#define CAN 24  // CTRL_X, cancel.
#define EM  25  // CTRL_Y, end of medium.
#define ESC 27  // CTRL_[, escape for ANSI codes.
#define GS  29  // CTRL_], group separator.
#define RS  30  // CTRL_^, record separator.
#define US  31  // CTRL_-, unit separator.
#define DEL 127 // BACKSPACE, delete that is really backspace.

// Codes that doesn't work or breaks the program. TODO: IGNORE THEM.
#define ETX 3   // CTRL_C, end of text, overridden by the signal handler.
#define CR  13  // CTRL_M, carriage return, doesn't work.
#define DC1 17  // CTRL_Q, device control 1 (xon), doesn't work.
#define DC3 19  // CTRL_S, device control 3 (xoff), causes break.
#define SUB 26  // CTRL_Z, substitute, overridden by the signal handler.
#define FS  28  // CTRL_\, file separator, causes segmentation fault, STH GLOBAL.

#define NUL_SZ    1
#define SLASH_SZ  1
#define NEG      -1
#define CTRL_D    4
#define CTRL_X    24

// These keys moves the cursor...
#define CTRL_G 7 // left.
#define CTRL_H 8 // right.

// From logic.
extern void save_file(meta* Dt);
extern meta* shift_text_horizonally(char direction, meta* Dt);
extern meta* extend_act_line_mem(meta* Dt);
extern meta* extend_prev_line_mem(meta* Dt);
extern meta* shrink_act_line_mem(meta* Dt);
extern meta* extend_lines_array(meta* Dt);
extern meta* shrink_lines_array(meta* Dt);
extern meta* shrink_prev_line_mem(meta* Dt);

// Knows what to do next with pressed key or combination. Based on ASCII.
meta* recognize_key(char key, meta* Dt);

// Adds char when the pressed key is a printable one.
meta* text_char(char key, meta* Dt);

// Initialize the new line.
meta* linefeed(meta* Dt);

// Removes a last char and optionally deletes the last line.
meta* backspace(meta* Dt);

// Moves the cursor left.
meta* ctrl_g(meta* Dt);

// Moves the cursor right.
buf_t ctrl_h(buf_t cusr_x);

#endif

