#ifndef KEYS_H
#define KEYS_H

extern meta* txtshift(char direction, meta* Dt);
extern meta* chrsalloc(meta* Dt);

meta* ctrlh(meta* Dt);
meta* ctrlg(meta* Dt);
meta* addchar(char key, meta* Dt);
meta* linefeed(meta* Dt);
meta* backspace(meta* Dt);
#endif

