#ifndef FILES_H
#define FILES_H

#define NTERM_SZ 1
#define NTERM    0
#define SLASH_SZ 1

extern meta* add_char(meta* dt, char key);

void set_fname(meta* dt, const char* passed); // Optionally get current dir.
meta* read_file(meta* dt);
void save_file(meta* dt);
#endif

