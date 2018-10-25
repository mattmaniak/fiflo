#ifndef KEYS_H
#define KEYS_H

#define NTERM_SZ  1
#define SLASH_SZ  1
#define NEG      -1
#define NTERM     0 // Also CTRL+@ - CTRL+SHIFT+2.
#define CTRL_D    4
#define TAB       9
#define LF        10
#define CTRL_X    24
#define ESCAPE    27 // '\033'
#define BACKSPACE 127

// From logic.
extern meta* shift_txt_horizonally(char direction, meta* Dt);
extern meta* extend_curr_ln_mem(meta* Dt);
extern meta* shrink_curr_ln_mem(meta* Dt);
extern meta* extend_lns_array(meta* Dt);
extern meta* shrink_lns_array(meta* Dt);
extern meta* shrink_prev_ln_mem(meta* Dt);

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

