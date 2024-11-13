// Some shit that starts the program.

#include <stdbool.h>

#include "args.h"
#include "config.h"
#include "extension.h"
#include "filename.h"
#include "input.h"
#include "modes.h"
#include "options.h"
#include "syntax.h"
#include "window.h"
#include "v_file.h"

// Initialize the app.
bool init(V_file*, Config* const, Modes* const, Syntax*, char**,
          size_t* const);
