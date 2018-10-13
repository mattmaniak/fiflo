#ifndef KEYS_H
#define KEYS_H

extern meta* allocblk(meta* Dt, char mode);
extern meta* freeblk(meta* Dt);
extern meta* txtshift(meta* Dt, char direction);

meta* ctrlh(meta* Dt);
meta* ctrlg(meta* Dt);
meta* ctrly(meta* Dt);
meta* ctrlb(meta* Dt);
meta* backspace(meta* Dt);
meta* linefeed(meta* Dt);
#endif

