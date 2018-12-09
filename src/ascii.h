#ifndef ASCII_H
#define ASCII_H

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

// Doesn't work properly.
#define CR__CTRL_M 0x0d // Carriage return, converted to 10 (linefeed).

#define NUL_SZ   1

#endif
