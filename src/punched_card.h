#ifndef PUNCHED_CARD_H
#define PUNCHED_CARD_H

// Punch card (wrap guide) rendering.

#include <stdio.h>

#include "config.h"
#include "ui.h"
#include "v_file.h"

// As a rendered char background.
void punched_card__print_at_offset(const Config* const, const size_t);

#endif
