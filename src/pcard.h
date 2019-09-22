#ifndef PCARD_H
#define PCARD_H

// Punch card (wrap guide) rendering.

#include <stdio.h>

#include "buffer.h"
#include "config.h"
#include "ui.h"

// As a rendered char background.
void pcard__print_at_offset(const Conf_t* const, const idx_t);

// At the right side of a text.
void pcard__print_after_txt(const Conf_t* const, const Ui_t* const,
                            const char* const, const idx_t);

// In a place where there is no line.
void pcard__print_after_nothing(const Conf_t* const, const Ui_t* const);

#endif
