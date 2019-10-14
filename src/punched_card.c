#include "punched_card.h"

void punched_card__print_at_offset(const Config* const config,
                                   const size_t char_i)
{
    // Invert colors if a char has an offset of punched card width.
    if(char_i == (size_t) (config->punched_card_width.value - SIZE__I))
    {
        ANSI__INVERT();
    }
}
