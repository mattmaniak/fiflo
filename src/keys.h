#ifndef KEYS_H
#define KEYS_H

// TODO
// Control characters and format effectors.
#define NUL 0   // Terminator.
#define BEL 7   // CTRL_G, alarm beeper.
#define BS  8   // CTRL_H, fake backspace.
#define HT  9   // Horizontal tab.
#define LF  10  // Linefeed/newline.
#define VT  11  // Vertical tab.
#define FF  12  // Form feed.
#define CR  13  // Carriage return.
#define SO  14  // Shift out.
#define SI  15  // Shift in.
#define EM  25  // End of medium.
#define SUB 26  // Substitute.
#define ESC 27  // Escape for ANSI codes.
#define FS  28  // File separator.
#define GS  29  // Group separator.
#define RS  30  // Record separator.
#define US  31  // Unit separator.
#define DEL 127 // Delete that is really backspace.

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
extern meta* extend_act_ln_mem(meta* Dt);
extern meta* shrink_act_ln_mem(meta* Dt);
extern meta* extend_lines_array(meta* Dt);
extern meta* shrink_lines_array(meta* Dt);
extern meta* shrink_prev_ln_mem(meta* Dt);

// Knows what to do next with pressed key or combination. Based on ASCII.
meta* recognize_key(char key, meta* Dt);

// Adds char when the pressed key is a printable one.
meta* non_control_chr(char key, meta* Dt);

// Initialize the new line.
meta* linefeed(meta* Dt);

// Removes a last char and optionally deletes the last line.
meta* backspace(meta* Dt);

// Moves the cursor left.
meta* ctrl_g(meta* Dt);

// Moves the cursor right.
meta* ctrl_h(meta* Dt);
#endif

