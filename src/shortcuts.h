#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#include "config.h"
#include "modes.h"

// Ancient ASCII control characters that can be used as keyboard shortcuts.

#define CTRL_A             1   // Start of header
#define CTRL_B             2   // Start of text.
#define CTRL_C             3   // End of text.
#define CTRL_D             4   // End of transmission.
#define	CTRL_E             5   // Enquiry.
#define CTRL_F             6   // Acknowledge.
#define CTRL_M             13  // Carriage return. Converted to linefeed (10).
#define CTRL_N             14  // Shift out.
#define CTRL_O             15  // Shift in.
#define CTRL_P             16  // Data link escape.
#define CTRL_Q             17  // Device control 1 (xon).
#define CTRL_R             18  // Device control 2.
#define CTRL_S             19  // Device control 3 (xoff).
#define CTRL_T             20  // Device control 4.
#define CTRL_U             21  // Negative acknowledge.
#define CTRL_V             22  // Synchronous idle.
#define CTRL_W             23  // End of transmission block.
#define CTRL_X             24  // Cancel.
#define CTRL_Y             25  // End of medium.
#define CTRL_Z             26  // Substitute.
#define CTRL_LEFT_BRACKET  27  // Escape for ANSI codes.
#define CTRL_BACKSLASH     28  // File separator.
#define CTRL_RIGHT_BRACKET 29  // Group separator.
#define CTRL_CARRET        30  // Record separator.
#define CTRL_DASH          31  // Unit separator.
#define BACKSPACE          127 // Delete. Backspace for real.

// These ones moves the cursor.
void shortcuts__arrow_left(Buff_t*, Conf_t*, Mod_t*);
void shortcuts__arrow_right(Buff_t*, Conf_t*, Mod_t*);
void shortcuts__arrow_up(Buff_t*);
void shortcuts__arrow_down(Buff_t*);

// Scrolls more intensively.
void shortcuts__ctrl__arrow_up(Buff_t*);
void shortcuts__ctrl__arrow_down(Buff_t*);

#endif
