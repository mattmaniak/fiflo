#ifndef FILES_H
#define FILES_H

#define NTERM_SZ 1
#define NTERM 0
#define SLASH_SZ 1

extern buf* charadd(buf* dt, char key);

void fnameset(buf* dt, const char* passed); // Optionally get current dir.
buf* readfile(buf* dt);
void savefile(buf* dt);

#endif

