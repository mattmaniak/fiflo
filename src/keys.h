#ifndef KEYS_H
#define KEYS_H

#define LN_ABOVE Dat->txt[Dat->lns - 1]

extern meta* dealloc_block(meta* Dat);
extern meta* shift_txt(meta* Dat, char direction);

meta* ctrl_h(meta* Dat);
meta* ctrl_g(meta* Dat);
meta* ctrl_y(meta* Dat);
meta* ctrl_b(meta* Dat);
meta* backspace(meta* Dat);
#endif

