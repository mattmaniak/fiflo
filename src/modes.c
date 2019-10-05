#include "modes.h"

void modes__init(Modes* const modes)
{
    modes->live_fname_edit = false;
    modes->expanded_lbar   = false;
    modes->tabs_to_spaces  = false;
}
