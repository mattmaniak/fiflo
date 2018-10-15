#ifndef KEYS_H
#define KEYS_H

extern meta* freeblk(meta* Dt);
extern meta* txtshift(char direction, meta* Dt);

meta* ctrlh(meta* Dt);
meta* ctrlg(meta* Dt);
meta* ctrly(meta* Dt);
meta* ctrlb(meta* Dt);
meta* addchar(char key, meta* Dt);
meta* linefeed(meta* Dt);
meta* backspace(meta* Dt);
#endif

