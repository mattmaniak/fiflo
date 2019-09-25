#ifndef ARROWS_H
#define ARROWS_H

// Ancient ASCII control characters that can be used as keyboard shortcuts.

#include "v_file.h"
#include "ascii.h"
#include "config.h"
#include "modes.h"

// These ones move the cursor.
void arrows__arrow_left(V_file_t* const, const Conf_t* const);
void arrows__arrow_right(V_file_t* const, const Conf_t* const);
void arrows__arrow_up(V_file_t* const);
void arrows__arrow_down(V_file_t* const);

// Skip whole words.
void arrows__ctrl_arrow_left(V_file_t* const);
void arrows__ctrl_arrow_right(V_file_t* const);

// Scroll to the beginning/end of a file.
void arrows__ctrl_arrow_up(V_file_t* const);
void arrows__ctrl_arrow_down(V_file_t* const);

#endif
