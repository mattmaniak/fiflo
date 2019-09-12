#ifndef MODES_H
#define MODES_H

#include <stdbool.h>

typedef struct
{
    bool live_fname_edit; // A filename is edited, not a text.
    bool expanded_lbar;   // An extended lower bar is displayed.
}
Mod_t;

// Initialize runtime parameters of the editor.
void modes__init(Mod_t* const);

#endif
