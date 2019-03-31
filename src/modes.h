#ifndef MODES_H
#define MODES_H

#include <stdbool.h>

typedef struct
{
    bool live_fname_edit; // As in the name.
    bool lbar_toggled;    // As in the name.
}
Mod_t;

// Set the runtime parameters of the editor.
void modes__init(Mod_t* Modes);

#endif
