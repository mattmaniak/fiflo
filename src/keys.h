#ifndef KEYS_H
#define KEYS_H

#include "config.h"
#include "modes.h"

// Ancient ASCII control characters that can be used as keyboard shortcuts.

enum
{
    KEYS__CTRL_A = 1,         // Start of header
    KEYS__CTRL_B,             // Start of text.
    KEYS__CTRL_C,             // End of text.
    KEYS__CTRL_D,             // End of transmission.
    KEYS__CTRL_E,             // Enquiry.
    KEYS__CTRL_F,             // Acknowledge.
    KEYS__CTRL_M = 13,        // Carriage return. Converted to linefeed (10).
    KEYS__CTRL_N,             // Shift out.
    KEYS__CTRL_O,             // Shift in.
    KEYS__CTRL_P,             // Data link escape.
    KEYS__CTRL_Q,             // Device control 1 (xon).
    KEYS__CTRL_R,             // Device control 2.
    KEYS__CTRL_S,             // Device control 3 (xoff).
    KEYS__CTRL_T,             // Device control 4.
    KEYS__CTRL_U,             // Negative acknowledge.
    KEYS__CTRL_V,             // Synchronous idle.
    KEYS__CTRL_W,             // End of transmission block.
    KEYS__CTRL_X,             // Cancel.
    KEYS__CTRL_Y,             // End of medium.
    KEYS__CTRL_Z,             // Substitute.
    KEYS__CTRL_LEFT_BRACKET,  // Escape for ANSI codes.
    KEYS__CTRL_BACKSLASH,     // File separator.
    KEYS__CTRL_RIGHT_BRACKET, // Group separator.
    KEYS__CTRL_CARRET,        // Record separator.
    KEYS__CTRL_DASH,          // Unit separator.
    KEYS__BACKSPACE = 127,    // Delete. Backspace for real.
};


// These ones move the cursor.
void keys__arrow_left(Buff_t* const, const Conf_t* const);
void keys__arrow_right(Buff_t* const, const Conf_t* const);
void keys__arrow_up(Buff_t* const);
void keys__arrow_down(Buff_t* const);

// Skip whole words.
void keys__ctrl_arrow_left(Buff_t* const);
void keys__ctrl_arrow_right(Buff_t* const);

// Scroll to the beginning/end of a file.
void keys__ctrl_arrow_up(Buff_t* const);
void keys__ctrl_arrow_down(Buff_t* const);

#endif
