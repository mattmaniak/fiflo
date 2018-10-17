#ifndef KEYS_H
#define KEYS_H

// From render.
extern meta* shift_txt_horizonally(char direction, meta* Dt);
extern meta* add_mem_for_chrs(meta* Dt);
extern meta* free_mem_for_chrs(meta* Dt);

// Adds char when the pressed key is a printable one.
meta* add_chr_as_txt(char key, meta* Dt);

// Initialize the new line.
meta* linefeed(meta* Dt);

// Removes a last char and optionally deletes the last line.
meta* backspace(meta* Dt);

// Moves the cursor left.
meta* ctrl_g(meta* Dt);

// Moves the cursor right.
meta* ctrl_h(meta* Dt);
#endif

