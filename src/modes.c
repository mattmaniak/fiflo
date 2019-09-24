#include "modes.h"

void modes__init(Mod_t* const Modes)
{
    Modes->live_fname_edit = false;
    Modes->expanded_lbar   = false;
    Modes->tabs_to_spaces  = false;
}
