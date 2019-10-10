#include "punched_card.h"

void punched_card__print_at_offset(const Config* const config,
                                   const size_t char_i)
{
    if(char_i == (size_t) (config->Punched_card_width.value - SIZE__I))
    {
        ANSI__INVERT();
    }
}
