#ifndef KEYS_H
#define KEYS_H

#define LN_ABOVE dt->txt[dt->lns - 1]

extern meta* dealloc_block(meta* dt);
extern meta* shift_txt(meta* dt, char direction);

meta* ctrl_h(meta* dt);
meta* ctrl_g(meta* dt);
meta* ctrl_y(meta* dt);
meta* ctrl_b(meta* dt);
meta* backspace(meta* dt);
#endif

