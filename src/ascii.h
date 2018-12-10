#ifndef ASCII_H
#define ASCII_H

// Control characters that can be used as keyboard shortcuts.
#define CTRL_A                 0x01 // Start of header
#define CTRL_B                 0x02 // Start of text.
#define CTRL_C                 0x03 // End of text.
#define CTRL_D                 0x04 // End of transmission.
#define	CTRL_E                 0x05 // Enquiry.
#define CTRL_F                 0x06 // Acknowledge.
#define CTRL_N                 0x0e // Shift out.
#define CTRL_O                 0x0f // Shift in.
#define CTRL_P                 0x10 // Data link escape.
#define CTRL_Q                 0x11 // Device control 1 (xon).
#define CTRL_R                 0x12 // Device control 2.
#define CTRL_S                 0x13 // Device control 3 (xoff).
#define CTRL_T                 0x14 // Device control 4.
#define CTRL_U                 0x15 // Negative acknowledge.
#define CTRL_V                 0x16 // Synchronous idle.
#define CTRL_W                 0x17 // End of transmission block.
#define CTRL_X                 0x18 // Cancel.
#define CTRL_Y                 0x19 // End of medium.
#define CTRL_Z                 0x1a // Substitute.
#define CTRL_LEFT_SQR_BRACKET  0x1b // Escape for ANSI codes.
#define CTRL_BACKSLASH         0x1c // File separator.
#define CTRL_RIGHT_SQR_BRACKET 0x1d // Group separator.
#define CTRL_CARRET            0x1e // Record separator.
#define CTRL_DASH              0x1f // Unit separator.
#define BACKSPACE              0x7f // Delete that is really backspace.

// Doesn't work properly.
#define CR__CTRL_M 0x0d // Carriage return, converted to 10 (linefeed).

#define NUL_SZ   1

#endif
