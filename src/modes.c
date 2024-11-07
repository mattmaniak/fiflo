#include "modes.h"

void modes__init(Modes* const modes)
{
    modes->expanded_lbar   = false;
    modes->live_fname_edit = false;
    modes->tabs_to_spaces  = false;
}
