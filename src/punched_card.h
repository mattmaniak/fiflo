#ifndef PCARD_H
#define PCARD_H

// Punch card (wrap guide) rendering.

#include <stdio.h>

#include "v_file.h"
#include "config.h"
#include "ui.h"

// As a rendered char background.
void punched_card__print_at_offset(const Config* const, const size_t);

#endif
