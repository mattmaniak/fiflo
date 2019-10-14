#ifndef ASCII_H
#define ASCII_H

enum
{
    ASCII__CTRL_A = 1,         // Start of header
    ASCII__CTRL_B,             // Start of text.
    ASCII__CTRL_C,             // End of text.
    ASCII__CTRL_D,             // End of transmission.
    ASCII__CTRL_E,             // Enquiry.
    ASCII__CTRL_F,             // Acknowledge.
    ASCII__CTRL_M = 13,        // Carriage return. Converted to linefeed (10).
    ASCII__CTRL_N,             // Shift out.
    ASCII__CTRL_O,             // Shift in.
    ASCII__CTRL_P,             // Data link escape.
    ASCII__CTRL_Q,             // Device control 1 (xon).
    ASCII__CTRL_R,             // Device control 2.
    ASCII__CTRL_S,             // Device control 3 (xoff).
    ASCII__CTRL_T,             // Device control 4.
    ASCII__CTRL_U,             // Negative acknowledge.
    ASCII__CTRL_V,             // Synchronous idle.
    ASCII__CTRL_W,             // End of transmission block.
    ASCII__CTRL_X,             // Cancel.
    ASCII__CTRL_Y,             // End of medium.
    ASCII__CTRL_Z,             // Substitute.
    ASCII__CTRL_LEFT_BRACKET,  // Escape for ANSI codes.
    ASCII__CTRL_BACKSLASH,     // File separator.
    ASCII__CTRL_RIGHT_BRACKET, // Group separator.
    ASCII__CTRL_CARRET,        // Record separator.
    ASCII__CTRL_DASH,          // Unit separator.
    ASCII__BACKSPACE = 127,    // Delete. Backspace for real.
};

#endif
