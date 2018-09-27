#ifndef FILES_H
#define FILES_H

#define NTERM_SZ 1
#define NTERM 0
#define CURRLN dt->txt[dt->lns]

#define SLASH_SZ 1

extern _Noreturn void freeallexit(meta* dt, _Bool code);
extern meta* charadd(meta* dt, char key);

void fnameset(meta* dt, const char* passed); // Optionally get current dir.
meta* readfile(meta* dt);
void savefile(meta* dt);

#endif

