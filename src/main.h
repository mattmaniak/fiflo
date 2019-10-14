// Some shit that starts the program.

#include <stdbool.h>

#include "args.h"
#include "v_file.h"
#include "config.h"
#include "filename.h"
#include "extension.h"
#include "modes.h"
#include "input.h"
#include "syntax.h"
#include "window.h"
#include "options.h"

// Initialize the app.
bool init(V_file*, Config* const, Modes* const, Syntax*, char**,
          size_t* const);
