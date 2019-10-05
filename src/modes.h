#ifndef MODES_H
#define MODES_H

// Possible states of the editor.

#include <stdbool.h>

typedef struct
{
    bool live_fname_edit; // A filename is edited, not a text.
    bool expanded_lbar;   // An extended lower bar is displayed.
    bool tabs_to_spaces;
}
Modes;

// Initialize runtime parameters of the editor.
void modes__init(Modes* const);

#endif
