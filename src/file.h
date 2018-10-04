#ifndef FILE_H
#define FILE_H

#define NTERM_SZ 1
#define NTERM    0
#define SLASH_SZ 1

extern meta* add_char(meta* Dat, char key);

void set_fname(meta* Dat, const char* passed); // Optionally get current dir.
meta* read_file(meta* Dat);
void save_file(meta* Dat);
#endif

