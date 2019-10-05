#ifndef PCARD_H
#define PCARD_H

// Punch card (wrap guide) rendering.

#include <stdio.h>

#include "v_file.h"
#include "config.h"
#include "ui.h"

// As a rendered char background.
void pcard__print_at_offset(const Config* const, const size_t);

// At the right side of a text.
void pcard__print_after_txt(const Config* const, const Ui* const,
                            const char* const, const size_t);

// In a place where there is no line.
void pcard__print_after_nothing(const Config* const, const Ui* const);

#endif
