#ifndef KEYS_H
#define KEYS_H

#define LN_ABOVE Dat->txt[Dat->lns - 1]

extern meta* freeblk(meta* Dat);
extern meta* txtshift(meta* Dat, char direction);

meta* ctrlh(meta* Dat);
meta* ctrlg(meta* Dat);
meta* ctrly(meta* Dat);
meta* ctrlb(meta* Dat);
meta* backspace(meta* Dat);
#endif

